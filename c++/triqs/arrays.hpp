/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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

// for python code generator, we need to know what to include...
#define TRIQS_INCLUDED_ARRAYS

#include <assert.h>

#include <complex>
#include <h5/h5.hpp>
#include <nda/nda.hpp>
#include <nda/h5.hpp>
#include <nda/mpi.hpp>
#include <nda/blas.hpp>
#include <nda/lapack.hpp>
#include <nda/linalg/det_and_inverse.hpp>
#include <nda/clef/adapters/math.hpp>
#include <nda/clef/sum.hpp>

#include "utility/concept_tools.hpp"
#include "utility/exceptions.hpp"

#define TRIQS_CLEF_MAKE_FNT_LAZY CLEF_MAKE_FNT_LAZY
#define TRIQS_CLEF_IMPLEMENT_LAZY_CALL CLEF_IMPLEMENT_LAZY_CALL

namespace stdutil = nda::stdutil;

namespace triqs {

  namespace clef = nda::clef;
  using dcomplex = std::complex<double>;
  using namespace std::literals::complex_literals;

  using nda::range;

} // namespace triqs

namespace triqs::utility {

  template <typename T, int R> using mini_vector = std::array<T, R>;

}

namespace triqs::arrays {

  using namespace nda;

  using nda::blas::dot;

  using utility::mini_vector;

  template <typename T>//[[deprecated]] 
    nda::matrix<T> make_unit_matrix(int dim) { return nda::eye<T>(dim); }

  template <typename... T> //[[deprecated]]
    std::array<long, sizeof...(T)> make_shape(T... x) { return {long(x)...}; }
} // namespace triqs::arrays

namespace nda {

  // Rotate the index n to 0, preserving the relative order of the other indices
  template <int N, typename A> //[[deprecated]] 
    auto rotate_index_view(A &&a) {
    return permuted_indices_view<encode(nda::permutations::cycle<get_rank<A>>(1, N))>(std::forward<A>(a));
  }

} // namespace nda
