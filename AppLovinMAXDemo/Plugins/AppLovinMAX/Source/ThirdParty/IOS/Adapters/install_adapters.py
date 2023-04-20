#!/usr/bin/env python3
#
# This script downloads AppLovin adapters for iOS and generates build settings for use in UE projects.
#
# Copyright © 2021 AppLovin Corporation. All rights reserved.
#

import json
import shutil
import subprocess

from urllib.request import urlretrieve
from pathlib import Path
from zipfile import ZipFile

build_rules = []
all_dependencies = set()


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

        return pods


def install_pods(pods):
    for pod in pods:
        process = subprocess.run(
            ["pod", "spec", "cat", pod], capture_output=True)
        spec = json.loads(process.stdout.decode().strip())
        create_embedded_framework(spec)


def create_embedded_framework(spec):
    name = spec["name"]
    url = spec["source"]["http"]
    version = spec["version"]

    print(f"  {name} ({version})")

    # Add all the dependencies (recursively) that will need to be manually installed by developer
    add_dependencies(spec["dependencies"])

    # Download and save adapter
    zip_path = Path(f"{name}.zip")
    urlretrieve(url, zip_path)

    # Extract into IOS directory
    with ZipFile(zip_path, 'r') as zip_file:
        zip_file.extractall()

    # Delete zip file
    zip_path.unlink()

    # Copy static library from extracted directory into parent directory
    adapter_dir = Path(f"{name}-{version}")
    lib_dir = adapter_dir / f"{name}.xcframework" / "ios-arm64_armv7"

    lib_path = next(lib_dir.glob("*.a"))
    lib_name = lib_path.name
    lib_path.rename(Path(f"../{lib_name}"))

    # Delete original adapter directory
    shutil.rmtree(adapter_dir)

    # Generate build settings
    # TODO: Specify any additional frameworks that need to be linked?
    rule = f'PublicAdditionalLibraries.Add( Path.Combine( AppLovinIOSPath, "{lib_name}" ) );'

    build_rules.append(rule)


def add_dependencies(dependencies):
    for dependency, value in dependencies.items():
        if dependency != "AppLovinSDK":
            all_dependencies.add(f"  {dependency} {value}")

            try:
                process = subprocess.run(
                    ["pod", "spec", "cat", dependency], capture_output=True)
                spec = json.loads(process.stdout.decode().strip())
                if "dependencies" in spec:
                    add_dependencies(spec["dependencies"])
            except:
                pass


def main():
    print("This script only installs AppLovin adapters and does not retrieve the third-party SDKs.\nPlease refer to the README instructions to complete your integration.\n")

    print("> Updating CocoaPods repos... (this may take a while)")
    subprocess.run(["pod", "--silent", "repo", "update"])

    print("> Installing adapters...\n")
    pods = parse_podfile()
    install_pods(pods)

    print(f"\n> Installed {len(build_rules)} adapter(s)")
    print("> Copy the following iOS build rules into AppLovinMAX.Build.cs:\n")
    print("\n".join(build_rules))

    print("\n> Download the following third-party SDKs and dependencies manually:\n")
    print("\n".join(sorted(all_dependencies)))
    print("\n> Create an embedded framework ZIP file for each framework and include it in the IOS directory alongside the adapters.")
    print("""> Add build rules for each of the additional frameworks in AppLovinMAX.Build.cs with the following format:\n
PublicAdditionalFrameworks.Add(
	new Framework(
		"{FRAMEWORK_NAME}",
		Path.Combine( AppLovinIOSPath, "{FRAMEWORK_NAME}.embeddedframework.zip" )
	);
);""")


if __name__ == "__main__":
    main()
