#/bin/bash

release="release"

# build visual studio project
#msbuild ...

rm -rf $release
mkdir -p $release

# SAPI installer
makensis sapi/install/install.nsi
cp sapi/install/htssapi.exe $release/leopold-voice.exe


