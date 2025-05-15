#!/usr/bin/env bash
set -euo pipefail

# ==== CONFIG ====
GRAPHS_DIR="./graphs"                     # folder with your .dimacs graphs
MY_EXE="./bin/robust_test"                # your benchmarking executable

# List of graph folders inside GRAPHS_DIR
GRAPH_FOLDERS=("basic_line" "matching" "random_2level" "square_mesh")

# ==== RUN BENCHMARKS ====
for folder in "${GRAPH_FOLDERS[@]}"; do
    OUTPUT_DIR="results/iterations/$folder"
    OUTPUT_CSV="$OUTPUT_DIR/robust_results.csv"

    # Prepare output directory and CSV file
    mkdir -p "$OUTPUT_DIR"
    cat <<'EOF' | tr -d '\n' > "$OUTPUT_CSV"
graph_name;graph_type;algorithm;max_flow;avg_runtime;avg_iterations;avg_bound;avg_r;avg_m;avg_n;avg_cFrac;avg_rBar;
avg_time_per_mI;avg_time_per_nI;avg_time_over_nm;avg_time_over_I_sntm;
avg_s_bar;avg_t_bar_forward;avg_t_bar_residual;avg_insert_norm;avg_delete_norm;avg_update_norm_m;avg_update_norm_theoretical
EOF

    printf '\n' >> "$OUTPUT_CSV"

    # ==== CHECK EXECUTABLE ====
    if [[ ! -x "$MY_EXE" ]]; then
        echo "❌ Error: executable '$MY_EXE' not found or not executable." >&2
        exit 1
    fi

    # Run benchmarks for each graph in the current folder
    for graph in $(ls "$GRAPHS_DIR/$folder"/*.dimacs | sort -V); do
        [[ -e "$graph" ]] || continue   # skip if no .dimacs files

        base=$(basename "$graph")

        # Pipe graph into the tester, prefix each line with the folder name (type of graph),
        # then prefix each line with graph name, and convert commas to semicolons for a uniform delimiter
        "$MY_EXE" < "$graph" \
            | sed -e "s/^/${folder};/" -e "s/^/${base};/" -e "s/,/;/g" \
            >> "$OUTPUT_CSV"
    done

    echo "✅ Benchmark complete for '$folder'. Results written to '$OUTPUT_CSV'"
done

echo "✅ All benchmarks completed."
