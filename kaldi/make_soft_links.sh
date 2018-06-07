#!/bin/bash

if [ $# -lt 2 ]; then
  echo "Usage: make_soft_links.sh <kaldi-root> <database-root>"
  exit 1
fi

kaldi=$1

ln -sf ${kaldi}/tools/openfst/lib/libfst.so.8.0.0  ${kaldi}/tools/openfst/lib/libfst.so
ln -sf ${kaldi}/tools/openfst/lib/libfst.so.8.0.0  ${kaldi}/tools/openfst/lib/libfst.so.8

timit=$2/en_Timit
librispeech=$2/en_LibriSpeech

ln -sf ${librispeech}/audio/enroll ${timit}/audio/enroll
ln -sf ${librispeech}/exp/tri4a ${timit}/exp/tri4a
ln -sf ${librispeech}/data/lang ${timit}/data/lang