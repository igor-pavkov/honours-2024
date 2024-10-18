# Honours 2024
*Integrating Static Analysers and Large Language Models for Code Vulnerability Detection*



**Note that the scripts in sections 1 and 2 do not need to be re-run as the resulting files are already part of the repository.**

## 1. Preparing Examples
### diversevul_create_files
Reads diversevul_2.json and performs the data filtering described in dissertation. Example files are located in 'Code Examples/DiverseVul Filtered'. 

Filtering steps:
1. Remove commits that include more than one CWE
2. Remove commits that include more than one vulnerable file
3. Remove all files that produce syntax errors

### create_samples.py
Uses the files produced by the previous script to create a balanced sample of vulnerable and secure code examples. Sample files are located in the directory 'Code Examples/DiverseVul Sample.


## 2. Running models
### run_analysers.py 
Runs static tools on the data sample and stores results in .pickle file. Edit *tool_args* dictionary to control which tools are run with which progam arguments. Example:
```python
# Run cppcheck without arguments
tool_args["cppcheck"] = [None]

# Run flawfinder with default settings, then with falsepositive flag
tool_args["flawfinder"] = [None, "--falsepositive"]

# Clang-tidy list is empty, so the tool is not run
tool_args["clang-tidy"] = []
```

### run_llms.py
Runs LLMs on the data sample and stores results in .pickle file. 

Modify the *LLM_MODELS* list to control which LLMs are run. Supported models:
* gpt-4o
* gpt-4o-mini
* o1-mini

Modify the *PROMPT_VARIATION* string to select prompt variant used with LLM. Options include:
* **Empty string ("")**: Standard prompt
* **role**: Role-based prompt
* **anti**: Anti prompt
* **chain**: Chain-of-thought prompt
* **emotive**: Emotive prompt
* **context**: Static-enhanced prompt

## 3. Displaying results

### print_graphs.py 
#### Script Flags
* **-f**: Name of tool results directory inside "Graph Inputs". Defaults to "DiverseVul Sample" if flag is not used.
* **-total**: Don't separate top-25 results

Shows graphs of tools/ensembles, prints performance metrics for each tool/ensemble and saves most recent tool metrics in *tool_results.txt*. For each list in the variable *graph_metrics*, a separate graph is produced containing the metrics in that last. The following metrics are available:
* accuracy
* fpr (false positive rate)
* recall
* precision
* f1 (F1 score)



#### Including Tools
To include individual tools/LLMs in the graphs, include their results .pickle files (generated in the last section) in the results directory. Tool ensembles are represented by a folder within the results directory. Results files inside the ensemble folder will be part of that ensemble. The ensemble voting method can be specified by including it in a hyphen after the ensemble name (e.g., "LLMs-majority"). Otherwise, the folder name becomes the name of the ensemble on the graphs, and the voting method specified in the variable *default_vote_method* will be used.

