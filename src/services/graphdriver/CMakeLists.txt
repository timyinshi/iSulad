# get current directory sources files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} local_graphdriver_srcs)
add_subdirectory(overlay2)
add_subdirectory(devmapper)

set(GRAPHDRIVER_SRCS
    ${local_graphdriver_srcs}
    ${OVERLAY2_SRCS}
    ${DEVMAPPER_SRCS}
    PARENT_SCOPE
    )
set(GRAPHDRIVER_INCS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay2
    ${CMAKE_CURRENT_SOURCE_DIR}/devmapper
    PARENT_SCOPE
    )
