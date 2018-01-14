include_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/fftw)
include_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/libsndfile)
include_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/libmpg123-1)
include_directories(include)

link_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/fftw)
link_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/libsndfile)
link_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/libmpg123-1)

file(GLOB SOURCE_FILES src/*.cpp)
file(GLOB EXPORT_HEADER_FILES include/*.h)

add_library(${FEATURE_LIB} SHARED
    ${SOURCE_FILES} 
    ${HEADER_FILES}
    ${EXPORT_HEADER_FILES} 
)

if(WIN32)
   target_link_libraries(${FEATURE_LIB} PRIVATE libsndfile-1 libfftw3-3 libmpg123-0)
elseif(UNIX)
   target_link_libraries(${FEATURE_LIB} PRIVATE sndfile fftw3 mpg123)
endif()


include (GenerateExportHeader)
GENERATE_EXPORT_HEADER(${FEATURE_LIB}
             BASE_NAME ${NAME}
             EXPORT_MACRO_NAME FEATURE_API
             EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/${FEATURE_LIB}/include/export.h)
  

