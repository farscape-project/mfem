// Copyright (c) 2010-2022, Lawrence Livermore National Security, LLC. Produced
// at the Lawrence Livermore National Laboratory. All Rights reserved. See files
// LICENSE and NOTICE for details. LLNL-CODE-806117.
//
// This file is part of the MFEM library. For more information and source code
// availability visit https://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the BSD-3 license. We welcome feedback and contributions, see file
// CONTRIBUTING.md for details

#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include "mfem.hpp"

namespace mfem {
namespace materials { 

/// Base class to transform a grid function.
class GFTransformer
{
 public:
  GFTransformer() = default;
  virtual ~GFTransformer() = default;
  virtual void Transform(ParGridFunction &x) const = 0;
};

/// This transformations is a pointwise transformation to 
/// transform a Gaussian random field to a random field following a uniform 
/// distributions. Specifically, we implement the transformations as described 
/// in the following paper:
/// Lazarov, B.S., Schevenels, M. & Sigmund, O. Topology optimization 
/// considering material and geometric uncertainties using stochastic 
/// collocation methods. Struct Multidisc Optim 46, 597–612 (2012). 
/// https://doi.org/10.1007/s00158-012-0791-7
/// Equation (19).
class UniformGRFTransformer : public GFTransformer
{
 public:
  UniformGRFTransformer(double min, double max) : min_(min), max_(max) {}
  virtual ~UniformGRFTransformer() = default;
  /// Transforms a GridFunction representing a Gaussian random field to a 
  /// uniform random field between a and b.
  virtual void Transform(ParGridFunction &x) const override;
 
 private:
  double min_ = 0.0;
  double max_ = 1.0;
};

/// Adds an constant offset to a grid function, i.e. u(x) = u(x) + offset.
class OffsetTransformer : public GFTransformer
{
 public:
  OffsetTransformer(double offset) : offset_(offset) {}
  virtual ~OffsetTransformer() = default;
  /// Offsets a grid function by an constant offset.
  virtual void Transform(ParGridFunction &x) const override;
 
 private:
  double offset_ = 0.0;
};

/// Transforms a grid function by scaling it by a constant factor.
class ScaleTransformer : public GFTransformer
{
 public:
  ScaleTransformer(double scale) : scale_(scale) {}
  virtual ~ScaleTransformer() = default;
  /// Scales a grid function by an constant factor.
  virtual void Transform(ParGridFunction &x) const override;
 
 private:
  double scale_ = 1.0;
};

} // namespace materials  
} // namespace mfem

#endif // TRANSFORMATION_HPP
