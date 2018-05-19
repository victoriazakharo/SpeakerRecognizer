#!/bin/bash

if [ $# -lt 4 ]; then  
  echo "Usage: process.sh <kaldi-trunk> <source-folder> <user-id> <record-id>"
  exit 1
fi

kaldi=$1
source=$2
id=$3
rec_number=$4
steps=${kaldi}/egs/wsj/s5/steps
utils=${kaldi}/egs/wsj/s5/utils
src=${kaldi}/src

export train_cmd="run.pl --mem 2G"
export decode_cmd="run.pl --mem 4G"
export mkgraph_cmd="run.pl --mem 8G"

export KALDI_ROOT=${kaldi}
[ -f $KALDI_ROOT/tools/env.sh ] && . $KALDI_ROOT/tools/env.sh
export PATH=${utils}:$PATH
[ ! -f $KALDI_ROOT/tools/config/common_path.sh ] && echo >&2 "The standard file $KALDI_ROOT/tools/config/common_path.sh is not present -> Exit!" && exit 1
. $KALDI_ROOT/tools/config/common_path.sh
export LC_ALL=C

echo $PATH

pushd ${source}

ln -s ${utils}/ .

x=data/${id}
ali=exp/${id}

${steps}/make_mfcc.sh --cmd "$train_cmd" --nj 1 $x exp/make_mfcc/$x mfcc/${id}
${steps}/compute_cmvn_stats.sh $x exp/make_mfcc/$x mfcc/${id}

${steps}/align_fmllr.sh --nj 1 --cmd "$train_cmd" $x data/lang exp/tri4a ${ali}
${src}/bin/ali-to-phones --ctm-output exp/tri4a/final.mdl ark:"gunzip -c ${ali}/ali.1.gz|" -> ${ali}/ali.1.ctm;
./format_ali_single.pl ${ali}/ali.1.ctm ${rec_number} result_ali_${id}.1

popd
