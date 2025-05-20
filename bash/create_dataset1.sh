#!/usr/bin/env bash
#
# chmod +x bash/create_dataset1.sh
# ./bash/create_dataset1.sh
#
# This script generates tournament instances using the bin/gen_tournament executable.
# It creates a top-level folder (DATASET_DIR) and, for each team size in the specified range,
# creates a subfolder named after the team size. Inside each subfolder, it generates 10 instances.
#
# In this dataset, the number of rounds (ROUNDS), fraction of games played (α), and team 1 bias (BIAS)
# are fixed to analyze how execution time scales with the number of teams.

# === CONFIGURATION ===
# Starting number of teams
START_TEAMS=10
# Ending number of teams
END_TEAMS=400
# Increment step for team sizes
STEP_TEAMS=10

# Number of instances to generate per team size
NUM_INSTANCES=10

# Fixed parameters passed to bin/gen_tournament:
ROUNDS=10           # Initial rounds between each pair of teams
ALPHA=0.5           # Fraction of games played (α ∈ [0, 1], where 1 = all games)
BIAS=0              # Bias for team 1 (0 = no bias)

# Output dataset directory
DATASET_DIR="dataset1"
# ======================

# Create the top-level dataset directory if it doesn't exist
mkdir -p "$DATASET_DIR"

# Loop over team sizes
for (( teams=START_TEAMS; teams<=END_TEAMS; teams+=STEP_TEAMS )); do
    # Create a subdirectory named after the team size
    SUBDIR="$DATASET_DIR/$teams"
    mkdir -p "$SUBDIR"

    echo "Generating $NUM_INSTANCES instances for $teams teams..."

    # Generate NUM_INSTANCES files for this team size
    for (( i=1; i<=NUM_INSTANCES; i++ )); do
        OUTFILE="$SUBDIR/instance_${i}.txt"
        ./bin/gen_tournament "$teams" "$ROUNDS" "$ALPHA" "$BIAS" > "$OUTFILE"
    done
done

echo "Dataset generation complete. Files saved in '$DATASET_DIR/'."