EXE_APP="/home/s6875712/work/SimDet_root_analysis/bin/pixelizeHits"
INPUT_LIST="input_list.txt"
OUTPUT_DIR="/scratch/s6875712/machine_learning/simDet_zip/"
INPUT_DIR="/home/s6875712/work/SimDet_root_analysis/inputs/"
BATCH_DIR="/home/s6875712/work/SimDet_root_analysis/batch_script/"

SubmitJobArray.sh \
    -d \
    --slurm \
    -e append \
    -o $OUTPUT_DIR \
    -i $INPUT_DIR \
    -s simPix128_30h_90to95 \
    -N ZIP \
    -r 20 \
    -n 283 \
    -Q "
        --time=99:99:99
        --mem=4GB
        "\
    $BATCH_DIR/submitSimDet2zip.sbatch \
    $EXE_APP \
    $INPUT_LIST
