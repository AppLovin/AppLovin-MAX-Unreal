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

    print(f"{name} ({version})")

    # Download and save adapter
    zip_path = Path(f"{name}.zip")
    urlretrieve(url, zip_path)

    # Extract into IOS directory
    with ZipFile(zip_path, 'r') as zip_file:
        zip_file.extractall()

    # Delete zip file
    zip_path.unlink()

    # Rename extracted directory and move to parent directory
    extract_path = Path(f"{name}-{version}")
    embedded_path = Path(f"../{name}.embeddedframework")
    embedded_zip_path = Path(f"../{name}.embeddedframework.zip")
    extract_path.rename(embedded_path)

    # Create embeddedframework.zip
    # NOTE: shutil.make_archive creates a ZIP file that doesn't work with Unreal for some reason
    subprocess.run(["zip", "-rq", embedded_zip_path, embedded_path])
    shutil.rmtree(embedded_path)

    # Generate build settings
    rule = f"""
PublicAdditionalFrameworks.Add(
    new Framework(
        "{name}",
        Path.Combine( AppLovinIOSPath, "{name}.embeddedframework.zip" )
    )
);"""

    build_rules.append(rule)


def main():
    print("This script only installs AppLovin adapters and does not retrieve the third-party SDKs.\nPlease refer to the README instructions to complete your integration.\n")

    print("> Updating CocoaPods repos... (this may take a while)")
    subprocess.run(["pod", "--silent", "repo", "update"])

    print("> Installing adapters...")
    pods = parse_podfile()
    install_pods(pods)

    print(f"\n> Installed {len(build_rules)} adapter(s)")
    print("> Copy the iOS build rules below into AppLovinMAX.Build.cs. You will also need to add build rules for the third-party SDKs later.")
    print("\n".join(build_rules))


if __name__ == "__main__":
    main()
