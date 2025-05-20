import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Load the dataset from the correct path
df = pd.read_csv("dataset2_summary.csv")

# Pivot the table to format it for the heatmap
heatmap_data = df.pivot(index="bias", columns="alpha", values="percentage_sim")

# Sort bias index so it goes from -1 to 1 (bottom to top in the plot)
heatmap_data = heatmap_data.sort_index(ascending=False)

# Create the heatmap
plt.figure(figsize=(12, 8))
sns.heatmap(
    heatmap_data,
    annot=True,
    fmt=".0f",
    cmap="RdYlGn",  # Red for low values, Green for high
    cbar_kws={'label': 'Percentage of "Sim"'}
)

# Configure the plot
plt.title("Heatmap of 'Sim' Percentage by Alpha and Bias")
plt.xlabel("Alpha")
plt.ylabel("Bias")

# Save the heatmap
plt.tight_layout()
plt.savefig("heatmap_alpha_bias.png")
