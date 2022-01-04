#!/bin/bash

mkdir -p build
(
  cd build
  curl "-JOL#" https://github.com/bismite/cairo-macOS/releases/download/1.5/cairo-macOS.tgz
  tar xf cairo-macOS.tgz
)
