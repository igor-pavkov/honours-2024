import json
from pathlib import Path
import subprocess as sp

# True if file contains syntax errors, false otherwise
def hasSyntaxErrors(ex):
        args = "cppcheck --enable=all " + '"' + str(ex) + '"'
        # print(args)

        res = sp.run(args, capture_output=True, text=True)

        output = res.stdout + ' ' + res.stderr # concatenate stdout and stderr

        return output.find("syntaxError") == -1


# Make directories for secure and vulnerable for code examples
code_dir = Path("Code Examples", "DiverseVul Filtered")
code_dir.mkdir(exist_ok=True)

sec_dir = Path(code_dir, "Secure")
sec_dir.mkdir(exist_ok=True)

vul_dir = Path(code_dir, "Vulnerable")
vul_dir.mkdir(exist_ok=True)


# Read dataset
file = open("diversevul_2.json")
dataset = json.load(file)



# Other information can be stored by reading original JSON file - ignore CWEs for file creation
# Code examples must contain only the code itself

# Loop through 
for id in dataset.keys():
    # Skip ids that aren't associated with exactly one vulnerability
    if len(dataset[id]["cwe"]) != 1:
        continue

    # Check how many vulnerable functions are part of the commit id
    n_vul = 0
    for ex in dataset[id]["code"]:
        if ex["target"] == 1:
            n_vul += 1
            vul_code = ex["func"] # Store vulnerable code

        if n_vul > 1:
            break

    # Skip ids that don't have exactly one vulnerable function
    if n_vul != 1:
        continue
    

    # Save vulnerable example
    filepath = Path(vul_dir, id + ".cpp")
    
    with open(filepath, 'w', encoding='utf-8') as file:
        file.write(vul_code)
    

    # Save secure code examples to file (Only if commit contains more than one function)


    if len(dataset[id]["code"]) > 1:
        i = 0
        for ex in dataset[id]["code"]:
            if ex["target"] == 1: # Skip vulnerable example (already written to file)
                continue

            filename = id + "-" + str(i) + ".cpp"

            filepath = Path(sec_dir, filename)

            with open(filepath, 'w', encoding='utf-8') as file:
                file.write(ex["func"])

            i += 1



# Need code to be written to files to check for syntax errors with cppcheck

for ex in Path(sec_dir).iterdir():
    if hasSyntaxErrors(ex):
        ex.unlink()

for ex in Path(vul_dir).iterdir():
    if hasSyntaxErrors(ex):
        ex.unlink()