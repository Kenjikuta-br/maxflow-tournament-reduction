#!/bin/bash

###############################################################################
# Script: create_dataset2.sh
# Description:
#   This script generates tournament instances using the bin/gen_tournament
#   executable. It fixes the number of teams and rounds and varies two
#   parameters: the fraction of games already played (alpha) and the bias of
#   team 1 (bias).
#
#   For each combination of alpha and bias, it creates a subdirectory inside
#   dataset2/ and generates a specified number of instances.
#
# Purpose:
#   This dataset will be used to study how the probability of team 1 winning
#   depends on the fraction of games already played and the team 1 bias.
###############################################################################

# ======================== CONFIGURABLE PARAMETERS ===========================
TEAMS=100              # Number of teams (fixed)
ROUNDS=10              # Number of rounds (fixed)
INSTANCES=100          # Number of instances per (alpha, bias) pair
ALPHA_START=0.0        # Starting value of alpha
ALPHA_END=1.0          # Ending value of alpha
ALPHA_STEP=0.1         # Step size for alpha

BIAS_START=-1.0        # Starting value of bias
BIAS_END=1.0           # Ending value of bias
BIAS_STEP=0.1          # Step size for bias

DATASET_DIR="dataset2" # Output dataset directory
EXECUTABLE="./bin/gen_tournament"  # Path to tournament generator executable
# ===========================================================================

# Create main dataset directory if it doesn't exist
mkdir -p "$DATASET_DIR"

# Generate instances
echo "Generating dataset in $DATASET_DIR..."

# Loop over all alpha values
alpha=$ALPHA_START
while (( $(echo "$alpha <= $ALPHA_END" | bc -l) )); do

    # Loop over all bias values
    bias=$BIAS_START
    while (( $(echo "$bias <= $BIAS_END" | bc -l) )); do

        # Format folder names to avoid issues with decimal points
        alpha_label=$(printf "%03.1f" "$alpha" | tr '.' '_')
        bias_label=$(printf "%+04.1f" "$bias" | tr '.' '_')

        # Directory for this configuration
        dir="$DATASET_DIR/alpha_${alpha_label}__bias_${bias_label}"
        mkdir -p "$dir"

        echo "Generating $INSTANCES instances for alpha=$alpha, bias=$bias..."

        for ((i = 1; i <= INSTANCES; i++)); do
            output_file="$dir/instance_$i.txt"
            $EXECUTABLE "$TEAMS" "$ROUNDS" "$alpha" "$bias" > "$output_file"
        done

        # Increment bias
        bias=$(echo "$bias + $BIAS_STEP" | bc)
    done

    # Increment alpha
    alpha=$(echo "$alpha + $ALPHA_STEP" | bc)
done

echo "Dataset generation completed in $DATASET_DIR"
