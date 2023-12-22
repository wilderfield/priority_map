# Add test cpp file
add_executable(priority_map_test priority_map_tests.cpp)

target_include_directories(priority_map_test PRIVATE ${Catch2_SOURCE_DIR}/single_include)

# Link to Catch2
target_link_libraries(priority_map_test Catch2::Catch2)

# Include Catch2 testing facilities
include(CTest)
include(Catch)
catch_discover_tests(priority_map_test)
