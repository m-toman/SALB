# SALB

## News

### Current state of the project

As the world of speech synthesis moved on to Deep Learning, I recommend checking out projects like Mozilla TTS, the Nvidia Tacotron 2 implementation or similar. 

### Publication

M. Toman and M. Pucher. “An Open Source Speech Synthesis Frontend for HTS”. in:
Proceedings of the 18th International Conference of Text, Speech and Dialogue (TSD).
Plzen, Czech Republic, 2015, pp. 291-298 


## Description

The SALB system is a software framework for speech synthesis 
using HMM based voice models built by HTS (http://hts.sp.nitech.ac.jp/).
See a more generic description on http://m-toman.github.io/SALB/.

The package currently includes:

A C++ framework that abstracts the backend functionality and
provides a SAPI5 interface, a command line interface 
and a C++ API.

Backend functionality is provided by 
- an internal text analysis module for (Austrian) German,
- flite as text analysis module for English and
- hts_engine for parameter generation/synthesis.
(see COPYING for information on 3rd party libraries)

Also included is an Austrian German male voice model.

For information on how to build the system see BUILD.

For information on using the system, using a custom voice or adding a new language, see the project wiki at https://github.com/m-toman/SALB/wiki


## Directory structure

See also doc/architecture.svg.

### api

APIs for other language (e.g. python).

### cli

A command line interface that can be used to synthesize sentences given a model file (and optionally text rules for non-english voice models).

### data

Voice models and text rules included with the system.

### sapi

SAPI5 interface code, produces htstts.dll that can be registered as SAPI5 engine.

### sapi/install

Contains a NSI script to create installer packages for SAPI voices.
install.nsi creates a package for the provided "Leo" voice, but can easily be adapted.

### engine/text

Text analysis modules.
These are currently flite and an internal module.

### engine/synthesis

Synthesis (parameter and waveform generation) modules.
Currently hts_engine.

### engine/manager

TTS engine coordinating text analysis and synthesis.
Used as an interface for frontends (sapi, cli, api) or directly as API.


## Coding style guideline

- Code formatting is done using astyle (http://astyle.sourceforge.net/) using the provided astyle config file (astyle.config): astyle --options=astyle.config .cpp .h
- Naming conventions for classes and methods:
  Classes and methods in CamelCase with first character uppercase (class FooBar { void GetStuff(); };)
  Member variables camelCase with first character lowercase (int someVal;)
  Functions in lowercase with underscores (void get_stuff();)
- no "using"-directives in header files

