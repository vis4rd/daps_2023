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
target_link_libraries(base_mpe INTERFACE base_mpi mpe X11)

add_library(base_omp INTERFACE)
target_compile_options(base_omp INTERFACE "-fopenmp")
target_link_options(base_omp INTERFACE "-fopenmp")
target_compile_definitions(base_omp INTERFACE "-DNUM_THREADS=4")

add_library(base_upc INTERFACE)
target_compile_options(base_upc INTERFACE "-bupc" "-network=udp" "-pthreads=4")
set_target_properties(base_upc PROPERTIES C_COMPILER_LAUNCHER "/opt/nfs/berkeley_upc-2022.10.0/bin/upcc")  # doesnt work

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

####################################################################################################

add_executable(lab7_A)
target_sources(lab7_A PRIVATE "${PROJECT_SOURCE_DIR}/lab7/A/pi_sequential.c")
target_link_libraries(lab7_A PRIVATE base_omp)

add_executable(lab7_B)
target_sources(lab7_B PRIVATE "${PROJECT_SOURCE_DIR}/lab7/B/pi_table.c")
target_link_libraries(lab7_B PRIVATE base_omp)

add_executable(lab7_C)
target_sources(lab7_C PRIVATE "${PROJECT_SOURCE_DIR}/lab7/C/pi_critical.c")
target_link_libraries(lab7_C PRIVATE base_omp)

add_executable(lab7_D)
target_sources(lab7_D PRIVATE "${PROJECT_SOURCE_DIR}/lab7/D/pi_reduction.c")
target_link_libraries(lab7_D PRIVATE base_omp)

add_executable(lab7_E)
target_sources(lab7_E PRIVATE "${PROJECT_SOURCE_DIR}/lab7/E/pi_tasks.c")
target_link_libraries(lab7_E PRIVATE base_omp)

####################################################################################################

add_executable(lab8)
target_sources(lab8 PRIVATE "${PROJECT_SOURCE_DIR}/lab8/mc_pi_mpe.c")
target_link_libraries(lab8 PRIVATE base_mpe)

####################################################################################################

# lab9

####################################################################################################

add_executable(lab10)
target_sources(lab10 PRIVATE "${PROJECT_SOURCE_DIR}/lab10/upc-1-matmul.c")
target_link_libraries(lab10 PRIVATE base_upc)
