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

list(APPEND SRCS
  auxiliary.cpp
  batched/batched.cpp
  batched/gpu_blas.cpp
  batched/magma.cpp
  batched/native.cpp
  batched/solver.cpp
  blockmatrix.cpp
  blockoperator.cpp
  blockvector.cpp
  complex_densemat.cpp
  complex_operator.cpp
  constraints.cpp
  densemat.cpp
  symmat.cpp
  handle.cpp
  matrix.cpp
  mma.cpp
  ode.cpp
  operator.cpp
  solvers.cpp
  sparsemat.cpp
  sparsesmoothers.cpp
  vector.cpp
  )

list(APPEND HDRS
  auxiliary.hpp
  batched/batched.hpp
  batched/gpu_blas.hpp
  batched/magma.hpp
  batched/native.hpp
  batched/solver.hpp
  blockmatrix.hpp
  blockoperator.hpp
  blockvector.hpp
  complex_densemat.hpp
  complex_operator.hpp
  constraints.hpp
  densemat.hpp
  dinvariants.hpp
  symmat.hpp
  dtensor.hpp
  handle.hpp
  invariants.hpp
  kernels.hpp
  lapack.hpp
  linalg.hpp
  matrix.hpp
  mma.hpp
  ode.hpp
  operator.hpp
  solvers.hpp
  sparsemat.hpp
  sparsesmoothers.hpp
  tlayout.hpp
  tmatrix.hpp
  ttensor.hpp
  tensor.hpp
  dual.hpp
  vector.hpp
  )

if (MFEM_USE_MPI)
  list(APPEND SRCS
    hypre.cpp
    hypre_parcsr.cpp)
  # If this list (HDRS -> HEADERS) is used for install, we probably want the
  # headers added all the time.
  list(APPEND HDRS
    hypre.hpp
    hypre_parcsr.hpp)
  if (MFEM_USE_PETSC)
    list(APPEND SRCS
      petsc.cpp)
    list(APPEND HDRS
      petsc.hpp)
    if (MFEM_USE_SLEPC)
      list(APPEND SRCS
        slepc.cpp)
      list(APPEND HDRS
        slepc.hpp)
    endif()
  endif()
endif()

if (MFEM_USE_SUNDIALS)
  list(APPEND SRCS sundials.cpp)
  list(APPEND HDRS sundials.hpp)
endif()

if (MFEM_USE_SUPERLU)
  list(APPEND SRCS superlu.cpp)
  # If this list (HDRS -> HEADERS) is used for install, we probably want the
  # header added all the time.
  list(APPEND HDRS superlu.hpp)
endif()

if (MFEM_USE_MUMPS)
  list(APPEND SRCS mumps.cpp)
  list(APPEND HDRS mumps.hpp)
endif()

if (MFEM_USE_STRUMPACK)
  list(APPEND SRCS strumpack.cpp)
  list(APPEND HDRS strumpack.hpp)
endif()

if (MFEM_USE_GINKGO)
  list(APPEND SRCS ginkgo.cpp)
  list(APPEND HDRS ginkgo.hpp)
endif()

if (MFEM_USE_AMGX)
  list(APPEND SRCS amgxsolver.cpp)
  list(APPEND HDRS amgxsolver.hpp)
endif()

if (MFEM_USE_HIOP)
  list(APPEND SRCS hiop.cpp)
  list(APPEND HDRS hiop.hpp)
endif()

if (MFEM_USE_MKL_CPARDISO)
  list(APPEND SRCS cpardiso.cpp)
  list(APPEND HDRS cpardiso.hpp)
endif()

if (MFEM_USE_MKL_PARDISO)
  list(APPEND SRCS pardiso.cpp)
  list(APPEND HDRS pardiso.hpp)
endif()

convert_filenames_to_full_paths(SRCS)
convert_filenames_to_full_paths(HDRS)

set(SOURCES ${SOURCES} ${SRCS} PARENT_SCOPE)
set(HEADERS ${HEADERS} ${HDRS} PARENT_SCOPE)
