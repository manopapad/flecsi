/*~--------------------------------------------------------------------------~*
 *  @@@@@@@@  @@           @@@@@@   @@@@@@@@ @@
 * /@@/////  /@@          @@////@@ @@////// /@@
 * /@@       /@@  @@@@@  @@    // /@@       /@@
 * /@@@@@@@  /@@ @@///@@/@@       /@@@@@@@@@/@@
 * /@@////   /@@/@@@@@@@/@@       ////////@@/@@
 * /@@       /@@/@@//// //@@    @@       /@@/@@
 * /@@       @@@//@@@@@@ //@@@@@@  @@@@@@@@ /@@
 * //       ///  //////   //////  ////////  //
 *
 * Copyright (c) 2016 Los Alamos National Laboratory, LLC
 * All rights reserved
 *~--------------------------------------------------------------------------~*/

#ifndef flecsi_space_vector_h
#define flecsi_space_vector_h

#include <array>
#include <cmath>

#include "../data/dimensioned_array.h"
#include "../utils/common.h"
#include "point.h"

/*!
 * \file space_vector.h
 * \authors bergen
 * \date Initial file creation: Sep 23, 2015
 */

namespace flecsi
{
/*!
  \class space_vector space_vector.h
  \brief space_vector defines an interface for storing and manipulating
  space_vector data associated with a geometric domain.

  The space_vector type is implemented using \ref dimensioned_array.  Look there
  for more information on the vector_t interface.
 */
template <typename T, size_t D>
using space_vector = dimensioned_array<T, D, 2>;

/*!
  \function operator*(const space_vector<T,D> & v, const T s)
 */
template <typename T, size_t D>
space_vector<T, D> operator*(const space_vector<T, D> & v, const T s)
{
  space_vector<T, D> tmp(v);
  for (size_t d(0); d < D; ++d)
    tmp[d] = s * v[d];
  return tmp;
}

/*!
  \function dot
 */
template <typename T, size_t D>
T dot(const space_vector<T, D> & a, const space_vector<T, D> & b)
{
  T sum(0);

  for (size_t d(0); d < D; ++d) {
    sum += a[d] * b[d];
  } // for

  return sum;
} // dot

/*!
  \function magnitude
 */
template <typename T, size_t D>
T magnitude(const space_vector<T, D> & a)
{
  T sum(0);
  for (size_t d(0); d < D; ++d) {
    sum += square(a[d]);
  } // for

  return std::sqrt(sum);
} // magnitude

/*!
  \function normal
  \brief for a vector xi + yj the normal vector is -yi + xj. given points
  a and b we use x = b[0] - a[0] and y = b[1] - a[1]
 */
template <typename T>
space_vector<T, 2> normal(const point<T, 2> & a, const point<T, 2> & b)
{
  space_vector<T, 2> tmp(a[1] - b[1], b[0] - a[0]);
  return tmp;
} // normal

/*!
  \function normal
 */
template <typename T>
space_vector<T, 3> normal(
    const space_vector<T, 3> & a, const space_vector<T, 3> & b)
{
  space_vector<T, 3> tmp(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
      a[0] * b[1] - a[1] * b[0]);
  return tmp;
} // normal

} // namespace flecsi

#endif // flecsi_space_vector_h

/*~-------------------------------------------------------------------------~-*
 * Formatting options
 * vim: set tabstop=2 shiftwidth=2 expandtab :
 *~-------------------------------------------------------------------------~-*/
