#!/usr/bin/env bash

set -e

usage="Usage: ./update_xcconfig.sh"

# Check if in the same directory as uproject
uproject_file=$(find . -maxdepth 1 -type f -name '*.uproject')
if [ -z "$uproject_file" ]; then
  echo "Error: No .uproject file found in current directory. Make sure to run this script from your project's root directory"
  exit 1
fi

project_name=$(basename "$uproject_file" .uproject)

echo "Updating .xcconfig files for '$project_name'..."

xcconfig_paths=$(find . -type f -path "*/Intermediate/**/AppLovinMAXDemo_*.xcconfig")
for unreal_xcconfig_path in $xcconfig_paths; do

  # Extract the build configuration and lowercase it
  build_config=$(echo "$unreal_xcconfig_path" | sed -n 's/.*_\(.*\)\.xcconfig/\1/p' | tr '[:upper:]' '[:lower:]')
  
  # Calculate depth of file for creating relative path to corresponding Pods xcconfig
  depth=$(( $(echo "$unreal_xcconfig_path" | tr -cd '/' | wc -c) - 1 ))
  relative_prefix=$(printf '../%.0s' $(seq 1 $depth))
  pod_xcconfig_path="${relative_prefix}Pods/Target Support Files/Pods-${project_name}/Pods-${project_name}.${build_config}.xcconfig"
  escaped_pod_xcconfig_path=$(echo "$pod_xcconfig_path" | sed 's/\//\\\//g')

  # Format include text
  include_text="#include \"${escaped_pod_xcconfig_path}\""

  # Check if the contents of the file already have the include line
  if ! grep -q "^${include_text}" "${unreal_xcconfig_path}"; then
    # Prepend include to the file
    sed -i '' "1s/^/${include_text}\n/" "${unreal_xcconfig_path}"
    echo "Updated ${unreal_xcconfig_path}"
  else
    echo "File already updated. Skipping ${unreal_xcconfig_path}"
  fi
done

