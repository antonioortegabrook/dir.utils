cmake_minimum_required(VERSION 3.0)


include(${CMAKE_CURRENT_SOURCE_DIR}/../../script/max-pretarget.cmake)


include_directories(
	"${MAX_INCLUDES}"
	"${MSP_INCLUDES}"
	"${JIT_INCLUDES}"
)


add_library(
	${PROJECT_NAME}
	MODULE
	${PROJECT_NAME}.c
)


include(${CMAKE_CURRENT_SOURCE_DIR}/../../script/max-posttarget.cmake)
