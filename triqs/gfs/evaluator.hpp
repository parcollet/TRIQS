/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2015 by O. Parcollet
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

namespace triqs {
  namespace gfs {

    /*----------------------------------------------------------
  *  Default
  *--------------------------------------------------------*/

    // gf_evaluator regroup functions to evaluate the function.
    // default : one variable. Will be specialized in more complex cases.
    template <typename Var, typename Target> struct gf_evaluator {
      static constexpr int arity = 1;

      template <typename G, typename X> auto operator()(G const &g, X x) const { return (g.mesh().evaluate(g, x)); }
    };

    /*----------------------------------------------------------
  *  imfreq
  *--------------------------------------------------------*/

    template <typename Target> struct gf_evaluator<imfreq, Target> {

      static constexpr int arity = 1;

      // FIXME : la tail est unique : tail -> valeur tensorielle
      // evaluate(s,f) --> retourne aussi un proxy !!!

      // technical details...
      using r_t  = typename Target::slice_t;

      // gf_evaluator
      // FIXME We can instead return a view and write the result of the tail calculation
      // to a member
      template <typename G> r_t operator()(G const &g, matsubara_freq const &f) const {
        if (g.mesh().is_within_boundary(f.n)) return g[f.n];
        if (g.mesh().positive_only()) {
          int sh = (g.mesh().domain().statistic == Fermion ? 1 : 0);
          if (g.mesh().is_within_boundary(-f.n - sh)) return r_t{conj(g[-f.n - sh])};
          TRIQS_RUNTIME_ERROR << " ERROR: Cannot evaluate Green function with positive only mesh outside grid ";
        }

        auto [tail, err] = fit_tail_no_normalize(g);

        dcomplex x                     = std::abs(g.mesh().omega_max()) / f;
        typename G::zero_regular_t res = g.get_zero();

        dcomplex z = 1.0;
        for (int n : range(first_dim(tail))) {
          res += tail(n, ellipsis()) * z;
          z = z * x;
        }
        return res;
      }

      // int -> replace by matsubara_freq
      template <typename G> decltype(auto) operator()(G const &g, int n) const {
        return g(matsubara_freq(n, g.mesh().domain().beta, g.mesh().domain().statistic));
      }
    };

    /*----------------------------------------------------------
  *  cartesian product
  *--------------------------------------------------------*/

    using triqs::make_const_view;
    inline dcomplex make_const_view(dcomplex z) { return z; }

    // now the multi d gf_evaluator itself.
    template <typename Target, typename... Ms> struct gf_evaluator<cartesian_product<Ms...>, Target> {

      static constexpr int arity = sizeof...(Ms); // METTRE ARITY DANS LA MESH !

      template <typename G, typename... Args> auto operator()(G const &g, Args &&... args) const {
        static_assert(sizeof...(Args) == arity, "Wrong number of arguments in gf evaluation");

        using r1_t = decltype(g.mesh().evaluate(g, std::forward<Args>(args)...));

        if constexpr (is_gf_expr<r1_t>::value or is_gf<r1_t>::value) {
          return g.mesh().evaluate(g, std::forward<Args>(args)...);
        } else {
          if (g.mesh().is_within_boundary(args...)) return make_regular(g.mesh().evaluate(g, std::forward<Args>(args)...));
          using rt = std::decay_t<decltype(make_regular(g.mesh().evaluate(g, std::forward<Args>(args)...)))>;
          return rt{g.get_zero()};
        }
      }
    };

    /*----------------------------------------------------------
  * Legendre
  *--------------------------------------------------------*/

    // Not finished, not tested
    template <> struct gf_evaluator<legendre, matrix_valued> {
      static constexpr int arity = 1;
      template <typename G> arrays::matrix_view<dcomplex> operator()(G const &g, long n) const {
        return g.data()(n, itertools::range(), itertools::range());
      }
    };
  } // namespace gfs
} // namespace triqs
