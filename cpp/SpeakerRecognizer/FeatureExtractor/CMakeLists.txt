include_directories(include)

file(GLOB SOURCE_FILES src/*.cpp)
file(GLOB EXPORT_HEADER_FILES include/*.h)

if(WIN32)  
   include_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/fftw)
   include_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/libsndfile)
   include_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/libmpg123-1)
   link_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/libsndfile)
   link_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/libmpg123-1)
   link_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/fftw)
   add_library(${FEATURE_LIB} SHARED
        ${SOURCE_FILES} 
        ${HEADER_FILES}
        ${EXPORT_HEADER_FILES} 
    )
   target_link_libraries(${FEATURE_LIB} PRIVATE libsndfile-1 libfftw3-3 libmpg123-0)
elseif(UNIX)
   add_library(${FEATURE_LIB} STATIC
        ${SOURCE_FILES} 
        ${HEADER_FILES}
        ${EXPORT_HEADER_FILES} 
    )
   list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
   find_package (libsndfile REQUIRED)
   find_package (libmpg123 REQUIRED)
   find_package (libfftw3 REQUIRED)
   include_directories(${SNDFILE_INCLUDE_DIRS})
   include_directories(${MPG123_INCLUDE_DIR})
   include_directories(${LIBFFTW3_INCLUDE_DIR})
   target_link_libraries(${FEATURE_LIB} PRIVATE ${SNDFILE_LIBRARIES} ${FFTW3_LIBRARY} ${MPG123_LIBRARY})
endif()

include (GenerateExportHeader)
GENERATE_EXPORT_HEADER(${FEATURE_LIB}
             BASE_NAME ${NAME}
             EXPORT_MACRO_NAME FEATURE_API
             EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/${FEATURE_LIB}/include/export.h)
  

