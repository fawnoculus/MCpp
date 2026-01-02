#!/bin/sh

pushd ..
mkdir build
cmake -G "Ninja" -B build
popd
