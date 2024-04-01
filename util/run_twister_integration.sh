#!/bin/bash

# Resolve the path to the current script and set CI_PATH to its directory
CI_PATH=$(dirname "$(realpath "$0")")

run_twister() {
    local test_directory=$1
    echo -n "Building configurations in \"$test_directory\": "

    # Run west twister with the specified arguments
    pushd "$CI_PATH" > /dev/null
    # Store in File
    west twister -T "../$test_directory" --integration > twister_out.txt 2> twister_err.txt
    #west twister -T "../$test_directory" --integration
    local status=$?
    popd > /dev/null

    # Check the exit status and print the relevant message
    if [ $status -eq 0 ]; then
        echo "[OK]"
    else
        echo "[FAIL]"
        cat $CI_PATH/twister_err.txt
	# Exit on first failure
	exit 1
    fi
}

# Run twister for the specified test directories
run_twister "test"
run_twister "app"
run_twister "samples"
