add_library(segment_display INTERFACE)

target_sources(segment_display INTERFACE 
  ${CMAKE_CURRENT_LIST_DIR}/src/segment_display.cpp
)

target_include_directories(segment_display INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(segment_display INTERFACE pico_stdlib hardware_i2c)