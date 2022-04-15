
# Configure README in project directory for Github, and as the main page
# in the documentation

message(STATUS "Processing docs")
message(STATUS ${PROJECT_SOURCE_DIR})
unset(README_HDR)
set(em "*")
unset(ex)
configure_file(readme.md.in ${PROJECT_SOURCE_DIR}/README.md @ONLY)
set(README_HDR "{#mainpage}")
unset(em)
set(ex "%")
configure_file(readme.md.in ${CMAKE_CURRENT_SOURCE_DIR}/mainpage.md @ONLY)
message(STATUS "Readme Configured")

# Make sure Doxygen is available

find_package(Doxygen)
if(Doxygen_FOUND)
    # First make two new Doxyfiles which are localized
    set(ARDUINO_DOXY "ARDUINO")
    configure_file(Doxyfile.in ${CMAKE_CURRENT_SOURCE_DIR}/DoxyfileA)
    unset(ARDUINO_DOXY)
    configure_file(Doxyfile.in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    # get the dependencies
    file(GLOB DEPENDS_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.hpp"  "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.c" 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.in" "${CMAKE_CURRENT_SOURCE_DIR}/*.xml" )
    #message(STATUS "${DEPENDS_LIST}")
    add_custom_command( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/html/group___arduino__diff.html
        DEPENDS ${DEPENDS_LIST}    
        COMMAND Doxygen::doxygen ${CMAKE_CURRENT_SOURCE_DIR}/DoxyfileA
        COMMAND Doxygen::doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        COMMENT "Running Doxygen for Arduino Docs"
        VERBATUM
        )

    add_custom_target(dummy ALL  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/html/group___arduino__diff.html
        COMMENT "Building Dummy")



else()
    message(STATUS "Doxygen not found!")
endif()

