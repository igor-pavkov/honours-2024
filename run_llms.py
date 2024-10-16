from pathlib import Path
import json
from time import time
import pickle
import argparse
from openai import OpenAI

# Runs set of LLMs with specified prompt variation on the specified dataset
# Results for each LLM saved to separate .pickle file


API_KEY = "placeholder" # replace with OpenAI API key

# Returns prediction of specified LLM model with specified example
# Parameters:
#   model: model name (gpt-4o, gpt-4o-mini, o1-mini)
#   ex: file path for code example
def isVulnerable(model, ex):
    # print(ex.name)

    with open(ex, "r", encoding="utf-8") as ex_file:
        code = ex_file.read()     

    code = code[:50000] # Cutoff to fit the context window if example is too long


    if PROMPT_VARIATION == "role":
        prompt = f"""{code} You are a cybersecurity expert. Analyse the above C/C++ code snippet.
            Respond only with a single number.
            If the snippet contains any security vulnerabilities, respond with 1. 
            Otherwise, respond with 0"""


    elif PROMPT_VARIATION == "emotive":

        prompt = f"""{code} Analyse the above C/C++ code snippet.
            Respond only with a single number.
            If the snippet contains any security vulnerabilities, respond with 1. 
            Otherwise, respond with 0. Your correct classification is extremely important to my work."""

        
    elif PROMPT_VARIATION == "context":

        # Add tool predictions to prompt
        pred_list = []
        for tool in full_dict:
            # Get tool prediction

            pred = "vulnerable" if full_dict[tool][ex.name]["predicted"] == 1 else "secure"

            # Tool with arguments
            if tool != "clang-tidy" and tool.count("-") > 0:
                toolname, arg = tool.split("-",1) # Separate tool from argument

                arg = "--" + arg

            else:
                toolname = tool
                arg = "none"


            # pred_list.append({"tool": toolname, "tool arg": arg, "vulnerable": pred})
            pred_list.append({"tool": toolname, "result": pred})
        
        
        
        prompt = f"""{code} Analyse the above C/C++ code snippet.
            Respond only with a single number.
            If the snippet contains any security vulnerabilities, respond with 1. 
            Otherwise, respond with 0. 
            A range of code analysis tools analysed the same snippet and produced the following results (in JSON format): {pred_list}"""


    elif PROMPT_VARIATION == "anti":
        prompt = f"""{code} Analyse the above C/C++ code snippet.
            Respond only with a single number.
            If the snippet contains no security vulnerabilities, respond with 0. 
            Otherwise, respond with 1"""

    else: # Standard Prompt
        prompt = f"""{code} Analyse the above C/C++ code snippet.
            Respond only with a single number.
            If the snippet contains any security vulnerabilities, respond with 1. 
            Otherwise, respond with 0"""


    if PROMPT_VARIATION == "chain":

        prompt1 = f"{code} Describe the function of the above C/C++ code snippet."

        prompt2 = f"""Does the code contain security vulnerabilities?
            Respond only with a single number.
            If the snippet contains any security vulnerabilities, respond with 1. 
            Otherwise, respond with 0"""

        completions = client.chat.completions.create(
            model=model,
            messages=[
                {
                    "role": "user",
                    "content": prompt1
                },
                {
                    "role": "user",
                    "content": prompt2
                }
            ]

        )
    else: # Single prompt (not chain)
        completions = client.chat.completions.create(
            model=model,
            messages=[
                {
                    "role": "user",
                    "content": prompt
                }
            ]

        )
    


    out = completions.choices[0].message.content

    return out != '0' # Count unexpected LLM output as vulnerable




client = OpenAI(api_key=API_KEY)

LLM_MODELS = ["gpt-4o-mini", "gpt-4o", "o1-mini"]




# Define directories
DIRNAME = "DiverseVul Sample"

CODE_DIR = Path("Code Examples", DIRNAME)
SECURE_DIR = Path(CODE_DIR,"Secure")
VUL_DIR = Path(CODE_DIR,"Vulnerable")

# Create output directory if it doesn't exist
Path("Detection Outputs").mkdir(exist_ok=True)
Path("Detection Outputs", DIRNAME).mkdir(exist_ok=True)




PROMPT_VARIATION = "" # Prompt Variation (standard, emotive, context, chain) - "" for standard prompt

if PROMPT_VARIATION != "":
    print(f"Testing prompt variation: {PROMPT_VARIATION}")


# Get tool results if using context prompt
if PROMPT_VARIATION == "context":
    full_dict = {}

    for dict_file in Path("Prompt Inputs").iterdir():

        # Skip sub-folders
        if not dict_file.is_file():
            continue

        with open(dict_file, "rb") as file:
            tool_dict = pickle.load(file)

            key = dict_file.name.split(".")[-2]
            key = key.split("=",1)[-1] # Split only first occurrence of '='

            full_dict[key] = list(tool_dict.values())[0]



# Load dataset
file = open("diversevul_2.json")
dataset = json.load(file)




for model in LLM_MODELS:
    start_time = time()

    print(f"Running model: {model}...")
    

    performances = {}
    performances[model] = {}

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
        prediction = 1 if isVulnerable(model, ex) else 0

        # Update dictionary with tool prediction for current example
        performances[model][ex.name] = {}

        performances[model][ex.name]["cwe"] = cwe
        performances[model][ex.name]["vulnerable"] = 1
        performances[model][ex.name]["predicted"] =  prediction






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
        prediction = 1 if isVulnerable(model, ex) else 0

        # Update dictionary with tool prediction for current example
        performances[model][ex.name] = {}

        performances[model][ex.name]["cwe"] = cwe
        performances[model][ex.name]["vulnerable"] = 0
        performances[model][ex.name]["predicted"] =  prediction
   


    # File path: Detection Outputs/DIRNAME/LLMs/PROMPT_VARIATION/file

    if PROMPT_VARIATION == "":
        out_file_name = "output=" + model + ".pickle"
    else: # Add prompt variation to filename 
        out_file_name = "output=" + model + "-" + PROMPT_VARIATION + ".pickle"

    out_root_dir = Path("Detection Outputs", DIRNAME, "LLMs")
    out_root_dir.mkdir(exist_ok=True) # Create folder if it doesn't exist

    if PROMPT_VARIATION == "":
        out_file_dir = Path(out_root_dir, "Standard")
    else:
        out_file_dir = Path(out_root_dir, PROMPT_VARIATION.capitalize())
        out_file_dir.mkdir(exist_ok=True) # Create folder if it doesn't exist

    out_file_path = Path(out_file_dir, out_file_name)


    with open(out_file_path, "wb") as file:
        pickle.dump(performances, file)



    print("Time taken: ", time() - start_time)
