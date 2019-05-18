set(HDR_FILES
    ${VP6_INC_DIR}/vp6/context.h
    ${VP6_INC_DIR}/vp6/decode.h
    ${VP6_INC_DIR}/vp6/frame.h
    ${VP6_INC_DIR}/vp6/huffman.h
    ${VP6_INC_DIR}/vp6/idct.h
    ${VP6_INC_DIR}/vp6/model.h
    ${VP6_INC_DIR}/vp6/rangedecoder.h
    ${VP6_INC_DIR}/vp6/types.h
    dequantize.h
    tables.h
    util.h
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