//-*-C++-*-

#ifndef SQUARE_2D_H
#define SQUARE_2D_H
#include "phys_library/domains/cluster/symmetries/symmetry_operations/2D/Cn.h"
#include "phys_library/domains/cluster/symmetries/symmetry_operations/2D/Sn.h"
#include "comp_library/type_list/type_list.h"

/*!
 *  \author: Peter Staar
 */

/*!
 *  group-actions
 */

typedef Cn_2D<1,4> Cn_2D_1_4_type;
typedef Cn_2D<2,4> Cn_2D_2_4_type;
typedef Cn_2D<3,4> Cn_2D_3_4_type;
typedef Cn_2D<4,4> Cn_2D_4_4_type;

typedef Sn_2D<0,8> Sn_2D_0_8_type;
typedef Sn_2D<1,8> Sn_2D_1_8_type;
typedef Sn_2D<2,8> Sn_2D_2_8_type;
typedef Sn_2D<3,8> Sn_2D_3_8_type;
typedef Sn_2D<4,8> Sn_2D_4_8_type;
typedef Sn_2D<5,8> Sn_2D_5_8_type;
typedef Sn_2D<6,8> Sn_2D_6_8_type;
typedef Sn_2D<7,8> Sn_2D_7_8_type;
typedef Sn_2D<8,8> Sn_2D_8_8_type;

/*
 *  pointgroup :: set of group-actions
 */

struct C4
{
  typedef TYPELIST_4(Cn_2D_1_4_type,
                     Cn_2D_2_4_type,
                     Cn_2D_3_4_type,
                     Cn_2D_4_4_type) point_group_type_list;
};

struct S4
{
  typedef TYPELIST_4(Sn_2D_0_8_type,
                     Sn_2D_2_8_type,
                     Sn_2D_4_8_type,
                     Sn_2D_6_8_type) point_group_type_list;
};

struct S4_plus
{
  typedef TYPELIST_4(identity_group_operation<2>,
                     Cn_2D_2_4_type,
                     Sn_2D_0_8_type,
                     Sn_2D_2_8_type) point_group_type_list;
};

struct S8
{
  typedef TYPELIST_9(Sn_2D_0_8_type,
                     Sn_2D_1_8_type,
                     Sn_2D_2_8_type,
                     Sn_2D_3_8_type,
                     Sn_2D_4_8_type,
                     Sn_2D_5_8_type,
                     Sn_2D_6_8_type,
                     Sn_2D_7_8_type,
                     identity_group_operation<2>) point_group_type_list;
};

struct D4
{
  typedef TYPELIST_12(Cn_2D_1_4_type,
                      Cn_2D_2_4_type,
                      Cn_2D_3_4_type,
                      Sn_2D_0_8_type,
                      Sn_2D_1_8_type,
                      Sn_2D_2_8_type,
                      Sn_2D_3_8_type,
                      Sn_2D_4_8_type,
                      Sn_2D_5_8_type,
                      Sn_2D_6_8_type,
                      Sn_2D_7_8_type,
                      identity_group_operation<2>) point_group_type_list;
};

#endif
