set(HDR_FILES 
    demuxer.hpp
    util.hpp
    )

set(SRC_FILES 
    decoder.cpp
    demuxer.cpp
    )

add_executable(vp6_decoder ${HDR_FILES} ${SRC_FILES})

target_include_directories(vp6_decoder PRIVATE
                    ${VP6_ROOT_DIR}/contrib/stb
                    ${VP6_ROOT_DIR}/contrib/cxxopts/include)
      
target_link_libraries(vp6_decoder libvp6)