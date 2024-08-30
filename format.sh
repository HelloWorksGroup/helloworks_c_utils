#!/bin/bash
find . | grep '.*\.[ch]$' | xargs clang-format --verbose -style=file -i &&set _
