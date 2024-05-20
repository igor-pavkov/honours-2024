import subprocess as sp
import sys, logging
from pathlib import Path


# switch to argparse
if len(sys.argv) == 1:
    languages = ["c","python"]
else:
    languages = sys.argv[1:]


# Dictionary of tool names with languages as keys
tools = {}
tools["c"] = ["flawfinder","cppcheck"]
tools["python"] = ["pylint","bandit"]
tools["cpp"] = ["flawfinder", "cppcheck"]


# Dictionary of tool arguments with tools as keys
tool_args = {}
tool_args["cppcheck"] = ["--enable=all"]

# Dictionary of output search terms with tools as keys
output_terms = {}
output_terms["flawfinder"] = [""]

# Set up logging for simultaneous output to stdout and file
targets = logging.StreamHandler(sys.stdout), logging.FileHandler("output.log", mode="w")
logging.basicConfig(format="%(message)s",level=logging.INFO, handlers=targets)


# Dictionary with file name as key
# Each value contains another dictionary with vulnerability type, language and whether the vulnerability was detected or not
results = {}

for language in languages:

    code_dir = Path("Vulnerable Code",language)

    logging.info("\n" + "="*100)
    logging.info(f"Language: {language}".center(100))
    logging.info("="*100 + "\n")

    for file in code_dir.iterdir():
        results[file] = {
            "language": language,
            # "vulnerability" = 
            "detected": True
        }


        for tool in tools[language]:

            args = [tool] + tool_args.get(tool,[]) + [file]

            logging.info("\n" + "-"*100)
            logging.info(f"{tool} output for {file.name}".center(100))
            logging.info("-"*100 + "\n")

            res = sp.run(args, capture_output=True, text=True)

            out = res.stdout + res.stderr # concatenate stdout and stderr


            # Check that tool found a vulnerability
            found = True
            for searchstr in output_terms[tool]:

                if out.find(searchstr) == -1: # If output doesn't contain expected term
                    found = False
                    break

            
            # Check that tool found correct vulnerability



            # Correct vulnerability detected
            if found:
               results[file]["detected"] = True 




