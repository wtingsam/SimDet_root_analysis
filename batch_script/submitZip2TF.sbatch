#!/bin/bash
# Load Taurus modules
module load modenv/eb
module load TensorFlow/1.7.0-goolfc-2018a-Python-3.6.4

# cd into the input directory
cd $TMP_INPUT
[ -e $1 ] && EXE_APP="$1" \
    || echo "Executable not found"
# Find the input file list
[ -e $TMP_INPUT/$2 ] && FILELIST="${TMP_INPUT}/$2" \
    || echo "File list not found"
# Enter number of bins 
NBINS=$3

THIS_JOB_OUTPUT=$TMP_OUTPUT
# Get the configration file for this job
FirstFile=$(get_first_file $SH_RUN_NUMBER 1)
ListOfSimDetRootFiles=${TMP_INPUT}/tmp_list.txt
cat $FILELIST | sort > $ListOfSimDetRootFiles
A_FILE=`get_input_file_list $ListOfSimDetRootFiles $FirstFile 1`
# Run 
OUTPUT_NAME=`echo $A_FILE | sed "s/.*\/\([^\/]*\).zip/\1/"`
CMD="$A_FILE $TMP_OUTPUT $NBINS"
python ${TMP_INPUT}/${EXE_APP} $CMD
