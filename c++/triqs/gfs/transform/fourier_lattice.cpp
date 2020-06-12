/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by M. Ferrero, O. Parcollet
 * Copyright (C) 2018- by Simons Foundation
 *               authors : O. Parcollet, N. Wentzell
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
#include "../../gfs.hpp"
#include "./fourier_common.hpp"
#include <itertools/itertools.hpp>

#define ASSERT_EQUAL(X, Y, MESS)                                                                                                                     \
  if (X != Y) TRIQS_RUNTIME_ERROR << MESS;

namespace triqs::gfs {

  // The implementation is almost the same in both cases...
  template <typename M1, typename M2> gf_vec_t<M1> __impl(int fftw_backward_forward, M1 const &out_mesh, gf_vec_cvt<M2> g_in) {

    //ASSERT_EQUAL(g_out.data().shape(), g_in.data().shape(), "Meshes are different");
    //ASSERT_EQUAL(g_out.data().indexmap().strides()[1], g_out.data().shape()[1], "Unexpected strides in fourier implementation");
    //ASSERT_EQUAL(g_out.data().indexmap().strides()[2], 1, "Unexpected strides in fourier implementation");
    //ASSERT_EQUAL(g_in.data().indexmap().strides()[1], g_in.data().shape()[1], "Unexpected strides in fourier implementation");
    //ASSERT_EQUAL(g_in.data().indexmap().strides()[2], 1, "Unexpected strides in fourier implementation");

    //check periodization_matrix is diagonal
    auto &period_mat = g_in.mesh().periodization_matrix;
    for (auto [i, j] : itertools::product_range(period_mat.shape()[0], period_mat.shape()[1]))
      if (i != j and period_mat(i, j) != 0) {
        std::cerr
           << "WARNING: Fourier Transform of k-mesh with non-diagonal periodization matrix. Please make sure that the order of real and reciprocal space vectors is compatible for FFTW to work. (Cf. discussion doi:10.3929/ethz-a-010657714, p.26)\n";
        break;
      }

    auto g_out    = gf_vec_t<M1>{out_mesh, std::array{g_in.target_shape()[0]}};
    long n_others = second_dim(g_in.data());

    auto dims = g_in.mesh().get_dimensions();
    auto dims_int = stdutil::make_std_array<int>(dims);

    _fourier_base(g_in.data(), g_out.data(), dims.size(), dims_int.data(), n_others, fftw_backward_forward);

    return std::move(g_out);
  }

  // ------------------------ DIRECT TRANSFORM --------------------------------------------

  gf_vec_t<mesh::torus> _fourier_impl(mesh::torus const &r_mesh, gf_vec_cvt<mesh::b_zone> gk) {
    auto gr = __impl(FFTW_FORWARD, r_mesh, gk);
    gr.data() /= gk.mesh().size();
    return std::move(gr);
  }

  // ------------------------ INVERSE TRANSFORM --------------------------------------------

  gf_vec_t<mesh::b_zone> _fourier_impl(mesh::b_zone const &k_mesh, gf_vec_cvt<mesh::torus> gr) {
    return __impl(FFTW_BACKWARD, k_mesh, gr);
  }

} // namespace triqs::gfs
