#!/bin/bash

export TARGET_DEVICE=lm3s811
export DESTDIR=/usr
export VERSION=0.0.6

echo "Installing device firmware $VERSION for $TARGET_DEVICE"
make install
