// Copyright (C) 2009 by Thomas Moulard, AIST, CNRS, INRIA.
//
// This file is part of the roboptim.
//
// roboptim is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// roboptim is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with roboptim.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ROBOPTIM_CORE_NUMERIC_LINEAR_FUNCTION_HXX
# define ROBOPTIM_CORE_NUMERIC_LINEAR_FUNCTION_HXX
# include "debug.hh"

# include <boost/format.hpp>

# include <roboptim/core/indent.hh>
# include <roboptim/core/numeric-linear-function.hh>
# include <roboptim/core/util.hh>

namespace roboptim
{
  template <typename T>
  GenericNumericLinearFunction<T>::GenericNumericLinearFunction
  (const matrix_t& a, const vector_t& b)
    throw ()
    : GenericLinearFunction<T>
      (a.cols (), a.rows (), "numeric linear function"),
      a_ (a),
      b_ (b)
  {
    assert (b.size () == this->outputSize ());
  }

  template <typename T>
  GenericNumericLinearFunction<T>::GenericNumericLinearFunction
  (const GenericLinearFunction<T>& function) throw ()
    : GenericLinearFunction<T>
      (function.inputSize (), function.outputSize (),
       (boost::format
	("numeric linear function (built from %s)")
	% function.getName ()).str ()),
      a_ (function.outputSize (),
	  function.inputSize ()),
      b_ (function.outputSize ())
  {
    vector_t x (function.inputSize ());
    x.setZero ();
    b_ = function (x);
    x.setConstant (1.);
    function.jacobian (a_, x);
  }

  template <typename T>
  GenericNumericLinearFunction<T>::~GenericNumericLinearFunction () throw ()
  {
  }

  // A * x + b
  template <typename T>
  void
  GenericNumericLinearFunction<T>::impl_compute (result_t& result,
						 const argument_t& argument)
    const throw ()
  {
    result.noalias () = a_* argument;
    result += b_;
  }

  // A
  template <typename T>
  void
  GenericNumericLinearFunction<T>::impl_jacobian
  (jacobian_t& jacobian, const argument_t&) const throw ()
  {
    jacobian = this->a_;
  }

  // A(i) - sparse specialization
  template <>
  inline void
  GenericNumericLinearFunction<EigenMatrixSparse>::impl_gradient
  (gradient_t& gradient, const argument_t&, size_type idFunction)
    const throw ()
  {
    for (size_type j = 0; j < this->inputSize (); ++j)
      gradient.coeffRef (j) = a_.coeff (idFunction, j);
  }

  // A(i)
  template <typename T>
  void
  GenericNumericLinearFunction<T>::impl_gradient (gradient_t& gradient,
					const argument_t&,
					size_type idFunction) const throw ()
  {
    for (size_type j = 0; j < this->inputSize (); ++j)
      gradient[j] = a_ (idFunction, j);
  }

  template <typename T>
  std::ostream&
  GenericNumericLinearFunction<T>::print (std::ostream& o) const throw ()
  {
    return o << "Numeric linear function" << incindent << iendl
             << "A = " << this->a_ << iendl
             << "B = " << this->b_
             << decindent;
  }

} // end of namespace roboptim

#endif //! ROBOPTIM_CORE_LINEAR_FUNCTION_HXX
