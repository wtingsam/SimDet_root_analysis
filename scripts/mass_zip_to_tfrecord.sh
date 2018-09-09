EXE_MOD="write_tfrecord_from_zip.py"
INPUT_LIST="input_list_zip.txt"
OUTPUT_DIR="/scratch/s6875712/machine_learning/simDet_tf/"
INPUT_DIR="/home/s6875712/work/SimDet_root_analysis/inputs/"
BATCH_DIR="/home/s6875712/work/SimDet_root_analysis/batch_script/"
NPIXELS=128

SubmitJobArray.sh \
    -d \
    --slurm \
    -e append \
    -o $OUTPUT_DIR \
    -i $INPUT_DIR \
    -s simTF${NPIXELS}_30h_90to95 \
    -N TF \
    -r 20 \
    -n 283 \
    -Q "
        --time=99:99:99
        --mem=16GB
        "\
    $BATCH_DIR/submitZip2TF.sbatch \
    $EXE_MOD \
    $INPUT_LIST \
    $NPIXELS
