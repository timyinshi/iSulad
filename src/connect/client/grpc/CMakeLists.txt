if (ENABLE_OCI_IMAGE)
    set(ISUAL_IMAGE_GRPC_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/grpc_isula_image_client.cc
        PARENT_SCOPE
        )
endif()

if (GRPC_CONNECTOR)
    set(CLIENT_GRPC_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/grpc_client.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/grpc_containers_client.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/grpc_images_client.cc
        PARENT_SCOPE
        )
endif()
