import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file into a DataFrame
df = pd.read_csv("execution_times.csv")

# Create the plot
plt.figure(figsize=(10, 6))  # Set the figure size
plt.plot(df["TeamSize"], df["TimeAvgSeconds"], marker='o', color='steelblue')  # Line plot with circular markers

# Configure the plot
plt.title("Execution Time vs Team Size")  # Title of the plot
plt.xlabel("Number of Teams")  # Label for the x-axis
plt.ylabel("Average Execution Time (seconds)")  # Label for the y-axis
plt.grid(True)  # Enable gridlines
plt.tight_layout()  # Adjust layout to prevent clipping

# Save the plot as an image file
plt.savefig("execution_time_plot.png")
