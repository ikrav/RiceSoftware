#!/bin/bash

echo "clean up"
set -x # turn echo on
find ../ -name "*.so" | xargs -i rm {}
find ../ -name "*.d" | xargs -i rm {}
