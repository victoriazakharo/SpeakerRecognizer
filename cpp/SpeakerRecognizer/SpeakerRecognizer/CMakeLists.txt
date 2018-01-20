include_directories(include)
include_directories(${PROJECT_SOURCE_DIR}/${FEATURE_LIB}/include)
include_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/armadillo/include)
include_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/essentia/include)

if(WIN32)
   link_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/armadillo/Debug)
   link_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/armadillo/Release)   
elseif(UNIX)
   link_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/armadillo) 
   link_directories(${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/essentia) 
endif()

file(GLOB SOURCE_FILES src/*.cpp)
file(GLOB HEADER_FILES include/*.h)

add_executable(${RECOGNIZER_APP} ${SOURCE_FILES} ${HEADER_FILES})

if(WIN32)
   target_link_libraries(${RECOGNIZER_APP} ${FEATURE_LIB} armadillo libblas liblapack)
elseif(UNIX)
   target_link_libraries(${RECOGNIZER_APP} ${FEATURE_LIB} armadillo blas lapack essentia fftw3 yaml avcodec avformat avutil avresample samplerate tag fftw3f)
endif()




