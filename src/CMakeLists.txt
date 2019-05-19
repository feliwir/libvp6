set(HDR_FILES
    ${VP6_INC_DIR}/vp6/context.hpp
    ${VP6_INC_DIR}/vp6/decode.hpp
    ${VP6_INC_DIR}/vp6/frame.hpp
    ${VP6_INC_DIR}/vp6/huffman.hpp
    ${VP6_INC_DIR}/vp6/idct.hpp
    ${VP6_INC_DIR}/vp6/model.hpp
    ${VP6_INC_DIR}/vp6/rangedecoder.hpp
    ${VP6_INC_DIR}/vp6/types.hpp
    dequantize.hpp
    tables.hpp
    util.hpp
    )

set(SRC_FILES
    context.cpp
    decode.cpp
    frame.cpp
    huffman.cpp
    idct.cpp
    model.cpp
    rangedecoder.cpp
    )

add_library(libvp6 ${HDR_FILES} ${SRC_FILES})

target_include_directories(libvp6 PUBLIC ${VP6_INC_DIR})