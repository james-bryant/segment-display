add_library(segment_display INTERFACE)

target_sources(segment_display INTERFACE 
  ${CMAKE_CURRENT_LIST_DIR}/src/segment_display.c
)

target_include_directories(segment_display INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(segment_display INTERFACE i2c_bus_scan pico_stdlib hardware_i2c)