function(target_source_group target rootDirOfTarget)
    # get the binary directory (not sure why)
    get_target_property(targetBinaryDir ${target} BINARY_DIR)
    # get the source directory (not ure why)
    get_target_property(targetSourceDir ${target} SOURCE_DIR)
    # get the sources of the project (files that have been added through target_sources)
    get_target_property(targetSources ${target} SOURCES)

    # find files in a folder or in root folder (set differently later)
    foreach(file ${targetSources})
        file(TO_CMAKE_PATH ${file} resultFile)
        get_filename_component(name ${resultFile} NAME)
        get_filename_component(path ${name} ABSOLUTE)

        if(IS_ABSOLUTE ${file})
            # only safe to call RELATIVE_PATH if path begins at rootDirOfTarget
            string(FIND "${file}" "${targetSourceDir}" relativeToSourceDir)
            string(FIND "${file}" "${targetBinaryDir}" relativeToBinaryDir)

            if ("${relativeToSourceDir}" EQUAL 0)
                file(RELATIVE_PATH relativePath ${targetSourceDir} ${file})
            elseif("${relativeToBinaryDir}" EQUAL 0)
                file(RELATIVE_PATH relativePath ${targetBinaryDir} ${file})
                set(path ${targetBinaryDir}/${name})
            endif()
        else()
            set(relativePath ${file})
        endif()

        if (EXISTS ${path})
            set(filesNotInFolder ${filesNotInFolder} ${relativePath})
        else()
            set(filesInFolder ${filesInFolder} ${relativePath})
        endif()
    endforeach()

    # files in a folder can be added directly
    source_group(TREE ${rootDirOfTarget} PREFIX "" FILES ${filesInFolder})
    # files not in a folder need a ' ' prefix to put them in root folder instead of "Source Files" or "Header Files" filters of VS
    source_group(TREE ${rootDirOfTarget} PREFIX " " FILES ${filesNotInFolder})
endfunction(target_source_group)
