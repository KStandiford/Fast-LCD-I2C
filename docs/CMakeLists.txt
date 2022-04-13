
# Configure README in project directory for Github, and as the main page
# in the documentation

message(STATUS "Processing docs")
message(STATUS ${PROJECT_SOURCE_DIR})
unset(README_HDR)
configure_file(readme.md.in ${PROJECT_SOURCE_DIR}/README.md)
set(README_HDR "{#mainpage}")
configure_file(readme.md.in ${CMAKE_CURRENT_SOURCE_DIR}/mainpage.md)
message(STATUS "Readme Configured")

# Make sure Doxygen is available
find_package(Doxygen)
if(Doxygen_FOUND)
    # First make two new Doxyfiles which are localized
    set(ARDUINO, "ARDUINO")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in, ${CMAKE_CURRENT_SOURCE_DIR}/DoxyfileA)
    unset(ARDUINO)
    configure_file(Doxyfile.in, ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

else()
    message(STATUS "Doxygen not found!")
endif()

