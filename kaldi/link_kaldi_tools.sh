#!/bin/bash

if [ $# -lt 1 ]; then  
  echo "Usage: link_kaldi_tools.sh <kaldi-root>"
  exit 1
fi

kaldi=$1

ln -sf ${kaldi}/tools/openfst/lib/libfst.so.8.0.0  ${kaldi}/tools/openfst/lib/libfst.so
ln -sf ${kaldi}/tools/openfst/lib/libfst.so.8.0.0  ${kaldi}/tools/openfst/lib/libfst.so.8
