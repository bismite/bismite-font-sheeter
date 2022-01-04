#!/bin/bash

mkdir -p build/licenses/
cp LICENSE.txt build/licenses/bismite-font-sheeter-LICENSE.txt
(
  cd build
  tar czf bismite-font-sheeter.tgz bin licenses
)
