# Copyright (c) 2010-2025, Lawrence Livermore National Security, LLC. Produced
# at the Lawrence Livermore National Laboratory. All Rights reserved. See files
# LICENSE and NOTICE for details. LLNL-CODE-806117.
#
# This file is part of the MFEM library. For more information and source code
# availability visit https://mfem.org.
#
# MFEM is free software; you can redistribute it and/or modify it under the
# terms of the BSD-3 license. We welcome feedback and contributions, see file
# CONTRIBUTING.md for details.

set(PETSC_EXAMPLES_SRCS "")
set(PETSC_RC_FILES "")

if (MFEM_USE_MPI)
  list(APPEND PETSC_EXAMPLES_SRCS
    ex1p.cpp
    ex2p.cpp
    ex3p.cpp
    ex4p.cpp
    ex5p.cpp
    ex6p.cpp
    ex9p.cpp
    ex10p.cpp
    )
  list(APPEND PETSC_RC_FILES
    rc_ex1p rc_ex1p_device rc_ex1p_deviceamg
    rc_ex2p rc_ex2p_bddc rc_ex2p_asm
    rc_ex3p rc_ex3p_bddc
    rc_ex4p rc_ex4p_bddc
    rc_ex5p_bddc rc_ex5p_fieldsplit
    rc_ex9p_expl rc_ex9p_expl_device rc_ex9p_impl
    rc_ex10p rc_ex10p_mf rc_ex10p_mfop rc_ex10p_jfnk
    )
endif()

if (MFEM_USE_SLEPC)
  list(APPEND PETSC_EXAMPLES_SRCS
    ex11p.cpp
    )
  list(APPEND PETSC_RC_FILES
    rc_ex11p_lobpcg rc_ex11p_lobpcg_device rc_ex11p_gd
    )
endif()

# Include the source directory where mfem.hpp and mfem-performance.hpp are.
include_directories(BEFORE ${PROJECT_BINARY_DIR})

# Add targets to copy rc_* files from the source directory
foreach(RC_FILE ${PETSC_RC_FILES})
  add_custom_command(OUTPUT ${RC_FILE}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/${RC_FILE} ${RC_FILE}
    COMMENT "copy ${RC_FILE}")
endforeach()
add_custom_target(copy_petsc_rc_files DEPENDS ${PETSC_RC_FILES}
  COMMENT "Copying PETSC example rc files ...")

# Add "test_petsc" target, see below.
add_custom_target(test_petsc
  ${CMAKE_CTEST_COMMAND} -R petsc USES_TERMINAL)

# Add one executable per cpp file, adding "petsc_" as prefix. Sets
# "copy_petsc_rc_files" as a prerequisite for the given examples. Also, sets
# "test_petsc" as a target that depends on these examples.
set(PFX petsc_)
add_mfem_examples(PETSC_EXAMPLES_SRCS ${PFX} copy_petsc_rc_files test_petsc)

# Testing.
# The PETSc tests can be run separately using the target "test_petsc" which
# builds the examples and runs:
#   ctest -R petsc

# Command line options for the tests.
set(EX1_ARGS_W       -m ../../data/amr-quad.mesh --usepetsc)
set(EX1_ARGS_P       -m ../../data/amr-quad.mesh --usepetsc --petscopts rc_ex1p)
set(EX1_ARGS_CUDA    -m ../../data/star.mesh --usepetsc --partial-assembly --device cuda --petscopts rc_ex1p_device)
set(EX1_ARGS_CUDAAMG -m ../../data/star.mesh --usepetsc --device cuda --petscopts rc_ex1p_deviceamg)
set(EX1_ARGS_HIP     -m ../../data/star.mesh --usepetsc --partial-assembly --device hip --petscopts rc_ex1p_device)
set(EX1_ARGS_HIPAMG  -m ../../data/star.mesh --usepetsc --device hip --petscopts rc_ex1p_deviceamg)
set(EX2_ARGS         -m ../../data/beam-quad.mesh --usepetsc --petscopts rc_ex2p)
set(EX2_ARGS_BDDC    -m ../../data/beam-tri.mesh --usepetsc --nonoverlapping --petscopts rc_ex2p_bddc)
set(EX2_ARGS_ASM     -m ../../data/beam-quad.mesh --usepetsc --petscopts rc_ex2p_asm)
set(EX3_ARGS_BDDC_2D -m ../../data/klein-bottle.mesh -o 2 -f 0.1 --usepetsc --petscopts rc_ex3p_bddc --nonoverlapping)
set(EX3_ARGS_BDDC_3D -m ../../data/amr-hex.mesh -rs 1 -rp 0 -o 2 -f 0.1 --usepetsc --petscopts rc_ex3p_bddc --nonoverlapping)
set(EX4_ARGS         -m ../../data/klein-bottle.mesh -o 2 --usepetsc --petscopts rc_ex4p_bddc --nonoverlapping)
set(EX4_HYB_ARGS     -m ../../data/klein-bottle.mesh -o 2 --usepetsc --petscopts rc_ex4p_bddc --nonoverlapping --hybridization)
set(EX5_BDDC_LB_ARGS -m ../../data/star.mesh --usepetsc -o 0 --petscopts rc_ex5p_bddc --nonoverlapping --local-bdr)
set(EX5_BDDC_GB_ARGS -m ../../data/star.mesh --usepetsc -o 0 --petscopts rc_ex5p_bddc --nonoverlapping)
set(EX5_FSPL_ARGS    -m ../../data/beam-tet.mesh --usepetsc -o 0 --petscopts rc_ex5p_fieldsplit)
set(EX6_ARGS         -m ../../data/amr-quad.mesh --usepetsc)
set(EX6_NONOVL_ARGS  -m ../../data/amr-quad.mesh --usepetsc --nonoverlapping)
set(EX9_E_ARGS       -m ../../data/periodic-hexagon.mesh --usepetsc --petscopts rc_ex9p_expl -dt 0.1)
set(EX9_ES_ARGS      -m ../../data/periodic-hexagon.mesh --usepetsc --petscopts rc_ex9p_expl --no-step)
set(EX9_ES_ARGS_CUDA -m ../../data/periodic-hexagon.mesh --usepetsc --petscopts rc_ex9p_expl_device --no-step --partial-assembly --device cuda)
set(EX9_ES_ARGS_HIP  -m ../../data/periodic-hexagon.mesh --usepetsc --petscopts rc_ex9p_expl_device --no-step --partial-assembly --device hip)
set(EX9_IS_ARGS      -m ../../data/periodic-hexagon.mesh --usepetsc --petscopts rc_ex9p_impl --implicit -tf 0.5)
set(EX10_ARGS        -m ../../data/beam-quad.mesh --usepetsc --petscopts rc_ex10p -tf 30 -s 3 -rs 2 -dt 3)
set(EX10_MF_ARGS     -m ../../data/beam-quad.mesh --usepetsc --petscopts rc_ex10p_mf -tf 6 -s 3 -rs 0 -dt 3)
set(EX10_MFOP_ARGS   -m ../../data/beam-quad.mesh --usepetsc --petscopts rc_ex10p_mfop -tf 6 -s 3 -rs 0 -dt 3)
set(EX10_JFNK_ARGS   -m ../../data/beam-quad.mesh --usepetsc --petscopts rc_ex10p_jfnk --jfnk -tf 6 -s 3 -rs 0 -dt 3)
if (MFEM_USE_SLEPC)
  set(EX11_ARGS_SINV   -m ../../data/star.mesh --useslepc)
  set(EX11_ARGS_LOBPCG -m ../../data/star.mesh --useslepc --slepcopts rc_ex11p_lobpcg)
  set(EX11_ARGS_LOBPCG_CUDA -m ../../data/star.mesh --useslepc --slepcopts rc_ex11p_lobpcg_device --device cuda)
  set(EX11_ARGS_LOBPCG_HIP  -m ../../data/star.mesh --useslepc --slepcopts rc_ex11p_lobpcg_device --device hip)
  set(EX11_ARGS_GD -m ../../data/star.mesh --useslepc --slepcopts rc_ex11p_gd)
endif()

# Add the tests: one test per command-line-variable.
if (MFEM_ENABLE_TESTING)
  set(TEST_OPTIONS_VARS
    EX1_ARGS_W EX1_ARGS_P EX2_ARGS EX2_ARGS_BDDC EX2_ARGS_ASM EX3_ARGS_BDDC_2D
    EX3_ARGS_BDDC_3D EX4_ARGS EX4_HYB_ARGS EX5_BDDC_LB_ARGS EX5_BDDC_GB_ARGS
    EX5_FSPL_ARGS EX6_ARGS EX6_NONOVL_ARGS EX9_E_ARGS EX9_ES_ARGS EX9_IS_ARGS
    EX10_ARGS EX10_MF_ARGS EX10_MFOP_ARGS EX10_JFNK_ARGS)
  if (MFEM_USE_SLEPC)
    list(APPEND TEST_OPTIONS_VARS
      EX11_ARGS_SINV EX11_ARGS_LOBPCG EX11_ARGS_GD)
  endif()
  # CUDA/HIP tests
  if (MFEM_USE_CUDA)
    list(APPEND TEST_OPTIONS_VARS
      EX1_ARGS_CUDA EX1_ARGS_CUDAAMG EX9_ES_ARGS_CUDA)
    if (MFEM_USE_SLEPC)
      list(APPEND TEST_OPTIONS_VARS EX11_ARGS_LOBPCG_CUDA)
    endif()
  elseif (MFEM_USE_HIP)
    list(APPEND TEST_OPTIONS_VARS
      EX1_ARGS_HIP EX1_ARGS_HIPAMG EX9_ES_ARGS_HIP)
    if (MFEM_USE_SLEPC)
      # SLEPc does not support BVSVEC with HIP
      # list(APPEND TEST_OPTIONS_VARS EX11_ARGS_LOBPCG_HIP)
    endif()
  endif()

  foreach(TEST_OPTIONS_VAR ${TEST_OPTIONS_VARS})
    string(REGEX REPLACE "^(.+)_ARGS" "\\1" TEST_NAME_UC ${TEST_OPTIONS_VAR})
    string(REGEX REPLACE "^([^_]+)" "\\1P" TEST_NAME_UC ${TEST_NAME_UC})
    string(TOLOWER ${TEST_NAME_UC} TEST_NAME_FULL)
    string(REGEX REPLACE "^([^_]+).*" "\\1" TEST_NAME ${TEST_NAME_FULL})
    set(TEST_NAME_FULL ${PFX}${TEST_NAME_FULL})
    set(TEST_NAME ${PFX}${TEST_NAME})
    set(TEST_OPTIONS "-no-vis" ${${TEST_OPTIONS_VAR}})
    # message(STATUS "${TEST_NAME_FULL} --> ${TEST_NAME} ${TEST_OPTIONS}")

    # All PETSC tests are parallel.
    if (MFEM_USE_MPI)
      add_test(NAME ${TEST_NAME_FULL}_np=${MFEM_MPI_NP}
        COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MFEM_MPI_NP}
        ${MPIEXEC_PREFLAGS}
        $<TARGET_FILE:${TEST_NAME}> ${TEST_OPTIONS}
        ${MPIEXEC_POSTFLAGS})
    endif()
  endforeach()
endif()
