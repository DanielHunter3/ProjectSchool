#!/bin/bash

boost/script/start.sh  || exit 1
boost/script/build.sh  || exit 1
boost/script/check.sh  || exit 1
boost/script/total.sh  || exit 1