# if boolArg is false, cmake will crash with the error
macro(assert boolArg errorMsg)
    if (NOT ${boolArg}) 
        message(FATAL_ERROR "Assertion Failed: ${errorMsg}")
    endif()
endmacro(assert)

# always fails the assert
macro(assertFail errorMsg)
    message(FATAL_ERROR "Assertion Failed: ${errorMsg}")
endmacro(assertFail)