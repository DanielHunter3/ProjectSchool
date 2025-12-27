#!/bin/bash

script/start.sh  || exit 1
script/build.sh  || exit 1
script/check.sh  || exit 1
script/total.sh  || exit 1
script/visual.sh || exit 1