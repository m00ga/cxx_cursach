project(cursach)

set (CMAKE_CXX_STANDARD 14)

# include_directories(
#     ${PROJECT_SOURCE_DIR}/include
# )

add_executable(cursach main.cpp linkedList.hpp utils.hpp table.hpp)
