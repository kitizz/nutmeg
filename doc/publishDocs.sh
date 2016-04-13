#!/bin/bash
docdir=$(pwd)
function finish {
    cd $docdir
}
trap finish EXIT

doxygen
cd src
git add -u .
git commit -m
