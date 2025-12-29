#!/bin/bash

alloc/script/start.sh  || exit 1
alloc/script/build.sh  || exit 1
alloc/script/check.sh  || exit 1
alloc/script/total.sh  || exit 1