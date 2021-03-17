set(HDR_FILES
    ${VP6_INC_DIR}/vp6/context.hpp
    ${VP6_INC_DIR}/vp6/decode.hpp
    ${VP6_INC_DIR}/vp6/frame.hpp
    ${VP6_INC_DIR}/vp6/huffman.hpp
    ${VP6_INC_DIR}/vp6/model.hpp
    ${VP6_INC_DIR}/vp6/rangedecoder.hpp
    ${VP6_INC_DIR}/vp6/types.hpp
    ${VP6_INC_DIR}/vp6/dsp/idct.hpp
    dequantize.hpp
    tables.hpp
    util.hpp
    )

set(SRC_FILES
    context.cpp
    decode.cpp
    frame.cpp
    huffman.cpp
    model.cpp
    rangedecoder.cpp
    dsp/idct.cpp
    )

add_library(libvp6 ${HDR_FILES} ${SRC_FILES})

target_include_directories(libvp6 PUBLIC ${VP6_INC_DIR})