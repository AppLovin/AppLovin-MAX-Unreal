#!/usr/bin/env python3
#
# This script downloads iOS dependencies from a Podfile and generates build settings for use in UE projects.
# You must run this script in the AppLovinMAX/Source/ThirdParty/IOS directory.
#
# Copyright © 2021 AppLovin Corporation. All rights reserved.
#

import json
import re
import shutil
import subprocess
import tarfile

from urllib.request import urlretrieve
from pathlib import Path
from zipfile import ZipFile


""" Globals """


install_dir = Path("Pods")
build_rules = set()
manual_pods = set()
seen = set()


""" Classes """


class Pod:
    all_frameworks = set()
    all_weak_frameworks = set()
    all_libraries = set()

    def __init__(self, spec, parent=None):
        self.name = (f"{parent.name}/" if parent else "") + spec["name"]
        self.version = spec.get("version", None)
        self.resources = spec.get("resources", None)

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

        # Path within downloaded dependency directory tree to vendored framework
        self.framework_path = spec.get("vendored_frameworks", "")
        if isinstance(self.framework_path, list):
            self.framework_path = self.framework_path[0]

        if "module_name" in spec:
            self.module_name = spec["module_name"]
        elif len(self.framework_path) != 0:
            self.module_name = self.framework_path.split("/")[-1]
        else:
            self.module_name = self.name

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
            map(lambda s: Pod(s, self), spec.get("subspecs", [])))

        self.libraries = spec.get("libraries", [])
        if isinstance(self.libraries, str):
            self.libraries = [self.libraries]

        if len(self.libraries) != 0:
            if "c++" in self.libraries:
                self.libraries.remove("c++")
            if "c++abi" in self.libraries:
                self.libraries.remove("c++abi")
            if "z" in self.libraries:
                self.libraries.remove("z")  # Already used by AppLovin SDK

            # TODO: Add correct libs for resolv, xml2, iconv, bz2

            Pod.all_libraries.update(self.libraries)


""" Utilities """


def run_shell(command):
    process = subprocess.run(command, capture_output=True)
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

    if pod.name == "AppLovinSDK":
        return
    elif pod.name == "Google-Mobile-Ads-SDK":
        install_google_sdk()
    else:
        download_pod(pod)
        create_build_rule(pod)
        install_dependencies(pod)
        install_subspecs(pod)


def download_pod(pod):
    if pod.url is None:
        return

    extract_dir = install_dir / pod.name

    if extract_dir.exists():
        return

    if pod.source == "http":
        file_name = pod.url.split('/')[-1]
        file_path = install_dir / file_name

        urlretrieve(pod.url, file_path)

        if file_name.endswith(".zip"):
            with ZipFile(file_path, 'r') as zip_file:
                zip_file.extractall(extract_dir)
        elif re.search(r'\.tar(\..*)?$', str(file_path)) is not None:
            with tarfile.open(file_path, 'r:*') as tar_file:
                tar_file.extractall(extract_dir)
        else:
            file_path.unlink()
            raise Exception(
                "Unable to extract dependencies from downloaded file")

        file_path.unlink()

    elif pod.source == "git":
        # Download is source code instead of vendored framework
        # Create empty directory for publisher to copy framework into
        if pod.framework_path == "":
            extract_dir.mkdir(exist_ok=True)
            manual_pods.add(pod)
            return

        run_shell(['git', 'clone', pod.url, extract_dir])


def create_build_rule(pod):
    if len(pod.framework_path) == 0:
        return

    path = f'Path.Combine(AppLovinPodsPath, "{pod.name}", '
    if pod in manual_pods:
        # Manually installed pods will be directly installed to directory
        path += f'"{pod.module_name}.xcframework")'
    else:
        components = pod.framework_path.split("/")
        components = map(lambda c: f'"{c}"', components)
        path += ", ".join(components) + ")"

    # Generate build rule
    rule = f"""PublicAdditionalFrameworks.Add(\n\tnew Framework(\n\t\t"{pod.module_name}",\n\t\t{path}"""

    if pod.resources is not None:
        rule += f',\n\t\t"{pod.resources}"'

    rule += "\n\t)\n);"

    build_rules.add(rule)


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
        install_dependencies(subspec)
        install_subspecs(subspec)  # can have nested subspecs


def install_user_pods():
    install_dir.mkdir(parents=True, exist_ok=True)

    count = 0
    for pod in parse_podfile():
        try:
            if pod.name == "AppLovinSDK":
                continue

            install_pod(pod)
            count += 1
            print(f"✔ Installed '{pod.name}'")
        except Exception as e:
            print(f"✘ Failed to install '{pod.name}'\n\t{e}")

    return count


# Convenience for installing Google SDK due to complexity and commonly used for UMP
def install_google_sdk():
    url = "https://dl.google.com/googleadmobadssdk/googlemobileadssdkios.zip"
    download_path = install_dir / "googlemobileadssdkios.zip"
    urlretrieve(url, download_path)

    with ZipFile(download_path, 'r') as zip_file:
        zip_file.extractall(install_dir)

    download_path.unlink()

    # Get downloaded directory name with version
    google_dir = next(install_dir.glob("GoogleMobileAdsSdkiOS-*"))
    for d in google_dir.iterdir():
        if d.suffix == ".xcframework":
            path = f'Path.Combine(AppLovinPodsPath, "{google_dir.name}", "{d.name}")'
            rule = f'''PublicAdditionalFrameworks.Add(\n\tnew Framework(\n\t\t"{d.stem}",\n\t\t{path}\n\t)\n);'''
            build_rules.add(rule)


""" Main """


def main():
    print("\nThis script installs AppLovin adapters and third-party SDKs using your Podfile.\n")
    print("Please refer to our documentation for complete instructions:")
    print("https://dash.applovin.com/documentation/mediation/unreal/mediation-adapters/ios\n")

    print("> Updating CocoaPods repos... (this may take a while)")
    run_shell("pod --silent repo update".split())

    # -- Automated installation

    print("> Installing dependencies from Podfile...\n")
    installed_count = install_user_pods()

    print(f"\n> Installed {installed_count} pod(s)")

    # --- Manual installation

    if len(manual_pods) != 0:
        print("\n-----\n")

        print("Download the following frameworks into their respective directories in IOS/Adapters/")
        print("(These should be *.framework or *.xcframework files)\n")

        for pod in manual_pods:
            if pod.module_name != pod.name:
                print(f"> {pod.name} ({pod.module_name})")
            else:
                print(f"> {pod.name}")

        input("\nPress 'Enter' to continue...")

    print("\n-----\n")

    # --- Generated instructions

    print("Follow the generated instructions below to modify AppLovinMAX.Build.cs.\n")
    input("Press 'Enter' to generate build instructions...")
    print()

    # Public Frameworks
    if len(Pod.all_frameworks) != 0:
        print_instruction(
            "Replace the existing PublicFrameworks with:")
        formatted_frameworks = ',\n'.join(
            sorted(map(lambda f: f'\t\t"{f}"', Pod.all_frameworks)))
        print("PublicFrameworks.AddRange(\n\tnew string[]\n\t{{\n{0}\n\t}}\n);".format(
            formatted_frameworks))
        print()

    # Weak Frameworks
    if len(Pod.all_weak_frameworks) != 0:
        print_instruction(
            "Add the build rule for including weak frameworks:")
        formatted_weak_frameworks = ',\n'.join(
            sorted(map(lambda f: f'\t\t"{f}"', Pod.all_weak_frameworks)))
        print("PublicWeakFrameworks.AddRange(\n\tnew string[]\n\t{{\n{0}\n\t}}\n);".format(
            formatted_weak_frameworks))
        print()

    # System Libraries
    if len(Pod.all_libraries) != 0:
        print_instruction(
            "Add the build rule for including system libraries:")

        for lib in Pod.all_libraries:
            print(f'PublicSystemLibraries.Add("{lib}");')

        print()

    # Public Additional Frameworks
    print_instruction("Add build rules for each dependency framework:")
    print("\n\n".join(sorted(build_rules)))


if __name__ == "__main__":
    main()
