#!/usr/bin/env sh

git submodule init
git submodule update
cd lib/hidapi/
git pull
cd ../libserialport/
git pull
cd ../libusb/
git pull
cd ../..
