import subprocess as sp
import sys, logging
from pathlib import Path


# switch to argparse
if len(sys.argv) == 1:
    languages = ["C","Python"]
else:
    languages = sys.argv[1:]


tools = {}
tools["c"] = ["flawfinder","cppcheck"]
tools["python"] = ["pylint","bandit"]
tools["cpp"] = ["flawfinder", "cppcheck"]

tool_args = {}
tool_args["cppcheck"] = ["--enable=all"]

# Set up logging for simultaneous output to stdout and file
targets = logging.StreamHandler(sys.stdout), logging.FileHandler("output.log", mode="w")
logging.basicConfig(format="%(message)s",level=logging.INFO, handlers=targets)

for language in languages:

    code_dir = Path("Vulnerable Code",language)

    logging.info("\n" + "="*100)
    logging.info(f"Language: {language}".center(100))
    logging.info("="*100 + "\n")

    for file in code_dir.iterdir():

        for tool in tools[language]:

            args = [tool] + tool_args.get(tool,[]) + [file]

            logging.info("\n" + "-"*100)
            logging.info(f"{tool} output for {file.name}".center(100))
            logging.info("-"*100 + "\n")

            res = sp.run(args, capture_output=True, text=True)

            logging.info(res.stdout)
            logging.info(res.stderr)


