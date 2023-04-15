cmake_minimum_required(VERSION 3.11)

project(daps_2023 CXX C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/bin" OR NOT IS_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
    message(STATUS "Creating binary directory '${PROJECT_SOURCE_DIR}/bin'")
    file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
endif()

add_library(base_mpi INTERFACE)
target_compile_definitions(base_mpi INTERFACE "-DUSE_MPI")
target_include_directories(base_mpi INTERFACE "/opt/nfs/mpich-4.0.1/include")
target_link_directories(base_mpi INTERFACE "/opt/nfs/mpich-4.0.1/lib")
target_link_libraries(base_mpi INTERFACE m mpi)

add_library(base_sprng INTERFACE)
target_compile_definitions(base_sprng INTERFACE "-DSPRNG_MPI")
target_include_directories(base_sprng INTERFACE "/opt/nfs/sprng5/include")
target_link_directories(base_sprng INTERFACE "/opt/nfs/sprng5/lib")
target_link_libraries(base_sprng INTERFACE base_mpi sprng)

add_library(base_mpe INTERFACE)
target_include_directories(base_mpe INTERFACE "/opt/nfs/mpe2-2.4.9b/include")
target_link_directories(base_mpe INTERFACE "/opt/nfs/mpe2-2.4.9b/lib")
target_link_libraries(base_mpe INTERFACE base_mpi X11)

####################################################################################################

add_executable(lab2_A)
target_sources(lab2_A PRIVATE "${PROJECT_SOURCE_DIR}/lab2/hellompi.c")
target_link_libraries(lab2_A PRIVATE base_mpi)

add_executable(lab2_B)
target_sources(lab2_B PRIVATE "${PROJECT_SOURCE_DIR}/lab2/mpi_test_time.c")
target_link_libraries(lab2_B PRIVATE base_mpi)

####################################################################################################

add_executable(lab3_A)
target_sources(lab3_A PRIVATE "${PROJECT_SOURCE_DIR}/lab3/pi_integral_intro.c")
target_link_libraries(lab3_A PRIVATE base_mpi)

####################################################################################################

add_executable(lab4_A)
target_sources(lab4_A PRIVATE "${PROJECT_SOURCE_DIR}/lab4/pi_max_demo.c")
target_link_libraries(lab4_A PRIVATE base_mpi)

add_executable(lab4_B)
target_sources(lab4_B PRIVATE "${PROJECT_SOURCE_DIR}/lab4/pi_server_demo.c")
target_link_libraries(lab4_B PRIVATE base_mpi)

####################################################################################################

add_executable(lab5_A)
target_sources(lab5_A PRIVATE "${PROJECT_SOURCE_DIR}/lab5/A/sprng_mpi.cpp")
target_link_libraries(lab5_A PRIVATE base_sprng)

add_executable(lab5_B)
target_sources(lab5_B PRIVATE "${PROJECT_SOURCE_DIR}/lab5/B/sprng_2streams_mpi.cpp")
target_link_libraries(lab5_B PRIVATE base_sprng)

add_executable(lab5_D)
target_sources(lab5_D PRIVATE "${PROJECT_SOURCE_DIR}/lab5/D/sprng_message_mpi.cpp")
target_link_libraries(lab5_D PRIVATE base_sprng)

add_executable(lab5_E)
target_sources(lab5_E PRIVATE "${PROJECT_SOURCE_DIR}/lab5/E/mpi_pi_sprng.cpp")
target_link_libraries(lab5_E PRIVATE base_sprng)

####################################################################################################

add_executable(lab6_A)
target_sources(lab6_A PRIVATE "${PROJECT_SOURCE_DIR}/lab6/mpe_pi_X11.c")
target_link_libraries(lab6_A PRIVATE base_mpe)
