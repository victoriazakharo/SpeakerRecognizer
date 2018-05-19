# we assume KALDI_ROOT is already defined
[ -z "$KALDI_ROOT" ] && echo >&2 "The variable KALDI_ROOT must be already defined" && exit 1
# The formatting of the path export command is intentionally weird, because
# this allows for easy diff'ing
export PATH=\
${KALDI_ROOT}/src/bin:\
${KALDI_ROOT}/src/featbin:\
${KALDI_ROOT}/src/gmmbin:\
$PATH
