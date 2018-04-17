SET ( CLANG_VERSION $ENV{CLANG_VERSION} )

IF (CLANG_VERSION)
   MESSAGE (STATUS "Formating with clang ALL source in ${CMAKE_CURRENT_SOURCE_DIR}")
  file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.cc *.h *.hh)
  execute_process(COMMAND clang-format -style=file -i ${ALL_SOURCE_FILES}
		WORKING_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR})

ENDIF()
		