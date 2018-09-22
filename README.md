[![Build Status](https://travis-ci.org/bendalab/NixView.svg?branch=master)](https://travis-ci.org/bendalab/NixView)

# NixView
A Qt based viewer for nix datafiles

--- under development ---

## Nix 
The nix data model is a generic model for storing scientific data with attached metadata.

For more information on nix see https://github.com/G-Node/nix


## Dependencies
 - Qt5
 - Boost > 1.49
 - cmake
 - Nix libraries (currently builds against the 1.4.2 release, build from source or install via launchpad/homebrew)


## Installation

To build NixView from source you may follow these instructions. This
software is under development and usually works with/requires the
latest nix version. To install this, follow the instructions given there: https://github.com/G-Node/nix/blob/1.3/README.md

### Ubuntu 14.04 and higher

```bash
sudo apt-get install qt5-default cmake libboost-regexp-dev libboost-program-options-dev libboost-date-time-dev libboost-program-options-dev libboost-system-dev libboost-filesystem-dev libcpptest
git clone https://github.com/bendalab/nixview.git
cd nixview
mkdir build
cd build
cmake ..
make -j 4
./nixview
```



### Ubuntu 16.04 
```bash
sudo apt-get install qt5-default cmake libboost-regex-dev libboost-program-options-dev libboost-date-time-dev libboost-program-options-dev libboost-system-dev libboost-filesystem-dev libcpptest-dev
git clone https://github.com/bendalab/nixview.git
cd nixview
mkdir build
cd build
cmake ..
make -j 4
./nixview
```

### MacOS X

The easiest way to install the dependencies is via [homebrew](http://brew.sh).
```shell
brew update
brew tap g-node/pkg
brew install nixio

brew install qt5
brew install boost

git clone https://github.com/bendalab/nixview.git
cd nixview
mkdir build
cd build
export CMAKE_PREFIX_PATH=/usr/local/opt/qt5/
cmake ..
make -j 4
./nixview
```
