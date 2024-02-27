#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
DOCKER_IMAGE=pyprt:linux-py3.8

docker run --init --rm --name pyprt \
  -v "${SCRIPT_DIR}":/tmp/pyprt -w /tmp/pyprt \
  ${DOCKER_IMAGE} \
  bash -c 'python -m pip install . && python tests/run_tests.py --xml_output_directory /tmp/pyprt/ws'
