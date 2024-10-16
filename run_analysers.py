import subprocess as sp
from pathlib import Path
import json
from time import time
import re
import pickle

# Runs set of static tools with specified program arguments on the specified dataset
# Results for each tool-argument pair saved to separate .pickle file


# Returns tool output when analysing a specified file with specified arguments
# Parameters
#   tool: name of tool
#   tool_arg: tool argument (None if no arguments)
#   ex: path to code example file
def analyse_file(tool, tool_arg, ex):

    if tool_arg == None:
        cmd_args = [tool, ex]
    else:
        cmd_args = [tool, tool_arg, ex]
    

    res = sp.run(cmd_args, capture_output=True, text=True)

    out, err = res.stdout, res.stderr 

    # Avoid 'None' in output
    if res.stdout == None:
        out = ""
    if res.stderr == None:
        err = ""

    return out + ' ' + err # concatenate stdout and stderr


# Returns true if vulnerability is detected in tool output, false otherwise
# Parameters:
#   tool: name of tool
#   output: tool output to be analysed
def isVulnerable(tool, output):

    # Pattern matching on tool-by-tool basis 

    if tool == "flawfinder":
        # Search term being present in output indicates vulnerability
        return output.find("Hits = ") != -1

    if tool == "cppcheck":
        # non-zero line indicator shows vulnerability
        return re.search(":[1-9]", output) != None
    
    if tool == "clang-tidy":
        # Search term being present in output indicates vulnerability
        return output.find("errors generated") != -1




# Define directories
DIRNAME = "DiverseVul Sample"

CODE_DIR = Path("Code Examples", DIRNAME)
SECURE_DIR = Path(CODE_DIR,"Secure")
VUL_DIR = Path(CODE_DIR,"Vulnerable")

OUTPUT_DIR = Path("Detection Outputs")
OUTPUT_DIR.mkdir(exist_ok=True)



# Dictionary of tool argument lists
# Each argument will be run on the dataset separately
# Need to include "None" to run tool with no arguments

tool_args = {}
# tool_args["cppcheck"] = [None, "--enable=all"]
tool_args["cppcheck"] = [None]
# tool_args["flawfinder"] = [None, "--falsepositive"] 
# tool_args["flawfinder"] = ["--minlevel=0", "--minlevel=1", "--minlevel=2", "--minlevel=3", "--minlevel=4", "--minlevel=5"]
tool_args["flawfinder"] = [None]
# tool_args["clang-tidy"] = ["--checks=-*,bugprone-*,cert-*,clang-analyzer-*,concurrency-*"]
tool_args["clang-tidy"] = [None]

# Create list of tool-argument combinations to run
combinations = []
for tool in tool_args:
    for arg in tool_args[tool]:
        combinations.append((tool,arg))


# Load dataset
file = open("diversevul_2.json")
dataset = json.load(file)




for tool, arg in combinations:
    start_time = time()

    if arg == None:
        print(f"Running tool: {tool} (No args)...")
    else:
        print(f"Running tool: {tool} (Argument: {arg})...")

        
    performances = {}
    
    performances[tool] = {}

    print("Checking vulnerable examples...")
    for ex in VUL_DIR.iterdir():

        # Get commit id from filename
        if "-" in ex.name:
            id = ex.name.split("-")[0]
            id = id.split(".")[0]
        
        else:
            id = ex.name.split(".")[0]

        # Get CWE
        cwe = dataset[id]["cwe"][0]


        # Get prediction
        out = analyse_file(tool, arg, ex)
        prediction = 1 if isVulnerable(tool, out) else 0

        # Update dictionary with tool prediction for current example
        performances[tool][ex.name] = {}

        performances[tool][ex.name]["cwe"] = cwe
        performances[tool][ex.name]["vulnerable"] = 1
        performances[tool][ex.name]["predicted"] =  prediction



    print(f"Checking secure examples... ({time()-start_time} elapsed)")
    for ex in SECURE_DIR.iterdir():

        
        # Get commit id from filename
        if "-" in ex.name:
            id = ex.name.split("-")[0]
            id = id.split(".")[0]
        
        else:
            id = ex.name.split(".")[0]

        # Get CWE
        cwe = dataset[id]["cwe"][0]

        # Get prediction
        out = analyse_file(tool, arg, ex)
        prediction = 1 if isVulnerable(tool, out) else 0

        # Update dictionary with tool prediction for current example
        performances[tool][ex.name] = {}

        performances[tool][ex.name]["cwe"] = cwe
        performances[tool][ex.name]["vulnerable"] = 0
        performances[tool][ex.name]["predicted"] =  prediction


    if arg == None:
        out_file_name = "output=" + tool + ".pickle"
    else: # Include tool args in filename (for graphs)
        arg_name = arg.split("-",2)[-1]
        arg_name = arg_name.replace("*","") # Asterisks cannot be used in filenames
        out_file_name = "output=" + tool + "-" + arg_name  + ".pickle"

    out_file_dir = Path(OUTPUT_DIR, DIRNAME)
    out_file_dir.mkdir(exist_ok=True) # Create folder if it doesn't exist

    # Create directory for each tool
    tool_dir = Path(out_file_dir, tool)
    tool_dir.mkdir(exist_ok=True)

    out_file_path = Path(tool_dir, out_file_name)

    with open(out_file_path, "wb") as file:
        pickle.dump(performances, file)


    print("All examples checked. Time taken: ",time() - start_time)
