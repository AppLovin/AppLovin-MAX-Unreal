#!/usr/bin/env bash

# Creates the ZIP file release for each engine version
#
# Usage: ./create_release.sh [VERSION]
# Example: ./create_release.sh 5.3
#
# Notes:
# - Version format is always X.X
# - Each major engine version requires a separate release
# - For 4.27.X support, set the VERSION to 4.27 (patch version will be set to 0)
# - If no VERSION is specified, creates an engine-agnostic plugin that should be integrated as project plugin

set -e

version="${1}"

unreal_path="${HOME}/AppLovin/AppLovin-MAX-Unreal"
packaged_path="${unreal_path}/Packaged"
plugin_path="${unreal_path}/AppLovinMAX"
uplugin_path="${plugin_path}/AppLovinMAX.uplugin"

mkdir -p "${packaged_path}"

if [ -n "$version" ]; then
    echo "Packaging for UE ${version}"

    # NOTE: Recommended Xcode version is 15.4 for UE 5.5, but works with prior UE versions
    # https://dev.epicgames.com/documentation/en-us/unreal-engine/ios-ipados-and-tvos-development-requirements-for-unreal-engine#versionhistory
    xcode_version='15.4'

    # Export environment variables for UAT Android builds based on requirements
    # https://dev.epicgames.com/documentation/en-us/unreal-engine/android-development-requirements-for-unreal-engine#versionhistory
    if [ "$version" == '5.2' ]; then
        studio_version='4.0'
        ndk_version='25.1.8937393'
    elif [ "$version" == '5.3' ] || [ "$version" == '5.4' ]; then
        studio_version='2022.2.1'
        ndk_version='25.1.8937393'
    else # Default: 5.5+
        studio_version='2024.1.2'
        ndk_version='25.1.8937393'
    fi

    # NOTE: Update these based on the installation directories for Android Studio/SDK
    export ANDROID_HOME="${HOME}/Library/Android/sdk"
    export ANDROID_SDK_HOME="${HOME}/Library/Android/sdk"
    export JAVA_HOME="${HOME}/Applications/Android Studio ${studio_version}.app/Contents/jre/jdk/Contents/Home"
    export NDKROOT="${HOME}/Library/Android/sdk/ndk/${ndk_version}"
    export NDK_ROOT="${HOME}/Library/Android/sdk/ndk/${ndk_version}"

    # Get RunUAT for UE version
    # Use `RunUAT.command BuildPlugin -help` for list of parameters
    uat="/Users/Shared/Epic Games/UE_${version}/Engine/Build/BatchFiles/RunUAT.command"

    # Builds for platforms in PlatformAllowList and Host Platform
    "${uat}" BuildPlugin \
        -Plugin="${uplugin_path}" \
        -Package="${packaged_path}/AppLovin-MAX-UE${version}"

    dirname="AppLovin-MAX-UE${version}"
    filename="AppLovin-MAX-UE${version}.zip"
else
    echo "Packaging generic UE plugin"

    # Clean intermediate/generated files only for direct source build added to project
    # Engine-specific versions require the extra directories
    rm -rf "${plugin_path}/Binaries"
    rm -rf "${plugin_path}/Content"
    rm -rf "${plugin_path}/Intermediate"

    python3 -c "
import json

with open('${uplugin_path}', 'r') as f:
    data = json.load(f)

data.pop('EngineVersion', None)

with open('${uplugin_path}', 'w') as f:
    json.dump(data, f, ensure_ascii=False, indent='\t')"

    dirname="../AppLovinMAX"
    filename="AppLovin-MAX-Unreal-Plugin.zip"
fi

# Create ZIP

cd "${packaged_path}"
zip -x "**/.DS_Store" -rq "${filename}" "${dirname}"
echo "Created ${filename}"

