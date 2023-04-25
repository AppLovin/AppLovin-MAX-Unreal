#!/usr/bin/env python3
#
# This script downloads AppLovin adapters for iOS and generates build settings for use in UE projects.
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

build_rules = []
swift_frameworks = []
all_libraries = set()
all_dependencies = set()
all_frameworks = set([
    "AdSupport",
    "AppTrackingTransparency",
    "AudioToolbox",
    "AVFoundation",
    "CFNetwork",
    "CoreGraphics",
    "CoreMedia",
    "CoreMotion",
    "CoreTelephony",
    "Foundation",
    "MessageUI",
    "SafariServices",
    "StoreKit",
    "SystemConfiguration",
    "UIKit",
    "WebKit"
])


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
    lib_path = [l for l in adapter_dir.rglob(
        "*.a") if not "simulator" in str(l)][0]
    lib_name = lib_path.name
    lib_path.rename(Path(f"../{lib_name}"))

    # Delete original adapter directory
    shutil.rmtree(adapter_dir)

    # Generate build settings
    rule = f'PublicAdditionalLibraries.Add( Path.Combine( AppLovinIOSPath, "{lib_name}" ) );'

    build_rules.append(rule)


def add_dependencies(dependencies):
    for dependency, version in dependencies.items():
        if dependency == "AppLovinSDK":
            continue

        # Extract precise version string
        version = version[0]
        exact_version = re.search('(\d+(\.\d+)*)', version).group(0)

        try:
            process = subprocess.run(
                ["pod", "spec", "cat", dependency, f"--version={exact_version}"], capture_output=True)
            spec = json.loads(process.stdout.decode().strip())

            download_url = spec["source"]["http"]
            all_dependencies.add(
                f"  {dependency} {version} -> {download_url}")

            if "dependencies" in spec:
                add_dependencies(spec["dependencies"])

            # Add frameworks that will need to be linked
            if "frameworks" in spec:
                all_frameworks.update(set(spec["frameworks"]))
            if "weak_frameworks" in spec:
                all_frameworks.update(set(spec["weak_frameworks"]))
            if "libraries" in spec:
                if "xml2" in spec["libraries"]:
                    all_libraries.add('PublicSystemLibraries.Add("xml2");')

            if "swift_version" in spec or "swift_versions" in spec:
                swift_frameworks.append(dependency)
        except:
            pass


def main():
    print("This script only installs AppLovin adapters and does not retrieve the third-party SDKs.\nPlease refer to the README instructions to complete your integration.\n")

    print("> Updating CocoaPods repos... (this may take a while)")
    subprocess.run(["pod", "--silent", "repo", "update"])

    print("> Installing adapters...\n")
    pods = parse_podfile()
    install_pods(pods)

    print(f"\n> Installed {len(build_rules)} adapter(s)\n")

    print("==========================================================")
    print("  Follow these instructions to complete your integration  ")
    print("==========================================================")

    print("\n1. Copy the following iOS build rules into AppLovinMAX.Build.cs:\n")
    print("\n".join(build_rules))

    print("\n2. Replace the existing PublicFrameworks with the following in AppLovinMAX.Build.cs:\n")
    formatted_frameworks = ',\n'.join(
        sorted(map(lambda f: f"\t\t\"{f}\"", all_frameworks)))
    print("PublicFrameworks.AddRange(\n\tnew string[]\n\t{{\n{0}\n\t}}\n);".format(
        formatted_frameworks))

    print("\n3. Add build rules to include the following libraries in AppLovinMAX.Build.cs:\n")
    if len(all_libraries) > 0:
        print("\n".join(sorted(all_libraries)))

    print("\n4. Add copy rules for linking Swift frameworks to AppLovinMAX_UPL_IOS.xml under the <init> tag:\n")

    for framework in swift_frameworks:
        print(
            f'<copyDir src="$S(EngineDir)/Intermediate/UnzippedFrameworks/{framework}/{framework}.embeddedframework/{framework}.framework" dst="$S(BuildDir)/Frameworks/{framework}.framework"/>')

    print("\n5. Download the following third-party SDKs and dependencies manually:\n")
    print("\n".join(sorted(all_dependencies)))
    print("\n6. Create an embedded framework ZIP file for each framework and include it in the IOS directory alongside the adapters.")
    print("""7. Add build rules for the additional frameworks in AppLovinMAX.Build.cs with the following format:\n
PublicAdditionalFrameworks.Add(
	new Framework(
		"{FRAMEWORK_NAME}",
		Path.Combine( AppLovinIOSPath, "{FRAMEWORK_NAME}.embeddedframework.zip" )
	);
);""")


if __name__ == "__main__":
    main()
