#!/bin/bash

# ==== CONFIG ====
GRAPHS_DIR="./graphs/square_mesh"                    # folder with your .dimacs graphs
MY_EXE="./bin/max_flow"                 # your max flow binary
RITT_EXE="./bin/ritt_max_flow"         # ritt's implementation
STRATEGIES=("bfs" "dfs" "fat" "scaling")         # your strategy options
OUTPUT_CSV="results/correctness/results.csv"               # where results will be stored

# ==== INIT OUTPUT FILE ====
echo "graph,strategy,max_flow,time_ns" > "$OUTPUT_CSV"

# ==== CHECK BINARIES ====
if [ ! -x "$MY_EXE" ]; then
  echo "❌ Error: '$MY_EXE' not found or not executable." >&2
  exit 1
fi
if [ ! -x "$RITT_EXE" ]; then
  echo "❌ Error: '$RITT_EXE' not found or not executable." >&2
  exit 1
fi

# ==== LOOP THROUGH GRAPHS AND STRATEGIES ====
for graph in "$GRAPHS_DIR"/*.dimacs; do
  [ -e "$graph" ] || continue  # skip if no .dimacs file

  for strat in "${STRATEGIES[@]}"; do
    echo "▶ Running: $strat on $(basename "$graph")"

    # Measure start time
    START_TIME=$(date +%s.%N)

    # Run and capture only the output (max flow)
    FLOW=$( "$MY_EXE" "$strat" < "$graph" )

    # Measure end time
    END_TIME=$(date +%s.%N)

    # Compute elapsed time in nanoseconds
    TIME_NS=$(echo "($END_TIME - $START_TIME) * 1000000000" | bc | awk '{printf "%.0f", $1}')

    echo "$(basename "$graph"),$strat,$FLOW,$TIME_NS" >> "$OUTPUT_CSV"
  done

  echo "▶ Running: ritt on $(basename "$graph")"

  # Measure start time
  START_TIME=$(date +%s.%N)

  # Run and capture max flow (output only)
  FLOW=$( "$RITT_EXE" < "$graph" )

  # Measure end time
  END_TIME=$(date +%s.%N)

  # Compute elapsed time in nanoseconds
  TIME_NS=$(echo "($END_TIME - $START_TIME) * 1000000000" | bc | awk '{printf "%.0f", $1}')

  echo "$(basename "$graph"),ritt,$FLOW,$TIME_NS" >> "$OUTPUT_CSV"
done

echo "✅ Benchmark complete. Results written to '$OUTPUT_CSV'"
