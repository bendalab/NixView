#
# project information
#
MESSAGE("=> setup NIXView Project settings ...")

if (APPLE)
  #
  # The following variables define the portability and compatability attributes of the Mac OS X build
  # they are choosen with care and should not be changed without good cause.
  #
  # Among other things these options are chosen to match the portability and compatability options of the
  # Qt framework dylibs which can be checked as follows:
  #
  # otool -l <binary> | grep -A3 LC_VERSION_MIN_MACOSX
  #
  set (CMAKE_OSX_DEPLOYMENT_TARGET 10.9 CACHE STRING
    "Earliest version of OS X supported 

Earliest version we can support with Qt 5.8, C++11 & libc++ is 10.9.
Do not override this if you intend to build an official deployable installer.")
  set (CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk CACHE STRING
    "Mac OS X SDK to build with

Change this to the newest SDK available that you can install on your system (10.11 preferred).
Do not override this if you intend to build an official deployable installer.")
endif (APPLE)


set (PROJECT_VERSION_MAJOR 0)
set (PROJECT_VERSION_MINOR 5)
set (PROJECT_VERSIOM_PATCH 1)

set (PROJECT_NAME "NixView")
set (PROJECT_VENDOR "Bendalab")
set (PROJECT_CONTACT "Jan Grewe <jan.grewe@g-node.org>")
set (PROJECT_COPYRIGHT "Copyright (C) 2016-2017 by Jan Grewe")
set (PROJECT_HOMEPAGE https://www.github.com/bendalab/nixview)

set (PROJECT_SUMMARY_DESCRIPTION "${PROJECT_NAME} - Viewer for Nix Data files.")
set (PROJECT_DESCRIPTION "${PROJECT_SUMMARY_DESCRIPTION}")
