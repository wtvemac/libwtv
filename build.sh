#!/usr/bin/env bash

# Bash strict mode http://redsymbol.net/articles/unofficial-bash-strict-mode/
set -euo pipefail


IFS=$'\n\t'

if [[ -z ${LIBWTV_INSTALL_DIR-} ]]; then
  echo LIBWTV_INSTALL_DIR environment variable is not defined
  echo Please set LIBWTV_INSTALL_DIR to point to your libwtv toolchain directory
  exit 1
fi

if [[ $OSTYPE == 'msys' ]]; then
  if [ "${MSYSTEM:-}" != "MINGW64" ]; then
    # We only support building host tools via mingw-x64 at the moment, so
    # enforce that to help users during installation.
    echo This script must be run from the \"MSYS2 MinGW x64\" shell
    echo Plase open that shell and run it again from there
    exit 1
  fi
fi

makeWithParams(){
  make -j"${JOBS}" "$@"
}

sudoMakeWithParams(){
  env LIBWTV_INSTALL_DIR="$LIBWTV_INSTALL_DIR" \
  make -j"${JOBS}" "$@" 2&> /dev/null || \
    sudo env LIBWTV_INSTALL_DIR="$LIBWTV_INSTALL_DIR" \
    make -j"${JOBS}" "$@"
}

makeAndInstallWithParams() {
  env BOX_CPU_SUBARCHITECTURE="$1" \
    make -j"${JOBS}" ${@:2}

  env LIBWTV_INSTALL_DIR="$LIBWTV_INSTALL_DIR" BOX_CPU_SUBARCHITECTURE="$1" \
  make -j"${JOBS}" install 2&> /dev/null || \
    sudo env LIBWTV_INSTALL_DIR="$LIBWTV_INSTALL_DIR" BOX_CPU_SUBARCHITECTURE="$1" \
    make -j"${JOBS}" install
}

# Limit the number of make jobs to the number of CPUs
JOBS="${JOBS:-$(getconf _NPROCESSORS_ONLN)}"
JOBS="${JOBS:-1}" # If getconf returned nothing, default to 1

# Clean, build, and install libwtv + tools
sudoMakeWithParams install-mk
makeWithParams clobber
#makeAndInstallWithParams 5000 libwtv
makeAndInstallWithParams 4650 libwtv
makeAndInstallWithParams 5000 libwtv


echo
echo libwtv built successfully!
