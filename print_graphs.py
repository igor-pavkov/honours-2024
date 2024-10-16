import pickle
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from pathlib import Path
import numpy as np
import argparse
import random



# Set random seed for deterministic majority voting
random.seed(1234)

# Return majority vote for an example file (1 = vulnerable, 0 = secure)
# Parameters:
#   ex: name of example file
#   full_dict: dictionary containing predictions of each tool on each code example file
#   toollist: list of tools/llms in ensemble
#   method: voting method (any/all/majority)
def ensemble_vote(ex, full_dict, toollist, method="majority"):
    
    pred_list = [] # list of predictions for each tool
    for tool in toollist:
        pred_list.append(full_dict[tool][ex]["predicted"])


    if method == "all": # Only vote positive if all tools predict positive
        predicted = 1 if 0 not in pred_list else 0

    if method == "any": # Vote positive if any tool predicts positive
        predicted = 1 if 1 in pred_list else 0
    
    if method == "majority": # Vote the majority prediction
        pos_count = pred_list.count(1)
        neg_count = pred_list.count(0)


        if pos_count > neg_count:
            predicted = 1

        elif pos_count < neg_count:
            predicted = 0

        else: # tiebreaker
            predicted = random.choice([0,1])


    return predicted


# Returns specified metric for specified tool
# Parameters:
#   metric: name of metric (accuracy, fpr, precision, recall, f1)
#   tool: name of tool
#   results: dictionary containing number of fp,tp,fn,tn detections for each tool
def calculate_metric(metric, tool, results):
    
    # Define aliases for easier referencing
    tp = results[tool]["tp"]
    tn = results[tool]["tn"]
    fp = results[tool]["fp"]
    fn = results[tool]["fn"]


    if metric == "accuracy":
        return (tp + tn) / (tp + tn + fp + fn)

    elif metric == "fpr":
        return fp / (fp + tn)
    
    elif metric == "precision":
        return tp / (tp + fp)

    elif metric == "recall":
        return tp / (tp + fn)
    
    elif metric == "f1":
        prec = tp / (tp + fp)
        rec = tp / (tp + fn)

        return 2*prec*rec / (prec + rec)


# Define command line arguments
parser = argparse.ArgumentParser()
parser.add_argument("-f", default="DiverseVul Sample") # Specify folder
parser.add_argument("-total", default=argparse.SUPPRESS, action="store_const", const=True) # combine top 25 and other into one group

args = parser.parse_args()


# Input folder containing tool/LLM results .pickle files
DICT_FOLDER = Path("Graph Inputs", args.f)

if not DICT_FOLDER.exists():
    print(f"Invalid folder name specified: {args.f}")
    exit(-1)

full_dict = {} # results for every individual tool

individual = [] # List of individual tools (not in ensembles)
ensembles = {} # Dictionary containing tool lists for every ensemble


for dict_file in reversed(list(DICT_FOLDER.iterdir())):

    # Skip exclude folder
    if dict_file.is_dir():
        foldername = dict_file.name
        if foldername == "Exclude": # Store results without them appearing in graph
            continue
        else: # Ensemble folder
            ensembles[foldername] = []
            
            # Load tool results inside ensemble folder
            for tool_file in dict_file.iterdir(): 
                with open(tool_file, "rb") as file:
                    tool_dict = pickle.load(file)

                    tool = tool_file.name.split(".")[-2]
                    tool = tool.split("=",1)[-1] # Split only first occurrence of '='

                    if tool not in full_dict: # Avoid repeating the same tool entry later
                        full_dict[tool] = list(tool_dict.values())[0]

                    ensembles[foldername].append(tool)
            
            continue


    with open(dict_file, "rb") as file:

        tool_dict = pickle.load(file)

        tool = dict_file.name.split(".")[-2]
        tool = tool.split("=",1)[-1] # Split only first occurrence of '='

        if tool not in full_dict:
            full_dict[tool] = list(tool_dict.values())[0]

        individual.append(tool)


# Get list of examples
first_tool = list(full_dict.keys())[0]

ex_list = list(full_dict[first_tool].keys()) # List of code examples 

print("Number of Examples: ", len(ex_list))

# Read top-25 vulnerabilities
with open("top25.txt", "r") as top25_file:
    top25_list = top25_file.read().split('\n')


# Separate CWEs if no total flag
if "total" in args:
    results = {"total": {}}
else:
    results = {"top25": {}, "other": {}}

# Generate stats for individual tools (tp, tn, fp, fn)
for tool in full_dict:
    for category in results:
        results[category][tool] = {"tp": 0, "tn": 0, "fp": 0, "fn": 0}


default_vote_method = "majority"

# Generate stats for ensembles
for ensemble in ensembles:

    for category in results:
        results[category][ensemble] = {"tp": 0, "tn": 0, "fp": 0, "fn": 0}



# Aggregate results for all files (tp,tn,fp,fn)
for ex in ex_list: 

    cwe = full_dict[first_tool][ex]["cwe"]


    # Select the correct dictionary for the code example (top-25 cwe or not)
    if "total" in args:
        results_dict = results["total"]
    else:
        results_dict = results["top25"] if cwe in top25_list else results["other"]


    # Vulnerable
    for tool in full_dict:
        # The tool detected a vulnerability
        if full_dict[tool][ex]["predicted"] == 1:
    
            # The file was actually vulnerable
            if full_dict[tool][ex]["vulnerable"] == 1:
                results_dict[tool]["tp"] += 1

            else:
                results_dict[tool]["fp"] += 1
        
        # The tool did not detect a vulnerability
        else:
            # The fle was actually vulnerable
            if full_dict[tool][ex]["vulnerable"] == 1:
                results_dict[tool]["fn"] += 1
            else:
                results_dict[tool]["tn"] += 1
    

    # Calculate results for tool ensemble
    for ensemble in ensembles:

        # Use method specified in filename
        if "-" in ensemble:
            method = ensemble.split("-")[-1]
        else:
            method = default_vote_method

        if ensemble_vote(ex, full_dict, ensembles[ensemble], method) == 1: # Ensemble predicted vulnerability
            if full_dict[tool][ex]["vulnerable"] == 1:
                results_dict[ensemble]["tp"] += 1
            
            else:
                results_dict[ensemble]["fp"] += 1
        
        else: # Ensemble did not predict vulnerability
            if full_dict[tool][ex]["vulnerable"] == 1:
                results_dict[ensemble]["fn"] += 1
            
            else:
                results_dict[ensemble]["tn"] += 1



for category in results:
    print(results[category])

first_category = list(results.keys())[0]


# Calculate metrics for top-25 and other
metrics_list = ["accuracy", "fpr", "recall", "precision", "f1"]

metrics = {}
for metric in metrics_list:
    metrics[metric] = {}

    for category in results:
        metrics[metric][category] = {}

    # calculate metrics for every tool/ensemble in results
    for tool in results[first_category]:
        for category in results:
            metric_val = calculate_metric(metric, tool, results[category])
            metrics[metric][category][tool] = metric_val

    


places = 3 # Number of decimal places for printing metrics 
print_list = individual + list(ensembles.keys())

# Print and save results to file for easier reading
with open("tool_results.txt", "w") as res_file:
    print("Tool Metrics:")
    res_file.write("Tool Metrics:" + "\n")

    for tool in print_list:
        print(f"\nTool: {tool}")
        res_file.write(f"\nTool: {tool}\n")

        for metric in metrics:

            res_string = f"{metric}: "

            if "total" in args:
                res_string += f"{metrics[metric]["total"][tool]:.{places}f}"
            else:

                for category in results:
                    res_string += f"{category}: {metrics[metric][category][tool]:.{places}f}, "

                
                res_string = res_string[:-2]
            
            print(res_string)
            res_file.write(res_string + "\n")


# Each list will print a separate graph containing only that subset of metrics
graph_metrics = [
    # ["accuracy", "fpr", "f1"],
    # ["precision", "recall"],
    metrics_list
]



# Graph Settings
barWidth = 0.08
linewidth = 1.5
titlesize = 24 # Title font size
axsize = 20 # Axis font size
labelsize = 18 # Label font size
legendsize=10

# Set title and axis labels manually
graphtitle = "Tool Metrics"  
xlabel = "Tool"

plt.rc("font", family="arial")
plt.rc("font", weight="normal")
plt.rc("axes", titlesize=titlesize)
plt.rc("axes", labelsize=axsize)
plt.rc("axes", labelpad=15)
plt.rc("legend", fontsize=legendsize)
plt.rc("xtick", labelsize=labelsize)
plt.rc("ytick", labelsize=labelsize)

# How metric names should display in the legend
display = {
    "accuracy": "Accuracy",
    "fpr": "FPR",
    "recall": "Recall",
    "precision": "Precision",
    "f1": "F1"
}

# Colours
# Accuracy = orange
# Precision = blue
# Recall = green
# FPR = red
# F1 = purple 

# (colour-top25, colour-other)
green = ("#4CB140", "#BFFCC6")
orange = ("#EC7A08","#FFCBA2")
blue = ("#06C", "#AFCBFF")
red = ("#F44336", "#FFBEBC")
purple = ("#981CEB", "#DEB7FF")

# Swap to change graph colour scheme
colors = {
    "accuracy": green, 
    "recall": blue, 
    "precision": orange,
    "fpr": red, 
    "f1": purple
}




columns = individual + list(ensembles.keys())


n_metrics = [len(g) for g in graph_metrics]


# for each graph: list of bar positions for each metric
pos_lists = [[np.arange(len(columns))] for g in graph_metrics] 

 # Add position of first bars
for i in range(len(pos_lists)):
    pos_lists[i].append(pos_lists[i][-1] + barWidth)



for i in range(len(n_metrics)):

    for j in range(n_metrics[i]):
        pos_lists[i].append(pos_lists[i][-1] +barWidth)

        # Only add second bar if separating CWEs
        if "total" not in args:
           pos_lists[i].append(pos_lists[i][-1] +barWidth)


for i in range(len(graph_metrics)):

    fig = plt.figure()
    pos = 0
    for metric in graph_metrics[i]:
        # Generate y values

        y = {} # Dictionary of lists
        for category in results:
            y[category] = []
            for tool in columns:
                y[category].append(metrics[metric][category][tool])

            if category == "top25" or category == "total":
                plt.bar(pos_lists[i][pos], y[category], width=barWidth, color=colors[metric][0], edgecolor='black', linewidth=linewidth)
            else:
                plt.bar(pos_lists[i][pos+1], y[category], width=barWidth, color=colors[metric][1], edgecolor='black', linewidth=linewidth)


        if "total" in args:
            pos += 1
        else:
            pos += 2


            
        
    # Creating plot legend
    patchlist = []

    col_ind = 0 # Colour index
    for category in results:
        for metric in graph_metrics[i]:
            if "total" in args:
                patchlist.append(mpatches.Patch(color=colors[metric][0], label=metric))
            
            else:
                category_label = display[metric] + "-" + category
                patchlist.append(mpatches.Patch(color=colors[metric][col_ind], label=category_label))

        
        col_ind += 1

                


    plt.title(graphtitle)
    # plt.legend(handles=patchlist, ncol=2, loc="upper right", bbox_to_anchor=(1,1.05)) # added control over legend position
    plt.legend(handles=patchlist, ncol=2)

    plt.xlabel(xlabel)


    # Ensure x ticks are centered on bar groups
    if "total" in args:
        plt.xticks([r + (n_metrics[i]-1)*0.5*barWidth for r in range(len(columns))], columns)
    else:
        plt.xticks([r + (n_metrics[i]-0.5)*barWidth for r in range(len(columns))], columns)


    # Make spines invisible
    ax = fig.axes[0]
    ax.spines["right"].set_visible(False)
    ax.spines["top"].set_visible(False)



plt.tight_layout()

plt.show()

