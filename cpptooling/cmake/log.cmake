# cmd color codes
string(ASCII 27 Esc)
set(clitext_Reset       "${Esc}[m")
set(clitext_Bold        "${Esc}[1m")
set(clitext_Red         "${Esc}[31m")
set(clitext_Green       "${Esc}[32m")
set(clitext_Yellow      "${Esc}[33m")
set(clitext_Blue        "${Esc}[34m")
set(clitext_Magenta     "${Esc}[35m")
set(clitext_Cyan        "${Esc}[36m")
set(clitext_White       "${Esc}[37m")
set(clitext_BoldRed     "${Esc}[1;31m")
set(clitext_BoldGreen   "${Esc}[1;32m")
set(clitext_BoldYellow  "${Esc}[1;33m")
set(clitext_BoldBlue    "${Esc}[1;34m")
set(clitext_BoldMagenta "${Esc}[1;35m")
set(clitext_BoldCyan    "${Esc}[1;36m")
set(clitext_BoldWhite   "${Esc}[1;37m")

# Types of message statements
# message(STATUS "message")
# message(FATAL_ERROR "message") # crashes
# message(WARNING "message")
