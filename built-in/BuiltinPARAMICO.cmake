#requires https://github.com/libffi/libffi to build
if ( NOT EXISTS ${TOP_CMAKE_SOURCE_DIR}/built-in/paramiko )
    message("Downloading paramiko source")

    execute_process(COMMAND rm -rf ${TOP_CMAKE_SOURCE_DIR}/built-in/paramiko
		    WORKING_DIRECTORY  ${TOP_CMAKE_SOURCE_DIR}/built-in)
    
    execute_process(COMMAND git clone https://github.com/paramiko/paramiko.git ${TOP_CMAKE_SOURCE_DIR}/built-in/paramiko
                    WORKING_DIRECTORY  ${TOP_CMAKE_SOURCE_DIR}/built-in)

    execute_process(COMMAND  git checkout tags/v1.18.1 
		    WORKING_DIRECTORY  ${TOP_CMAKE_SOURCE_DIR}/built-in/paramiko)		    

    execute_process(COMMAND pip install --prefix ${TOP_CMAKE_SOURCE_DIR}/built-in/paramiko/build/ paramiko
		    WORKING_DIRECTORY  ${TOP_CMAKE_SOURCE_DIR}/built-in/paramiko/)
endif()

