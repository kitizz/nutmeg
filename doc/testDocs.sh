#!/bin/bash
docdir=$(pwd)
function finish {
    cd $docdir
}
trap finish EXIT

doxygen
cd src
make html

