# SCIMon 
SCImon is a piece of software, that allows modular real-time remote control of an spheriod on chip lab-setup. This document gives a short overview of how to install the pre-requisites for SCIMon and how to build it. Detailed information about the code and its concepts can be found in the doc folder. 

## Build
SCIMon is not explicitly developed for specific OSs, but right now, only Windows is supported. The reason for that is, that some of the Devices, SCIMon shall interface with, only come with drivers for Windows. 
SCImon uses a CMake build system. As such, be sure to install CMake >=3.15. Furthermore, a few libraries are linked against. Some are already included as git sub-repositories, and do not require installation. Others are however not feasible to be added as sub-repositories, and have to be installed explicitly. Following libraries/frameworks have to be installed/built: 

* Qt6 - For GUI (https://www.qt.io/download) 
* Qwt - For plots (https://qwt.sourceforge.io/)
* HDF5 - For saving data (https://www.hdfgroup.org/downloads/hdf5/)
* Boost - For abstraction of communication devices (e.g. serial ports) (https://sourceforge.net/projects/boost/files/boost-binaries/)

## Installation
1. Download and install Qt6 to its default location.
2. Download Qwt, build it and install it to its default location.
3. Download HDF5 library and install it to its default location.
4. Download prebuilt Boost library and install it ot its default location.  

## License
None yet.