#!/bin/bash

NUM_OF_CORE=$(nproc)

rm -rf .cache compile_commands.json
make clean
bear -- make all -j$(( NUM_OF_CORE+1 ))
