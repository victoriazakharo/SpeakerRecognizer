include_directories(include)
include_directories(${PROJECT_SOURCE_DIR}/${FEATURE_LIB}/include)

if(WIN32)
   include_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/armadillo/include)
   link_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/armadillo/Debug)
   link_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/armadillo/Release) 
endif()

file(GLOB SOURCE_FILES src/*.cpp)
file(GLOB HEADER_FILES include/*.h)

add_executable(${MODELER_APP} ${SOURCE_FILES} ${HEADER_FILES})

if(WIN32)
   target_link_libraries(${MODELER_APP} ${FEATURE_LIB} armadillo libblas liblapack)
   add_custom_command(TARGET ${MODELER_APP} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/bin-windows $<TARGET_FILE_DIR:${MODELER_APP}>)
elseif(UNIX)
   list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
   find_package (libblas REQUIRED)
   find_package (liblapack REQUIRED)
   find_package (Armadillo REQUIRED)
   find_package (libfftw3 REQUIRED)
   include_directories(${ARMADILLO_INCLUDE_DIRS})
   include_directories(${BLAS_INCLUDE_DIR})
   include_directories(${LAPACK_INCLUDE_DIR})
   include_directories(${LIBFFTW3_INCLUDE_DIR})
   target_link_libraries(${MODELER_APP} ${FEATURE_LIB} ${ARMADILLO_LIBRARIES} ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES} libfftw3.a)
endif()


