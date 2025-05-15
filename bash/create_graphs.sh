#!/bin/bash

# chmod +x bash/create_graphs.sh

BIN="bin/gengraph"
BASE_OUTPUT_DIR="graphs"

echo "Using generator: $BIN"

# Function to generate 30 graphs of a given type with varied parameters
generate_graphs() {
    graph_type=$1
    output_subdir=$2
    shift 2
    mkdir -p "$BASE_OUTPUT_DIR/$output_subdir"

    echo "Generating 30 graphs of type $graph_type in $BASE_OUTPUT_DIR/$output_subdir"

    for i in $(seq 1 30); do
        # Generate parameters depending on the graph type
        case "$graph_type" in
            5)  # Square Mesh Graph
				# Simulates a 2D grid where each node is connected to its neighbors.
				# Parameters:
				#   s = side length of the mesh (graph has s*s nodes)
				#   d = fixed max degree (50)
				#   C = fixed max capacity (1000)

				# Range of s from 50 to 110 (inclusive), increasing by 1
				s=$((50 + (i - 1) * 2))  # Para i = 1 → s = 50, i = 2 → s = 52, ..., i = 31 → s = 110
				d=25
				C=1000
				output_file="$BASE_OUTPUT_DIR/$output_subdir/${i}graph_${s}_${d}_${C}.dimacs"
				$BIN $graph_type $s $d $C $output_file
				;;
            4)  # Matching Graph
				# Bipartite graph where each node on the left is matched to a subset on the right.
				# Parameters:
				#   n = number of nodes in each part 
				#   d = fixed degree (40)
				#   C = fixed max capacity (1000)

				n=$((2500 + (i - 1) * 165))

				d=25
				C=1000
				output_file="$BASE_OUTPUT_DIR/$output_subdir/${i}graph_${n}_${d}_${C}.dimacs"
				$BIN $graph_type $n $d $C $output_file
				;;
            3)  # Random 2-Level Graph
				# A layered graph with a grid-like structure, consisting of vertices arranged into r rows and c levels.
				# Parameters:
				#   r = number of rows 
				#   c = number of columns 
				#   C = fixed max capacity (1000)
				#
				# The total number of vertices in the graph is r * c, where r and c represent the number of rows and columns, respectively.
				# The values of r and c are chosen to keep the total number of vertices between 5000 and 20000, ensuring that the product r * c stays within this range.
				#
				# The graph structure:
				#   - The graph consists of r * c vertices, arranged in a layered grid-like structure.
				#   - The source vertex (0) is connected to the first r vertices (from the first row).
				#   - The sink vertex (r * c + 1) is connected to the last r vertices (from the last row).
				#   - Between each level (row), vertices are connected to 3 random vertices in the next level (row).
				#   - Capacities for edges are randomly chosen between 1 and C (where C = 1000).
				#   - The vertices are connected randomly, with each vertex in a row having edges to 3 randomly chosen vertices in the next row.
				#   - The total number of edges is influenced by r, c, and the random selection of vertices between rows.
				#
				r=$((50 + (i - 1) * 2))  # Para i = 1 → s = 50, i = 2 → s = 52, ..., i = 31 → s = 110
				c=$((50 + (i - 1) * 2))  # Para i = 1 → s = 50, i = 2 → s = 52, ..., i = 31 → s = 110

				C=1000
				output_file="$BASE_OUTPUT_DIR/$output_subdir/${i}graph_${r}_${c}_${C}.dimacs"
				$BIN $graph_type $r $c $C $output_file
				;;
            6)  # Basic Line Graph
				# The BasicLine graph is somewhat grid-like, but it differs from a mesh graph in its connections and structure.
				# Structure of a BasicLine Graph:
				#   Vertices: Like a mesh graph, it has n × m vertices. However, these vertices are not all connected to their neighbors in a regular grid structure.
				#   Edges:
				#     - The source vertex is connected to the first m vertices (like the first row of the grid).
				#     - The sink vertex is connected to the last m vertices (like the last row of the grid).
				#     - Each vertex between the source and sink is connected randomly to deg other vertices. The edges are randomly selected, so the structure of connections is not regular as in a mesh graph.
				#   In summary, while there is a grid of vertices, the connections in the BasicLine graph are not based purely on adjacency (like in a mesh graph). Instead, each vertex is connected randomly to a number of vertices, and the connections are based on a random subset selection of other vertices, not based on the grid's immediate neighbors.
				#
				# Parameters:
				#   n = number of lines (75 to 145, increasing by 5 every 2 graphs)
				#   m = number of segments per line (75 to 145, increasing by 5 every 2 graphs)
				#   d = fixed inter-line connection degree (50)
				#   C = fixed max capacity (1000)
				n=$((50 + (i - 1) * 2))  # Para i = 1 → s = 50, i = 2 → s = 52, ..., i = 31 → s = 110
				m=$((50 + (i - 1) * 2))  # Para i = 1 → s = 50, i = 2 → s = 52, ..., i = 31 → s = 110
				d=25
				C=1000
				output_file="$BASE_OUTPUT_DIR/$output_subdir/${i}graph_${n}_${m}_${d}_${C}.dimacs"
				$BIN $graph_type $n $m $d $C $output_file
				;;
        esac
    done
}

# Generate datasets for each graph type
generate_graphs 5 "square_mesh"
generate_graphs 4 "matching"
generate_graphs 3 "random_2level"
generate_graphs 6 "basic_line"

echo "All datasets generated successfully!"
