#!/usr/bin/env python3
#
# This script downloads iOS dependencies from a Podfile and generates build settings for use in UE projects.
# Copy this script into the AppLovinMAX plugin's `Source/ThirdParty/IOS`` directory and run it from there to install dependencies.
# Usage: ./install_pods.py
#
# Copyright © 2021 AppLovin Corporation. All rights reserved.
#

import json
import re
import shutil
import subprocess
import tarfile

import xml.etree.ElementTree as ET

from urllib.request import urlretrieve
from pathlib import Path
from zipfile import ZipFile


""" Globals """


install_dir = Path.cwd() / "Pods"
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
            self.git_tag = source["tag"]
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


# Dictionaries cannot be added to sets, so ensure uniqueness when adding to list
def add_unique(l, object):
    if not any(x == object for x in l):
        l.append(object)


def find_dirs_with_extension(extension, dir):
    return [d for d in dir.rglob(f"*.{extension}") if d.is_dir()]


def dict_to_xml(parent, data):
    if isinstance(data, dict):
        for key, value in data.items():
            sub_elem = ET.SubElement(parent, key)
            dict_to_xml(sub_elem, value)
    elif isinstance(data, list):
        for item in data:
            item_elem = ET.SubElement(parent, 'Item')
            dict_to_xml(item_elem, item)
    else:
        parent.text = str(data)


def serialize_to_xml(data, root_tag='root'):
    root = ET.Element(root_tag)
    dict_to_xml(root, data)
    return ET.tostring(root, encoding='unicode')


""" Installation """


def parse_podfile():
    with open(install_dir.parent / "Podfile") as podfile:
        # Trim whitespace
        pods = map(lambda l: l.strip(), podfile.readlines())

        # Extract lines for pods
        pods = filter(lambda l: l.startswith("pod"), pods)

        # Get pod names
        pods = map(lambda l: l.split(' ', 1)[1].strip("\'"), pods)

        # Ignore AppLovinSDK
        pods = filter(lambda l: l != "AppLovinSDK", pods)

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

        cmd = ['git', 'clone', pod.url, extract_dir, "--single-branch"]
        if pod.git_tag != None:
            cmd += ["--branch", pod.git_tag]

        run_shell(cmd)


def install_dependencies(pod):
    for dependency, version in pod.dependencies.items():
        # Split the dependency into main pod name and subspec
        # Use the main pod name to install the dependency
        name, subspec = dependency.split("/", 1) \
            if "/" in dependency \
            else (dependency, None)

        if name != "AppLovinSDK" and name not in seen:
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


""" Main """


def main():
    print("\nThis script installs AppLovin adapters and third-party SDKs using your Podfile.\n")
    print("Please refer to our documentation for complete instructions:")
    print("https://developers.applovin.com/en/unreal/preparing-mediated-networks#ios\n")

    print("> Updating CocoaPods repos...")
    run_shell("pod --silent repo update".split())

    # -- Automated Installation -- #

    print("> Installing dependencies from Podfile... (this may take a while)\n")
    installed_count = install_user_pods()

    print(f"\n> Installed {installed_count} pod(s)")

    # -- Manual Installation -- #

    if len(manual_pods) != 0:
        print("\n-----\n")

        print("Download the following frameworks into their respective directories in IOS/Adapters/")
        print("(These should be *.framework or *.xcframework files)\n")

        for pod in manual_pods:
            if pod.module_name != pod.name:
                print(f"> {pod.name} ({pod.module_name})")
            else:
                print(f"> {pod.name}")

        input("\nPress 'Enter' after downloading frameworks to continue installation...")

    # -- Build Rules -- #

    additional_libraries = []
    additional_frameworks = []

    # Prevent duplicate linking of dependencies distributed with static/dynamic frameworks (e.g., Facebook, Vungle)
    seen_pod_names = set()

    def find_resources_bundle(prefix, dir):
        resources = next(dir.rglob(f"{prefix}*.bundle"), None)
        if resources:
            resources = resources.relative_to(install_dir)

        return resources

    # Iterate through each subdirectory per Pod
    pod_dirs = [x for x in install_dir.iterdir() if x.is_dir()]
    for pod_dir in pod_dirs:
        found = False

        # Search for all .xcframeworks
        xcframeworks = find_dirs_with_extension("xcframework", pod_dir)

        # Create a build rule for each xcframework
        for xcf in xcframeworks:
            name = xcf.stem
            if name in seen_pod_names:
                continue

            # If xcframework contains *.a, link as static library
            static_libs = [path for path in xcf.glob(
                "**/*.a") if "simulator" not in str(path)]

            if static_libs:
                static_lib = static_libs[0]
                add_unique(additional_libraries,
                           static_lib.relative_to(install_dir))

                seen_pod_names.add(name)
                found = True

            # Contains .framework, so link to xcframework directly
            elif find_dirs_with_extension("framework", pod_dir):
                # Prefer static frameworks over dynamic ones
                if "/dynamic/" in str(xcf).lower():
                    continue

                add_unique(additional_frameworks, {
                    "Name": name,
                    "Path": xcf.relative_to(install_dir),
                    "Resources": find_resources_bundle(name, xcf.parent)
                })

                seen_pod_names.add(name)
                found = True

        # No xcframeworks found, search for .frameworks
        if not found:
            frameworks = find_dirs_with_extension("framework", pod_dir)
            for f in frameworks:
                name = f.stem
                if name in seen_pod_names:
                    continue

                add_unique(additional_frameworks, {
                    "Name": name,
                    "Path": f.relative_to(install_dir),
                    "Resources": find_resources_bundle(name, f.parent)
                })

                seen_pod_names.add(name)
                found = True

        if not found:
            print(f"Failed to find framework for {pod_dir.name}")

    # -- Write XML Configuration -- #

    # Create a tree from the root element
    config_data = {
        "PublicFrameworks": list(Pod.all_frameworks),
        "PublicWeakFrameworks": list(Pod.all_weak_frameworks),
        "PublicSystemLibraries": list(Pod.all_libraries),
        "PublicAdditionalLibraries": additional_libraries,
        "PublicAdditionalFrameworks": additional_frameworks
    }

    config_xml = serialize_to_xml(config_data)

    with open(install_dir / "config.xml", "w", encoding="utf-8") as file:
        file.write(config_xml)


if __name__ == "__main__":
    main()
