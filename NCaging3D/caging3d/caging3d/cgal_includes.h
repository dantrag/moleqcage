#ifndef CAGING3D_CGAL_INCLUDES_H
#define CAGING3D_CGAL_INCLUDES_H

#undef INCLUDE_EPECK
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#ifdef INCLUDE_EPECK
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#endif

#include <CGAL/Regular_triangulation_3.h>
#include <CGAL/Regular_triangulation_euclidean_traits_3.h>
//#include <CGAL/Fixed_alpha_shape_vertex_base_3.h>
//#include <CGAL/Fixed_alpha_shape_cell_base_3.h>
#include <CGAL/box_intersection_d.h>

namespace caging3d {

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3                                          Point;
typedef K::Segment_3                                        Segment;
typedef CGAL::Triangle_3<K>                                 Triangle;
typedef CGAL::Tetrahedron_3<K>                              Tetrahedron;
typedef CGAL::Ray_3<K>                                      Ray;
/*
typedef CGAL::Regular_triangulation_vertex_base_3<K>        Rvb;
typedef CGAL::Fixed_alpha_shape_vertex_base_3<K,Rvb>        Vb;
typedef CGAL::Regular_triangulation_cell_base_3<K>          Rcb;
typedef CGAL::Fixed_alpha_shape_cell_base_3<K,Rcb>          Cb;
typedef CGAL::Triangulation_data_structure_3<Vb,Cb>         Tds;
typedef CGAL::Regular_triangulation_3<K,Tds>                Triangulation_3;
typedef Triangulation_3::Weighted_point                     Weighted_point;
typedef Triangulation_3::Bare_point                         Bare_point;
*/

//for CGAL 4.7
typedef CGAL::Regular_triangulation_euclidean_traits_3<K>   Traits;
typedef Traits::Bare_point                                  Bare_point;
typedef Traits::Weighted_point                              Weighted_point;
typedef CGAL::Regular_triangulation_3<Traits>               Triangulation_3;

#ifdef INCLUDE_EPECK
typedef CGAL::Exact_predicates_exact_constructions_kernel      K_exact;
typedef CGAL::Point_3<K_exact>                                 Point_exact;
typedef CGAL::Segment_3<K_exact>                               Segment_exact;
typedef CGAL::Triangle_3<K_exact>                              Triangle_exact;
typedef CGAL::Tetrahedron_3<K_exact>                           Tetrahedron_exact;
typedef std::vector<Tetrahedron_exact>                         Tetrahedra;
typedef Tetrahedra::iterator                                   Tetrahedra_iterator;
typedef std::vector<Cell_handle>                               Cells;
typedef Cells::iterator                                        Cells_iterator;
typedef CGAL::Box_intersection_d::Box_with_handle_d<double, 3, Cells_iterator> Box;
#endif

} // namespace caging3d

#endif // CAGING3D_CGAL_INCLUDES_H
