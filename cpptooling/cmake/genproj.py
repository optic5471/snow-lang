
# Copyright (c) 2025 Andrew Griffin - All Rights Reserved

import os, shutil, argparse, subprocess
import pyutils

rootDir = os.path.normpath(os.path.join(os.path.dirname(__file__), ".."))
buildDir = os.path.join(rootDir, "build")
presetDir = os.path.join(buildDir, "win32")
logFilePath = pyutils.makeLogFile(buildDir)

parser = argparse.ArgumentParser(description="Sets up the build process and starts the cmake generation")
parser.add_argument("--clean", help="Deletes the build folder for this preset and reconstructs it from scratch", required=False, action="store_true")
#parser.add_argument("--notests", help="Disables tests", required=False, action="store_true")
parser.add_argument("--generator", help="Cmake generator to use, defaults to VS2022", required=False, default="\"Visual Studio 17 2022\"")
args = parser.parse_args()

# clean
if (args.clean):
    if (os.path.exists(buildDir)):
        pyutils.log(logFilePath, f"Cleaning build directory '{presetDir}'")
        shutil.rmtree(presetDir)

# create build dir if needed
if (not os.path.exists(presetDir)):
    os.makedirs(presetDir)
    pyutils.log(logFilePath, f"Build directory created at '{presetDir}'")

testOnOff = "ON"
#if (args.notests):
#    testOnOff = "OFF"

# configure cmake run
cmakeCommand = f"cmake -B {presetDir} -G {args.generator} -DLOG_FILE_PATH={logFilePath} -DTEST_ENABLED={testOnOff}"
pyutils.log(logFilePath, f"Running cmake command: '{pyutils.clitext.BRIGHT_YELLOW}{cmakeCommand}{pyutils.clitext.DEFAULT_COLOR}'")
with subprocess.Popen(cmakeCommand, cwd=rootDir, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, bufsize=1, universal_newlines=True) as process:
        for line in process.stdout:
            pyutils.log(logFilePath, f"CMAKE: {pyutils.tryRemoveNewLine(line)}")
        for line in process.stderr:
            if (line.find("warning")):
                pyutils.log(logFilePath, f"CMAKE WARN: {pyutils.clitext.BRIGHT_YELLOW}{pyutils.tryRemoveNewLine(line)}{pyutils.clitext.DEFAULT_COLOR}")
            else:
                pyutils.log(logFilePath, f"CMAKE ERR: {pyutils.clitext.BRIGHT_RED}{pyutils.tryRemoveNewLine(line)}{pyutils.clitext.DEFAULT_COLOR}")

if (process.returncode != 0):
    pyutils.abort(logFilePath, "CMake has encounted an error, build failed!")
pyutils.log(logFilePath, f"{pyutils.clitext.BRIGHT_GREEN}{pyutils.clitext.BOLD_ON}Project generation successfully completed!{pyutils.clitext.BOLD_OFF}{pyutils.clitext.DEFAULT_COLOR}")