/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

#ifndef TRIQS_DEBUG
#define TRIQS_DEBUG
#endif

#include <mpi/mpi.hpp>
#include <triqs/arrays.hpp>
#include <h5/h5.hpp>
#include <triqs/utility/typeid_name.hpp>
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>
#include <nda/gtest_tools.hpp>
#include <nda/clef/adapters/math.hpp>

#if H5_VERSION_GE(1, 8, 9)
#include <h5/serialization.hpp>
#endif

using dcomplex = std::complex<double>;
using triqs::clef::placeholder;

// Check that 'cout << Y' prints X
#define EXPECT_PRINT(X, Y)                                                                                                                           \
  {                                                                                                                                                  \
    std::stringstream ss;                                                                                                                            \
    ss << Y;                                                                                                                                         \
    EXPECT_EQ(X, ss.str());                                                                                                                          \
  }
#define ASSERT_PRINT(X, Y)                                                                                                                           \
  {                                                                                                                                                  \
    std::stringstream ss;                                                                                                                            \
    ss << Y;                                                                                                                                         \
    ASSERT_EQ(X, ss.str());                                                                                                                          \
  }

#define MAKE_MAIN                                                                                                                                    \
  int main(int argc, char **argv) {                                                                                                                  \
    ::mpi::environment env(argc, argv);                                                                                                              \
    ::testing::InitGoogleTest(&argc, argv);                                                                                                          \
    return RUN_ALL_TESTS();                                                                                                                          \
  }

// Arrays are equal

// ------------------  HDF5 --------------------
//
// We serialize to H5, deserialize, compare

template <typename T> T rw_h5(T const &x, std::string filename = "ess", std::string name = "x") {

  namespace h5 = h5;
  T y; // must be default constructible

  {
    h5::file file(filename + ".h5", 'w');
    h5_write(file, name, x);
  }

  {
    h5::file file(filename + ".h5", 'r');
    h5_read(file, name, y);
  }

#if H5_VERSION_GE(1, 8, 9)

//#define TRIQS_TEST_USE_H5_SERIA
#ifdef TRIQS_TEST_USE_H5_SERIA

  std::cerr << "Checking H5 serialization/deserialization of \n " << triqs::utility::demangle(typeid(x).name()) << std::endl;
  auto s  = h5::serialize(x);
  T x2    = h5::deserialize<T>(s);
  auto s2 = h5::serialize(x);
  std::cerr << "Length of serialization string " << first_dim(s) << std::endl;
  EXPECT_EQ_ARRAY(s, s2); // << "Test h5 save, load, save, compare has failed !";
#endif

#endif

  return y;
}
