#!/usr/bin/env sh

git submodule init
git submodule update
cd lib/hidapi/
git checkout master
git pull
cd ../libserialport/
git checkout master
git pull
cd ../libusb/
git checkout master
git pull
cd ../..
