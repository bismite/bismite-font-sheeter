#!/bin/bash

mkdir -p build
(
  cd build
  wget https://github.com/bismite/cairo-x86_64-w64-mingw32/releases/download/1.0.0/cairo-x86_64-w64-mingw32.tgz
  tar xf cairo-x86_64-w64-mingw32.tgz
)
