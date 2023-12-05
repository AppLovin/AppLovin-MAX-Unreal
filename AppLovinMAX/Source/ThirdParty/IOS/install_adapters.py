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
dependencies = set()
seen = set(["AppLovinSDK"])


""" Classes """


class Pod:
    def __init__(self, spec):
        self.name = spec["name"]
        self.version = spec.get("version", None)

        source = spec.get("source", {})
        if "http" in source:
            self.url = source["http"]
        elif "git" in source:
            self.url = source["git"]
        else:
            self.url = None

        self.vendored_frameworks = spec.get("vendored_frameworks", [])
        if isinstance(self.vendored_frameworks, str):
            self.vendored_frameworks = [self.vendored_frameworks]

        self.frameworks = spec.get("frameworks", [])
        if isinstance(self.frameworks, str):
            self.frameworks = [self.frameworks]

        self.weak_frameworks = spec.get("weak_frameworks", [])
        if isinstance(self.weak_frameworks, str):
            self.weak_frameworks = [self.weak_frameworks]

        self.dependencies = spec.get("dependencies", {})
        self.subspecs = list(map(Pod, spec.get("subspecs", [])))

        # TODO: Libraries


""" Utilities """


def run_shell(command):
    process = subprocess.run(command.split(), capture_output=True)
    return process.stdout.decode().strip()


def print_instruction(instruction):
    if not hasattr(print_instruction, "counter"):
        print_instruction.counter = 1

    print(f"{print_instruction.counter}. {instruction}\n")

    print_instruction.counter += 1


""" Installation """


def parse_adapters():
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
    # TODO: Make this support ranged version?
    exact_version = re.search(r'^=\s?(\d+(\.\d+)*)', version)
    if exact_version:
        command += f" --version={exact_version.group(1)}"

    output = run_shell(command)
    spec = json.loads(output)
    return Pod(spec)


def install_pod(pod):
    if pod.name in seen:
        return

    seen.add(pod.name)

    # Download dependency
    # if source exists
    # zip_path = install_dir / f"{pod.name}.zip"
    # urlretrieve(pod.url, zip_path)
    # with ZipFile(zip_path, 'r') as zip_file:
    #     zip_file.extractall(install_dir)
    # zip_path.unlink()

    install_dependencies(pod)

    # Install dependencies for subspec
    for subspec in pod.subspecs:
        install_dependencies(subspec)


def install_dependencies(pod):
    # Parse dependency info and install pods
    for d, version in pod.dependencies.items():
        # Split the dependency into main name and subspec
        name, subspec = d.split("/") if "/" in d else (d, None)
        if name not in seen:
            d_pod = get_pod_info(
                name, version[0] if len(version) == 1 else "")
            install_pod(d_pod)


def install_adapters():
    install_dir.mkdir(parents=True, exist_ok=True)

    count = 0
    for adapter in parse_adapters():
        try:
            install_pod(adapter)
            count += 1
            print(f"✔ Installed '{adapter.name}'")
        except Exception as e:
            print(f"✘ Failed to install '{adapter.name}'\n\t{e}")

    return count


""" Main """


def main():
    print("\nThis script installs AppLovin adapters and third-party SDKs using your Podfile.\n")
    print("Please refer to our documentation to complete your integration:")
    print("https://dash.applovin.com/documentation/mediation/unreal/mediation-adapters/ios\n")

    print("> Updating CocoaPods repos... (this may take a while)")
    # run_shell("pod --silent repo update")

    print("> Installing adapters...\n")
    installed_count = install_adapters()

    print(f"\n> Installed {installed_count} adapter(s)")

    print(sorted(seen))
    # print("\n***************************************************\n")

    # print("The script will now generate build rules you must copy into AppLovinMAX.Build.cs.\n")
    # input("Press any key to continue...")
    # print()

    # print_instruction(
    # "Copy the following iOS build rules into AppLovinMAX.Build.cs:")
    # print("\n".join(build_rules))

    # print_instruction(
    # "Replace the existing PublicFrameworks with the following in AppLovinMAX.Build.cs:")
    # formatted_frameworks = ',\n'.join(
    # sorted(map(lambda f: f"\t\t\"{f}\"", all_frameworks)))
    # print("PublicFrameworks.AddRange(\n\tnew string[]\n\t{{\n{0}\n\t}}\n);".format(
    # formatted_frameworks))

    # print_instruction(
    # f"Add the following build rules to AppLovinMAX.Build.cs:")


if __name__ == "__main__":
    main()
