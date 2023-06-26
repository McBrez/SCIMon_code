# SCIMon 
SCImon is a piece of software, that allows real-time remote control of an spheriod on chip lab-setup.


## Build
SCImon uses a CMake build system. As such, be sure to install CMake >=3.15. Furthermore, a few libraries are linked against. Some are already included as git sub-repositories, and do not require installation. Others are however not feasible to be added as sub-repositories, and have to be installed explicitly. Following libraries/frameworks have to be installed: 

* Qt6 (https://www.qt.io/download)
* HDF5 (https://www.hdfgroup.org/downloads/hdf5/)
* BOOST (https://sourceforge.net/projects/boost/files/boost-binaries/)

If installed to their default location, they should be found automatically by CMake.

## Installation

## License