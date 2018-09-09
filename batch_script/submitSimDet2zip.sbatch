#!/bin/bash

# cd into the input directory
cd $TMP_INPUT
[ -e $1 ] && EXE_APP="$1" \
          || echo "Executable not found"
# Find the input file list
[ -e $TMP_INPUT/$2 ] && FILELIST="${TMP_INPUT}/$2" \
                     || echo "File list not found"

THIS_JOB_OUTPUT=$TMP_OUTPUT
# Get the configration file for this job
FirstFile=$(get_first_file $SH_RUN_NUMBER 1)
ListOfSimDetRootFiles=${TMP_INPUT}/tmp_list.txt
cat $FILELIST | sort > $ListOfSimDetRootFiles
A_FILE=`get_input_file_list $ListOfSimDetRootFiles $FirstFile 1`
# Run 
OUTPUT_NAME=`echo $A_FILE | sed "s/.*\/\([^\/]*\).root/\1/"`
CMD="bin $TMP_OUTPUT/$OUTPUT_NAME $A_FILE"
${EXE_APP} $CMD
zip -r -X "$TMP_OUTPUT/$OUTPUT_NAME" "$TMP_OUTPUT/$OUTPUT_NAME.bin"
rm -f "$TMP_OUTPUT/$OUTPUT_NAME.bin"
