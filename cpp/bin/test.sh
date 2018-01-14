cmd=utils/run.pl
program=./SpeakerRecognizer

usage="Usage: $0 <result-file> <model-folder> <test-files> <record-folder> [<log-dir>]
Note: <log-dir> defaults to <model-folder>/log
e.g.: $0 recognized.txt models basefolder/test_files.txt basefolder/data/LibriSpeech/test-clean"

if [ $# -lt 4 ] || [ $# -gt 5 ]; then
  >&2 echo "$usage"
  exit 1
fi
if [[ "$1" = "--help" || "$1" = "-h" ]]; then 
  echo "$usage"
  exit 0
fi 

result="$1"
models="$2"
ideal="$3"
record_folder="$4"

tmpModelFile="${models}/tmp_model.txt"
modelFile="${models}/model.txt"

if [ -f $tmpModelFile ] ; then
    rm $tmpModelFile
fi 
if [ -f $modelFile ] ; then
    rm $modelFile
fi  

cd "${models}"
cat model*.txt > tmp_model.txt
cd ~-
./cat_models.pl "${models}"

logdir="${models}/log"

$program "$result" "$models/" model.txt "$ideal" "$record_folder/";
