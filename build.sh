#!/usr/bin/env bash

clang -o bookkeeper ./*.c

FOLDER=$HOME/.local/bin
if [ $? ]; then
   mv bookkeeper "$FOLDER"
fi
