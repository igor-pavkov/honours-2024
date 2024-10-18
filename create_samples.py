# Takes folder containing code examples and creates a random sample balancing secure and vulnerable code
# Supports splitting examples into training and test set

import json
from pathlib import Path
import random
import shutil

# Get original folder of examples
source_dir = Path("Code Examples", "DiverseVul Filtered")
source_sec = Path(source_dir, "Secure")
source_vul = Path(source_dir, "Vulnerable")


# Make directories for code samples
train_dir = Path("Code Examples", "DiverseVul Sample")
train_dir.mkdir(exist_ok=True)

train_sec = Path(train_dir,"Secure")
train_sec.mkdir(exist_ok=True)

train_vul = Path(train_dir,"Vulnerable")
train_vul.mkdir(exist_ok=True)

test_dir = Path("Code Examples", "Test")
test_dir.mkdir(exist_ok=True)

test_vul = Path(test_dir,"Secure")
test_vul.mkdir(exist_ok=True)

test_sec = Path(test_dir,"Vulnerable")
test_sec.mkdir(exist_ok=True)


# Read dataset
file = open("diversevul_2.json")
dataset = json.load(file)

# Dictionary containing lists of secure example file names, with CWEs as keys
# Used to randomly select a secure example with a particular CWE
sec_cwes = {} 

# Go through every secure code file
for ex in source_sec.iterdir():
    id = ex.name.split("-")[0]
    
    cwe = dataset[id]["cwe"][0]

    # Set empty list if ex is first instance of CWE
    sec_cwes.setdefault(cwe,[])

    sec_cwes[cwe].append(ex) # append filename to list


# Shuffle lists of secure examples
for cwe in sec_cwes:
    random.shuffle(sec_cwes[cwe])


# Separate vulnerable examples into 80-20 split
vul_list = list(source_vul.iterdir())
random.shuffle(vul_list)
cutoff = round(len(vul_list)*0.8)

vul_train = vul_list[:cutoff]
vul_test = vul_list[cutoff:]


split = 1 # All code in training set
cutoff = len(vul_list)*split
i = 0 # current example index

for ex in vul_list:

    # Split training and test sets
    if i < cutoff:
        vul_dir = train_vul
        sec_dir = train_sec
    else:
        vul_dir = test_vul
        sec_dir = test_sec
    
    # Copy vulnerable example into vulnerable dir
    vulpath = Path(vul_dir, ex.name)
    shutil.copyfile(ex,vulpath)

    
    # Get CWE of vulnerable example
    id = ex.name.split(".")[0]
    cwe = dataset[id]["cwe"][0] 
    
    # Copy random secure example in secure dir

    if cwe in sec_cwes:
        sec_ex = sec_cwes[cwe].pop()
    else: # No secure example for given CWE
        while True: # Keep looping until valid CWE is found
            try:
                rand_cwe = random.choice(list(sec_cwes.keys()))
                sec_ex = sec_cwes[rand_cwe].pop()
                break
            except KeyError:
                continue



    secpath = Path(sec_dir, sec_ex.name)
    shutil.copyfile(sec_ex, secpath)

    i += 1



