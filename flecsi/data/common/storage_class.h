/*
    @@@@@@@@  @@           @@@@@@   @@@@@@@@ @@
   /@@/////  /@@          @@////@@ @@////// /@@
   /@@       /@@  @@@@@  @@    // /@@       /@@
   /@@@@@@@  /@@ @@///@@/@@       /@@@@@@@@@/@@
   /@@////   /@@/@@@@@@@/@@       ////////@@/@@
   /@@       /@@/@@//// //@@    @@       /@@/@@
   /@@       @@@//@@@@@@ //@@@@@@  @@@@@@@@ /@@
   //       ///  //////   //////  ////////  //

   Copyright (c) 2016, Triad National Security, LLC
   All rights reserved.
                                                                              */
#pragma once

/*!
  @file

  This file defines the base \em storage_class_u type that can be
  specialized by storage class, and by topology type.

  This file also defines the storage classes for the internal \em global
  and \em color client types.
 */

#include <flecsi/data/common/data_reference.h>

#ifndef POLICY_NAMESPACE
#error You must define a data policy namespace before including this file.
#endif

namespace flecsi {
namespace data {

/*!
  The storage_label_t type enumerates the available FleCSI storage classes.
  A FleCSI storage class provides a specific interface for different
  logical data layouts, e.g., dense vs. sparse. The actual data layout
  is implementation-dependent.
 */

enum storage_label_t : size_t {
  global,
  color,
  dense,
  sparse,
  ragged,
  subspace
}; // enum storage_label_t

namespace POLICY_NAMESPACE {

/*!
  Base storage class type for topology-specific specializations.
  Specializations of this type must implement a get_reference method that takes
  a topology reference.
 */

template<size_t STORAGE_CLASS, typename CLIENT_TYPE>
struct storage_class_u {};

} // namespace POLICY_NAMESPACE
} // namespace data
} // namespace flecsi
