cmake_minimum_required(VERSION 2.8)
set(NAME SpeakerRecognizer)
set(RECOGNIZER_APP SpeakerRecognizer)
set(MODELER_APP SpeakerModeler)
set(FEATURE_LIB FeatureExtractor)
set(TEST_APP Test)
set(BINARY_DIR ${CMAKE_BINARY_DIR}/bin)
project(${NAME})

if(WIN32)
    set(LIB_FOLDER Lib-windows)       
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER CMake)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Debug;Release")
elseif(UNIX)
    set(LIB_FOLDER Lib-linux)               
    add_definitions(-std=c++11)
endif()

add_definitions(-DARMA_EXTRA_MAT_PROTO=${CMAKE_SOURCE_DIR}/${LIB_FOLDER}/arma-extend/Mat_extra_bones.hpp)
add_definitions(-DARMA_EXTRA_MAT_MEAT=${CMAKE_SOURCE_DIR}/${LIB_FOLDER}/arma-extend/Mat_extra_meat.hpp)

add_subdirectory(${FEATURE_LIB})
add_subdirectory(${MODELER_APP})
add_subdirectory(${RECOGNIZER_APP}) 
add_subdirectory(${TEST_APP}) 

set_target_properties(${FEATURE_LIB} ${MODELER_APP} ${RECOGNIZER_APP} ${TEST_APP}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR}
)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set_target_properties(${FEATURE_LIB} ${MODELER_APP} ${RECOGNIZER_APP} ${TEST_APP} 
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_DIR}
        RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_DIR}
    )
endforeach()
