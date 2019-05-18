set(HDR_FILES 
    demuxer.h
    util.h
    )

set(SRC_FILES 
    decoder.cpp
    demuxer.cpp
    )

add_executable(vp6_decoder ${HDR_FILES} ${SRC_FILES})

target_include_directories(vp6_decoder PRIVATE
                    ${VP6_ROOT_DIR}/contrib/cxxopts)
      
target_link_libraries(vp6_decoder libvp6)