/*~-------------------------------------------------------------------------~~*
 * Copyright (c) 2014 Los Alamos National Security, LLC
 * All rights reserved.
 *~-------------------------------------------------------------------------~~*/

///
/// \file
/// \date Initial file creation: Apr 11, 2017
///

#include <cinchtest.h>

#include <flecsi/execution/execution.h>
#include <flecsi/supplemental/coloring/add_colorings.h>
#include <flecsi/supplemental/mesh/test_mesh_2d.h>

clog_register_tag(coloring);

namespace flecsi {
namespace execution {

using coloring_info_t = flecsi::coloring::coloring_info_t;
using adjacency_info_t = flecsi::coloring::adjacency_info_t;

using test_mesh_t = flecsi::supplemental::test_mesh_2d_t;

template<typename DC, size_t PS>
using client_handle_t = data_client_handle__<DC, PS>;

void
init(client_handle_t<test_mesh_t, ro> mesh, sparse_mutator<double> mh) {
  auto & context = execution::context_t::instance();
  auto rank = context.color();
  auto coloring_info = context.coloring_info(mh.h_.index_space).at(rank);

  for (size_t i = 0; i < coloring_info.exclusive + coloring_info.shared; ++i) {
    for (size_t j = 0; j < 5; j += 2) {
      mh(i, j) = i * 100 + j + rank * 10000;
    }
  }
} // init

void
print(
    client_handle_t<test_mesh_t, ro> mesh,
    sparse_accessor<double, ro, ro, ro> h) {
  auto & context = execution::context_t::instance();
  auto rank = context.color();
  if (rank == 0) {
    for (auto index : h.indices()) {
      for (auto entry : h.entries(index)) {
        CINCH_CAPTURE() << index << ":" << entry << ": " << h(index, entry)
                        << std::endl;
      }
    }
  }
} // print

void
modify(
    client_handle_t<test_mesh_t, ro> mesh,
    sparse_accessor<double, rw, rw, rw> h) {

  auto & context = execution::context_t::instance();
  auto rank = context.color();
  auto coloring_info = context.coloring_info(h.handle.index_space).at(rank);

  for (auto c : mesh.cells(owned)) {
    for (auto entry : h.entries(c)) {
      h(c, entry) = -h(c, entry);
    }
  }
} // modify

void
mutate(client_handle_t<test_mesh_t, ro> mesh, sparse_mutator<double> mh) {
  auto & context = execution::context_t::instance();
  auto rank = context.color();
  auto coloring_info = context.coloring_info(mh.h_.index_space).at(rank);

  for (size_t i = 0; i < coloring_info.exclusive + coloring_info.shared; ++i) {
    for (size_t j = 5; j < 7; ++j) {
      mh(i, j) = i * 100 + j + rank * 10000;
    }
  }
} // mutate

flecsi_register_data_client(test_mesh_t, meshes, mesh1);

flecsi_register_task_simple(init, loc, single);
flecsi_register_task_simple(print, loc, single);
flecsi_register_task_simple(modify, loc, single);
flecsi_register_task_simple(mutate, loc, single);

flecsi_register_field(
    test_mesh_t,
    hydro,
    pressure,
    double,
    sparse,
    1,
    index_spaces::cells);

//----------------------------------------------------------------------------//
// Specialization driver.
//----------------------------------------------------------------------------//

void
specialization_tlt_init(int argc, char ** argv) {
  clog(info) << "In specialization top-level-task init" << std::endl;
  coloring_map_t map{index_spaces::vertices, index_spaces::cells};
  flecsi_execute_mpi_task(add_colorings, flecsi::execution, map);

  auto & context = execution::context_t::instance();
  auto & cc = context.coloring_info(index_spaces::cells);
  auto & cv = context.coloring_info(index_spaces::vertices);

  adjacency_info_t ai;
  ai.index_space = index_spaces::cells_to_vertices;
  ai.from_index_space = index_spaces::cells;
  ai.to_index_space = index_spaces::vertices;
  ai.color_sizes.resize(cc.size());

  for (auto & itr : cc) {
    size_t color = itr.first;
    const coloring_info_t & ci = itr.second;
    ai.color_sizes[color] = (ci.exclusive + ci.shared + ci.ghost) * 4;
  }

  context.add_adjacency(ai);

  context_t::sparse_index_space_info_t isi;
  isi.index_space = index_spaces::cells;
  isi.max_entries_per_index = 10;
  isi.exclusive_reserve = 8192;
  context.set_sparse_index_space_info(isi);
} // specialization_tlt_init

void
specialization_spmd_init(int argc, char ** argv) {
  auto mh = flecsi_get_client_handle(test_mesh_t, meshes, mesh1);
  flecsi_execute_task(initialize_mesh, flecsi::supplemental, single, mh);
} // specialization_spmd_init

//----------------------------------------------------------------------------//
// User driver.
//----------------------------------------------------------------------------//

void
driver(int argc, char ** argv) {
  auto ch = flecsi_get_client_handle(test_mesh_t, meshes, mesh1);
  auto mh = flecsi_get_mutator(ch, hydro, pressure, double, sparse, 0, 10);
  auto ph = flecsi_get_handle(ch, hydro, pressure, double, sparse, 0);

  flecsi_execute_task_simple(init, single, ch, mh);
  flecsi_execute_task_simple(print, single, ch, ph);

  flecsi_execute_task_simple(modify, single, ch, ph);
  flecsi_execute_task_simple(print, single, ch, ph);

  flecsi_execute_task_simple(mutate, single, ch, mh);
  flecsi_execute_task_simple(print, single, ch, ph);

  auto & context = execution::context_t::instance();
  if (context.color() == 0) {
    ASSERT_TRUE(CINCH_EQUAL_BLESSED("sparse_data.blessed"));
  }
} // specialization_driver

//----------------------------------------------------------------------------//
// TEST.
//----------------------------------------------------------------------------//

TEST(sparse_data, testname) {} // TEST

} // namespace execution
} // namespace flecsi

/*~------------------------------------------------------------------------~--*
 * Formatting options for vim.
 * vim: set tabstop=2 shiftwidth=2 expandtab :
 *~------------------------------------------------------------------------~--*/
