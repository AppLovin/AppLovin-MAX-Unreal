#!/usr/bin/env python3
#
# This script downloads AppLovin adapters for iOS and generates build settings for use in UE4 projects.
#
# Copyright © 2021 AppLovin Corporation. All rights reserved.
#

import json
import shutil
import subprocess

from urllib.request import urlretrieve
from pathlib import Path
from zipfile import ZipFile

adapter_code = []


def parse_podfile():
    with open("Podfile") as podfile:
        print("> Installing adapters")

        # Trim whitespace
        pods = map(lambda l: l.strip(), podfile.readlines())

        # Extract lines for pods
        pods = filter(lambda l: l.startswith("pod"), pods)

        # Get pod names
        pods = map(lambda l: l.split(' ', 1)[1].strip("\'"), pods)

        return pods


# Parse and retrieve top-level pod dependencies
def parse_pods(pods):
    for pod in pods:
        if not pod.startswith("AppLovinMediation"):
            continue

        process = subprocess.run(
            ["pod", "spec", "cat", pod], capture_output=True)
        spec = json.loads(process.stdout.decode().strip())

        create_embedded_framework(spec)


def create_embedded_framework(spec):
    name = spec["name"]
    url = spec["source"]["http"]
    version = spec["version"]

    print(f"Installing {name} ({version})")

    # Download and save adapter
    zip_path = Path(f"../{name}.zip")
    urlretrieve(url, zip_path)

    # Extract into IOS directory
    extract_path = Path(f"{name}-{version}")
    with ZipFile(zip_path, 'r') as zip_file:
        zip_file.extractall()

    # Delete zip file
    zip_path.unlink()

    # Rename
    embedded_path = Path(f"../{name}.embeddedframework")
    embedded_zip_path = Path(f"../{name}.embeddedframework.zip")
    extract_path.rename(embedded_path)

    # Create embeddedframework.zip
    # NOTE: shutil.make_archive creates a ZIP file that doesn't work with Unreal for some reason
    subprocess.call(["zip", "-rq", embedded_zip_path, embedded_path])
    shutil.rmtree(embedded_path)

    # Generate build settings
    code = f"""
PublicAdditionalFrameworks.Add(
    new Framework(
        "{name}",
        Path.Combine( AppLovinIOSPath, "{name}.embeddedframework.zip" )
    )
);"""

    adapter_code.append(code)


def main():
    print("NOTE: This script only installs AppLovin adapters. Please follow the instructions at the end of installation to complete your integration.\n")

    print("> Updating CocoaPods repos... (this may take a while)")
    subprocess.call(["pod", "--silent", "repo", "update"])

    pods = parse_podfile()
    parse_pods(pods)

    print(f"\n> Installed {len(adapter_code)} adapter(s)")
    print("\nINTEGRATION STEPS")
    print("\n1. Go to https://dash.applovin.com/documentation/mediation/manual-integration-ios")
    print("2. Follow the instructions for \"Preparing Mediated Networks\". Add any necessary changes to Info.plist in AppLovinMAX_UPL_IOS.xml")
    print("3. Download the third-party SDKs required by each installed adapter and create a corresponding '.embeddedframework.zip' file under ThirdParty/IOS")
    print("4. Copy the iOS build rules below into AppLovinMAX.Build.cs. You will also need to add build rules for the third-party SDKs")
    print("\n".join(adapter_code))

if __name__ == "__main__":
    main()

