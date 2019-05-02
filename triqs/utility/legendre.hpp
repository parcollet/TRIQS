/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2018 by L. Boehnke, M. Ferrero, O. Parcollet
 * Copyright (C) 2019 The Simons Foundation
 *   authors: N. Wentzell
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

#include <complex>
#include <ostream>

namespace triqs::utility {

  // This is T_{nl} following Eq.(E2) of our paper
  std::complex<double> legendre_T(int n, int l);

  // This is t_l^p following Eq.(E8) of our paper
  double legendre_t(int l, int p);

  // Modified spherical Bessel function of the first kind i(n,x)
  double mod_cyl_bessel_i(int n, double x);

  /**
   * Generates the Legendre polynomials
   *  P_0(x) = 1.0
   *  P_1(x) = x
   *  n P_{n} = (2n-1) x P_{n-1}(x) - (n-1) P_{n-2}(x)
   */
  class legendre_generator {

    double _x;
    unsigned int n;
    double cyclicArray[2];

    public:
    double next() {
      if (n > 1) {
        unsigned int eo = (n) % 2;
        cyclicArray[eo] = ((2 * n - 1) * _x * cyclicArray[1 - eo] - (n - 1) * cyclicArray[eo]) / n;
        n++;
        return cyclicArray[eo];
      } else {
        n++;
        return cyclicArray[n - 1];
      }
    }

    void reset(double x) {
      _x             = x;
      n              = 0;
      cyclicArray[0] = 1.0;
      cyclicArray[1] = x;
    }
  };

} // namespace triqs::utility
