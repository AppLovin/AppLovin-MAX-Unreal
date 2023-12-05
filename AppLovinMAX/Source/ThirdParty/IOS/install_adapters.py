#!/usr/bin/env python3
#
# This script downloads AppLovin adapters and dependencies for iOS and generates build settings for use in UE projects.
#
# Copyright © 2021 AppLovin Corporation. All rights reserved.
#

import json
import re
import shutil
import subprocess

from urllib.request import urlretrieve
from pathlib import Path
from zipfile import ZipFile


""" Globals """


install_dir = Path("Adapters")
build_rules = set()
seen = set()


""" Classes """


class Pod:
    all_frameworks = set()
    all_weak_frameworks = set()

    def __init__(self, spec, parent=""):
        self.name = (f"{parent}/" if parent else "") + spec["name"]
        self.version = spec.get("version", None)

        source = spec.get("source", {})
        if "http" in source:
            self.url = source["http"]
            self.source = "http"
        elif "git" in source:
            self.url = source["git"]
            self.source = "git"
        else:
            self.url = None
            self.source = None

        self.vendored_frameworks = spec.get("vendored_frameworks", [])
        if isinstance(self.vendored_frameworks, str):
            self.vendored_frameworks = [self.vendored_frameworks]

        self.frameworks = spec.get("frameworks", [])
        if isinstance(self.frameworks, str):
            self.frameworks = [self.frameworks]

        Pod.all_frameworks.update(self.frameworks)

        self.weak_frameworks = spec.get("weak_frameworks", [])
        if isinstance(self.weak_frameworks, str):
            self.weak_frameworks = [self.weak_frameworks]

        Pod.all_weak_frameworks.update(self.weak_frameworks)

        self.dependencies = spec.get("dependencies", {})
        self.subspecs = list(
            map(lambda s: Pod(s, self.name), spec.get("subspecs", [])))

        # TODO: Libraries


""" Utilities """


def run_shell(command):
    process = subprocess.run(command, capture_output=True)
    stderr = process.stderr.decode()
    if len(stderr) != 0:
        raise Exception(f"Failed to execute command: {command}\n{stderr}")
    else:
        return process.stdout.decode().strip()


def print_instruction(instruction):
    if not hasattr(print_instruction, "counter"):
        print_instruction.counter = 1

    print(f"{print_instruction.counter}. {instruction}\n")

    print_instruction.counter += 1


""" Installation """


def parse_podfile():
    with open("Podfile") as podfile:
        # Trim whitespace
        pods = map(lambda l: l.strip(), podfile.readlines())

        # Extract lines for pods
        pods = filter(lambda l: l.startswith("pod"), pods)

        # Get pod names
        pods = map(lambda l: l.split(' ', 1)[1].strip("\'"), pods)

        # Only get AppLovin adapters
        pods = filter(lambda l: l.startswith("AppLovinMediation"), pods)

    return list(map(get_pod_info, pods))


def get_pod_info(name, version=""):
    command = f"pod spec cat --regex {name}"

    # Extract exact version requirement if exists
    exact_version = re.search(r'^=\s?(\d+(\.\d+)*)', version)
    if exact_version:
        command += f" --version={exact_version.group(1)}"

    output = run_shell(command.split())
    spec = json.loads(output)
    return Pod(spec)


def install_pod(pod):
    if pod.name in seen:
        return

    seen.add(pod.name)

    # Download dependency
    if pod.name != "AppLovinSDK" and pod.url is not None:
        if pod.source == "http":
            zip_path = install_dir / f"{pod.name}.zip"
            urlretrieve(pod.url, zip_path)

            with ZipFile(zip_path, 'r') as zip_file:
                zip_file.extractall(install_dir / pod.name)

            zip_path.unlink()
        elif pod.source == "git":
            run_shell(["git", "clone", pod.url, install_dir / pod.name])

    create_build_rule(pod)
    install_dependencies(pod)
    install_subspecs(pod)


def install_dependencies(pod):
    for dependency, version in pod.dependencies.items():
        # Split the dependency into main pod name and subspec
        # Use the main pod name to install the dependency
        name, subspec = dependency.split("/", 1) \
            if "/" in dependency \
            else (dependency, None)

        if name not in seen:
            dependency_pod = get_pod_info(
                name, version[0] if len(version) == 1 else "")
            install_pod(dependency_pod)


def install_subspecs(pod):
    for subspec in pod.subspecs:
        create_build_rule(subspec)
        install_dependencies(subspec)
        install_subspecs(subspec)  # can have nested subspecs


def install_adapters():
    install_dir.mkdir(parents=True, exist_ok=True)

    count = 0
    for adapter in parse_podfile():
        # try:
        install_pod(adapter)
        count += 1
        print(f"✔ Installed '{adapter.name}'")
        # except Exception as e:
        # print(f"✘ Failed to install '{adapter.name}'\n\t{e}")

    return count


def create_build_rule(pod):
    if pod.name == "AppLovinSDK":
        return

    # Check install directory for pod
    # 1. If it's a regular pod, the vendored frameworks will be the path to framework
    # 2. If it's a pod with subspecs, each subspec has it's venggdored framework path
    # 3. If no vendored framework or subspecs, could be source file based
    #  - Let's just accept that only a subset of adapters will work with installer script
    #  - CriteoPublisherSdk
    # Locate any Resources bundle
    # Get name of vendored frameworks

    # if len(pod.vendored_frameworks) == len(pod.subspecs) == 0:
    # pass

    for vf in pod.vendored_frameworks:
        build_rules.add(vf)


""" Main """


def main():
    print("\nThis script installs AppLovin adapters and third-party SDKs using your Podfile.\n")
    print("Please refer to our documentation to complete your integration:")
    print("https://dash.applovin.com/documentation/mediation/unreal/mediation-adapters/ios\n")

    print("> Updating CocoaPods repos... (this may take a while)")
    run_shell("pod --silent repo update".split())

    print("> Installing adapters...\n")
    installed_count = install_adapters()

    print(f"\n> Installed {installed_count} adapter(s)")

    print(sorted(seen))
    # print("\n***************************************************\n")

    # print("The script will now generate build rules you must copy into AppLovinMAX.Build.cs.\n")
    # input("Press any key to continue...")
    # print()

    if len(Pod.all_frameworks) != 0:
        print_instruction(
            "Replace the existing PublicFrameworks with the following:")
        formatted_frameworks = ',\n'.join(
            sorted(map(lambda f: f"\t\t\"{f}\"", Pod.all_frameworks)))
        print("PublicFrameworks.AddRange(\n\tnew string[]\n\t{{\n{0}\n\t}}\n);".format(
            formatted_frameworks))
        print()

    if len(Pod.all_weak_frameworks) != 0:
        print_instruction(
            "Copy the following rule for PublicWeakFrameworks:")
        formatted_weak_frameworks = ',\n'.join(
            sorted(map(lambda f: f"\t\t\"{f}\"", Pod.all_weak_frameworks)))
        print("PublicWeakFrameworks.AddRange(\n\tnew string[]\n\t{{\n{0}\n\t}}\n);".format(
            formatted_weak_frameworks))
        print()

    print_instruction(
        "Copy the following build rules to add dependencies:")
    print("\n".join(build_rules))


if __name__ == "__main__":
    main()
