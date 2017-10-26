nj=1 # must be equal to the number of result_ali.* files
cmd=utils/run.pl
program=./SpeakerModeler

. utils/parse_options.sh

usage="Usage: $0 <model-folder> <alignment-folder> <record-folder> <record-location-file> [<log-dir>]
Note: <log-dir> defaults to <model-folder>/log
e.g.: $0 models basefolder basefolder/data/LibriSpeech/dev-clean basefolder/data/train/rec_map.txt
Options: 
  --nj <nj>                                        # number of parallel jobs
  --cmd (utils/run.pl|utils/queue.pl <queue opts>) # how to run jobs."

if [ $# -lt 4 ] || [ $# -gt 5 ]; then
  >&2 echo "$usage"
  exit 1
fi
if [[ "$1" = "--help" || "$1" = "-h" ]]; then 
  echo "$usage"
  exit 0
fi 

model_folder="$1"
alignment="$2"
record_folder="$3"
record_location="$4"

if [ $# -ge 6 ]; then
  logdir="$6"
else
  logdir="${model_folder}/log"
fi

if [ -f $logdir ] ; then
   rm "${logdir}/*"
fi
if [ -f $model_folder ] ; then
   rm "${model_folder}/*"
fi

$cmd JOB=1:$nj "${logdir}/speaker_modeler.JOB.log" "$program" "${model_folder}/" model.JOB.txt "${alignment}/result_ali.JOB" "${record_folder}/" "$record_location";
