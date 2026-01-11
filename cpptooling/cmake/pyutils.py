
# Copyright (c) 2025 Andrew Griffin - All Rights Reserved

import sys, re, os
from datetime import date, datetime

# ANSI escape codes for common formatting in command prompts
if (sys.platform == "win32"):
    class clitext:
        BOLD_ON = "\033[1m"
        BOLD_OFF = "\033[22m"
        DEFAULT_FONT = "\033[10m"
        DEFAULT_COLOR = "\033[0m"
        RED = "\033[31m"
        GREEN = "\033[32m"
        YELLOW = "\033[33m"
        BRIGHT_RED = "\033[91m"
        BRIGHT_GREEN = "\033[92m"
        BRIGHT_YELLOW = "\033[93m"
        WHITE = "\033[97m"
else:
    class clitext:
        BOLD_ON = "\u001b[1m"
        BOLD_OFF = "\u001b[22m"
        DEFAULT_FONT = "\u001b[10m"
        DEFAULT_COLOR = "\u001b[0m"
        RED = "\u001b[31m"
        GREEN = "\u001b[32m"
        YELLOW = "\u001b[33m"
        BRIGHT_RED = "\u001b[91m"
        BRIGHT_GREEN = "\u001b[92m"
        BRIGHT_YELLOW = "\u001b[93m"
        WHITE = "\u001b[97m"

# Removes all ansi codes from the message
def removeAnsiCodes(message):
    newMessage = re.sub(r'\x1b\[[0-9m;]+m?', '', message)
    newMessage = re.sub(r'\u001b\[[0-9m;]+m?', '', newMessage)
    return re.sub(r'\033\[[0-9m;]+m?', '', newMessage)

# Removes ending new line from string, does not remove internal new lines
def tryRemoveNewLine(message):
    return message[:-1] if (message.endswith('\n')) else message

# logs the message
def log(logFilePath, message):
    print(message, flush=True)
    with open(logFilePath, "a") as file:
        file.write(removeAnsiCodes(message + "\n"))

# aborts the program and logs the abort to the file
def abort(logFilePath, message):
    log(logFilePath, clitext.BOLD_ON + clitext.BRIGHT_RED + "ABORT: " + message + clitext.BOLD_OFF + clitext.DEFAULT_COLOR)
    sys.exit(1)

# aborts if the condition is true
def abortTrue(logFilePath, condition, message):
    if (condition == True):
        abort(logFilePath, message)

# aborts if the condition is false
def abortFalse(logFilePath, condition, message):
    if (condition == False):
        abort(logFilePath, message)

# Creates a new log file under ./{folder}/build_logs/
def makeLogFile(folder):
    currentTime = datetime.now().strftime("%b-%d-%Y_%H-%M-%S")
    logFileFolder = os.path.join(folder, "build_logs")
    logFile = "buildlog_" + currentTime + ".txt"
    logFileFullPath = os.path.join(logFileFolder, logFile)
    if (not os.path.exists(logFileFolder)):
        if (not os.path.exists(folder)):
            os.mkdir(folder)
        os.mkdir(logFileFolder)
    file = open(logFileFullPath, "w")
    file.close()
    return logFileFullPath
