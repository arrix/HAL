#!/usr/bin/env bash

# HAL
#
# Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
# Licensed under the terms of the Apache Public License.
# Please see the LICENSE included with this distribution for details.

set -e

declare -rx VERBOSE=1

declare -r HAL_DISABLE_TESTS="OFF"

cmd+="cmake"
cmd+=" -DHAL_DISABLE_TESTS=${HAL_DISABLE_TESTS}"

declare -r CMAKE_HOST_WIN32=$(cmake -P cmake/IsWin32.cmake 2>&1 | tr -d '\r\n')

if [[ x"${CMAKE_HOST_WIN32}" == "x1" ]]; then
    cmd+=" -G \"Visual Studio 12 2013\""
    declare -r project_name=$(grep -E '^\s*project[(][^)]+[)]\s*$' CMakeLists.txt | awk 'BEGIN {FS="[()]"} {printf "%s", $2}')
    declare -r solution_file_name="${project_name}.sln"
    declare -r MSBUILD_PATH="c:/Program Files (x86)/MSBuild/12.0/Bin/MSBuild.exe"
    declare -r BUILD_DIR="build"
    cmd+=" ../"
    cmd+=" && \"${MSBUILD_PATH}\" ${solution_file_name}"
else
    declare -r CMAKE_BUILD_TYPE=Debug
    declare -r BUILD_DIR=build.$(echo ${CMAKE_BUILD_TYPE} | tr '[:upper:]' '[:lower:]')
    cmd+=" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    cmd+=" ../"
    cmd+=" && make -j 4"
fi


function echo_and_eval {
    local -r cmd="${1:?}"
    echo "${cmd}" && eval "${cmd}"
}

echo_and_eval "rm -rf \"${BUILD_DIR}\""
echo_and_eval "mkdir -p \"${BUILD_DIR}\""
echo_and_eval "pushd \"${BUILD_DIR}\""
echo_and_eval "${cmd}"

if [[ x"${CMAKE_HOST_WIN32}" == "x1" ]]; then
    # On Windows we need to copy the DLL to the location of the
    # executables.
    declare -r dll_paths=$(find . -type f -name "*.dll" | sort | uniq | awk '{printf "\"%s\" ", $1}')
    declare -r exe_directories=$(find ./test ./examples -type f -name "*.exe" -exec dirname {} \; | sort | uniq | awk '{printf "\"%s\" ", $0}')
    for exe_directory in ${exe_directories}; do
        for dll_path in ${dll_paths}; do
            echo_and_eval "cp -p ${dll_path} ${exe_directory}"
        done
    done
fi

if [[ "${HAL_DISABLE_TESTS}" != "ON" ]]; then
    echo_and_eval "ctest -VV --output-on-failure"
fi

echo_and_eval "popd"
