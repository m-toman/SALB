Build instructions

===========================
All platforms without SAPI5
===========================
CMake (http://www.cmake.org/) build files are provided with the package.

Linux Example:

Probably needed prerequisites:
$ sudo apt-get install g++
$ sudo apt-get install cmake

Build:
$ cmake .
$ make

Resulting libraries in ./release/lib
Resulting binaries in ./release/bin


==================
Windows with SAPI5
==================
To build the SAPI5 code for Windows, please use the provided MS Visual Studio project files (htstts.sln).

Description below:

Solution htstts
---------------
htstts.sln contains the complete solution.

Debug and Release configurations for 32- and 64-bit builds are provided for all projects.

The projects usually compile their output in subfolders of their project directory and are then prerequisites for other projects.
Visual Studio dependencies are set in the solution, so prerequisites are built first.

hts_engine and flite as backend modules are built first and produce static libraries.
These are used by the manager project to build a static manager library.
The frontends (sapi, api, cli) then incorporate the manager library.

Project flite
-------------
Compiles flite as a backend module for text analysis.
This is used as text analysis module for English.

Project hts_engine
------------------
Compiles hts_engine as a backend module for synthesis.

Project manager
---------------
Requires flite and hts_engine as backend, provides an interface to the frontend modules.

Project cli
-----------
Command-Line-Interface project.
Requires manager project.

Project sapi
------------
SAPI5 interface.
sapi/register contains a small tool for registration of a SAPI5 voice.
sapi/install contains an NSIS (Nullsoft Scriptable Install System) script to generate Windows installers for SAPI voices.
Requires manager project.

