#include <algorithm>
#include <vector>

#include "rastBBox_fix.h"
#include "assert.h"
#include "limits.h"
#include "helper.h"

void rastBBox_vec_fix(vector < u_Poly < int64, ushort > >& polys, zbuff& z) {
  int i;
  int l = polys.size();
  u_Poly < int64, ushort > poly;
  int64 screen_h, screen_w;

  int r_shift = 10;
  int r_val = 1024;

  screen_w = z.w * 1024;
  screen_h = z.h * 1024;

  for (i = 0; i < l; i ++) {
    poly = polys[i];
    rastBBox_uPoly_fix(poly, z, screen_w, screen_h,
                       z.ss_i, z.ss_w_lg2, r_shift, r_val);
  }
}

void rastBBox_uPoly_fix(u_Poly < int64, ushort >& poly,
                        zbuff& z,
                        int64 screen_w,
                        int64 screen_h,
                        int64 si,  // sample interval
                        int  si_lg2,  // log2 of sample interval
                        int64 r_shift,
                        int64 r_val) {
  // Calculate BBox

  bool valid;  // is u_poly valid based on bbox

  int64 ll_x, ll_y, ur_x, ur_y;
  int64 s_x, s_y;
  int64 j_x, j_y;

  int x, y, ss_x, ss_y, vert;
  fragment f;

  rastBBox_bbox_fix(poly, ll_x, ll_y, ur_x, ur_y,
                    z.ss_w_lg2, screen_w, screen_h,
                    valid, r_shift, r_val);

  // Iterate over Samples, Test if In uPoly
  for (s_x = ll_x; s_x <= ur_x; s_x += si) {
    for (s_y = ll_y; s_y <= ur_y; s_y += si) {
      rastBBox_jhash_jit_fix(s_x, s_y, z.ss_w_lg2, &j_x, &j_y);
      j_x = j_x << 2;
      j_y = j_y << 2;
      vert = rastBBox_stest_fix(poly, s_x + j_x, s_y + j_y);

      if (vert != -1) {
        x =  s_x >> r_shift;
        y =  s_y >> r_shift;
        ss_x = (s_x - (x << r_shift)) / si;
        ss_y = (s_y - (y << r_shift)) / si;
        f.z = poly.v[vert].x[2];
        f.c[0] = poly.v[vert].c[0];
        f.c[1] = poly.v[vert].c[1];
        f.c[2] = poly.v[vert].c[2];
        f.c[3] = poly.v[vert].c[3];
        z.process_Fragment(x, y, ss_x, ss_y,
                           f.z, f.c[0], f.c[1],
                           f.c[2], f.c[3]);
      }
    }
  }
}

void rastBBox_bbox_fix(u_Poly < int64, ushort >& poly,
                       int64& ll_x,
                       int64& ll_y,
                       int64& ur_x,
                       int64& ur_y,
                       int&  ss_w_lg2,  // log2 of subsample width
                       int64& screen_w,
                       int64& screen_h,
                       bool& valid,
                       int64 r_shift,
                       int64 r_val) {
  /*
  /  Function: rastBBox_bbox_fix
  /  Function Description: Determine a bounding box ll_x,ll_y,
  /   ur_x,ur_y from the micropolygon poly.  Note that this is
  /   a fixed point function.
  /
  /  Inputs:
  /    u_Poly < int64 , ushort >& poly - a micropolygon.  Definition found
  /                          in helper.h. Values given in fixed point
  /    int& ss_w_lg2 - a vector indicate the level of multisample anti-aliasing
  /        1xMSAA  : ss_w_lg2 = 0
  /        4xMSAA  : ss_w_lg2 = 1
  /        16xMSAA : ss_w_lg2 = 2
  /        64xMSAA : ss_w_lg2 = 3
  /    int64& screen_w - fixed point screen width
  /    int64& screen_h - fixed point screen height
  /    int64 r_shift - Number of Fractional Bits
  /    int64 r_val - 2 ^ r_shift  (eg r_shift = 10 r_val = 1024)
  /
  /  Outputs:
  /    bool& valid - Is the bounding box an micropolygon valid
  /                  for example an offscreen microplygon is invalid
  /    int64& ll_x - Bounding Box Lower Left X Coordinate
  /    int64& ll_y - Bounding Box Lower Left Y Coordinate
  /    int64& ur_x - Bounding Box Upper Right X Coordinate
  /    int64& ur_y - Bounding Box Upper Right Y Coordinate
  /
  */

  /* rastBBox_bbox_fix utilized fixed point computation
  /   to determine the bounding box for the micropolygon
  /   given in poly.  Note that u_poly is specified in helper.h
  /   The bounding box values are returned via reference
  /   in the variables ll_x ll_y ur_x ur_y correpsonding
  /   to lower left coordinate and upper right coordinate.
  /   ss_w_lg2 is a log2 value related to supersampling:
  /     1xMSAA  : ss_w_lg2 = 0
  /     4xMSAA  : ss_w_lg2 = 1
  /     16xMSAA : ss_w_lg2 = 2
  /     64xMSAA : ss_w_lg2 = 3
  /   screen_w is the screen width and screen_h
  /   is the screen height.
  /   r_shift represents the total number of fractional
  /   bits utilized in the fixed point values.  r_val
  /   represents the value of those bits in fixed point.
  /   For a fixed point representation which allocates
  /   10 bits for the fraction r_val=1024 and r_shift=10.
  /   r_shift and ss_w_lg2 are useful for rounding down
  /   to the nearest sample  ex:
  /   rounded_value = (val >> (r_shift - ss_w_lg2)) << (r_shift -
  /             ss_w_lg2);
  /   additionally it should be noted that whether a poly is
  /   a quad or triangle can be determined by examing poly.vertices
  */

  ur_x = 0;
  ur_y = 0;
  ll_x = 0;
  ll_y = 0;

  /////
  ///// Bounding Box Function Goes Here
  /////

  ///// PLACE YOUR CODE HERE

  // calculating boundary box
  int vertexid = 1;
  ur_x = poly.v[0].x[0];
  ll_x = poly.v[0].x[0];
  ur_y = poly.v[0].x[1];
  ll_y = poly.v[0].x[1];
  for (; vertexid < poly.vertices; vertexid ++) {
    ur_x = max(poly.v[vertexid].x[0], ur_x);
    ll_x = min(poly.v[vertexid].x[0], ll_x);
    ur_y = max(poly.v[vertexid].x[1], ur_y);
    ll_y = min(poly.v[vertexid].x[1], ll_y);
  }

  // clipping with screen
  ur_x = min(ur_x, screen_w);
  ll_x = max(ll_x, static_cast<int64>(0));
  ur_y = min(ur_y, screen_h);
  ll_y = max(ll_y, static_cast<int64>(0));

  // validation and rounding
  if (ur_x <= ll_x || ur_y <= ll_y) {
    valid = false;
  } else {
    valid = true;
    ur_x = (ur_x >> (r_shift - ss_w_lg2)) << (r_shift - ss_w_lg2);
    ll_x = (ll_x >> (r_shift - ss_w_lg2)) << (r_shift - ss_w_lg2);
    ur_y = (ur_y >> (r_shift - ss_w_lg2)) << (r_shift - ss_w_lg2);
    ll_y = (ll_y >> (r_shift - ss_w_lg2)) << (r_shift - ss_w_lg2);
  }

  /////
  ///// Bounding Box Function Goes Here
  /////
}


#define DEBUG

void debug_print_poly(u_Poly < int64, ushort >& poly) {
  int i;
  for (i=0; i < poly.vertices; i++) {
    fprintf(stderr, "poly %d: %ld, %ld\n", i,
        poly.v[i].x[0],
        poly.v[i].x[1]);
  }
}

int rastBBox_stest_fix(u_Poly < int64, ushort >& poly,
                       int64 s_x, int64 s_y) {
  // This function determines whether the sample coordinates s_x and s_y
  //  lie inside the micropolygon poly. Note that u_poly is specified in
  //  helper.h.  The psuedocode should be more than sufficient in explaining
  //  the implementation.   Also, note that the fact that the values
  //  are fixed point is insignficant.

  /*
  /  Function: rastBBox_stest_fix
  /  Function Description: Does the sample point defined by s_x and s_y
  /                        lie inside the micropolygon defined by poly
  /  Inputs:
  /    u_Poly < int64 , ushort >& poly - A micropolygon.  Definition in helper.h
  /    int64 s_x - The fixed point value for the sample's x coordinate
  /    int64 s_y - The fixed point value for the sample's y coordinate
  /
  /  Output:
  /   Return -1   : If Miss
  /   Return  0   : If Hit
  /     note that we return (result - 1) so result can be set to 0 for
  /     miss and 1 for hit
  /
  */
  /****************************************************************************/
  /****************************************************************************/

  int result = 1;  // Default to hit state
  int i;
  int j;
  int64 edge_dist_to_origin[4];
  bool edge_ok[4];
  int64 s[2] = {s_x, s_y};

  // A new polygon that we run the tests against - this polygon is referenced
  // to the origin based on s_x and s_y
  u_Poly < int64, ushort > poly_origin;
  poly_origin.vertices = poly.vertices;

  // First re-reference the polygon to the origin
  for (i = 0; i < poly.vertices; i++) {
    for (j = 0; j < 2; j++) {
      poly_origin.v[i].x[j] = poly.v[i].x[j] - s[j];
    }
  }

  // Evaluate the distance of each edge to the origin
  for (i = 0; i < poly_origin.vertices; i++) {
    j = (i + 1) % poly_origin.vertices;
    edge_dist_to_origin[i] =
      poly_origin.v[i].x[0] * poly_origin.v[j].x[1] -
      poly_origin.v[j].x[0] * poly_origin.v[i].x[1];
  }

  // For each edge, test if the origin is to the right of the edge
  for (i=0; i < poly_origin.vertices; i++) {
    edge_ok[i] = edge_dist_to_origin[i] <= 0;
  }

  // For each edge and the result and ok to return overall ok
  for (i=0; i < poly_origin.vertices; i++) {
    result = result && edge_ok[i];
  }

  #ifdef DEBUG
  debug_print_poly(poly);
  fprintf(stderr, "sample: %ld, %ld\n", s_x, s_y);
  debug_print_poly(poly_origin);
  for (i=0; i < poly.vertices; i++) {
    fprintf(stderr, "edge_dist %d: %ld\n", i, edge_dist_to_origin[i]);
    fprintf(stderr, "edge_ok %d: %d\n", i, edge_ok[i]);
  }
  #endif

  /****************************************************************************/
  /****************************************************************************/
  return (result-1);  // Return 0 if hit, otherwise return -1
}


void rastBBox_40t8_hash(uchar* arr40, ushort* val, int shift) {
  uchar arr32[4];
  uchar arr16[2];
  uchar arr8;

  ushort mask = 0x00ff;
  mask = mask >> shift;

  arr32[0] = arr40[0] ^ arr40[1];
  arr32[1] = arr40[1] ^ arr40[2];
  arr32[2] = arr40[2] ^ arr40[3];
  arr32[3] = arr40[3] ^ arr40[4];

  /*  printf("C: %.2x %.2x %.2x %.2x\n",
      arr32[3], arr32[2] , arr32[1] , arr32[0]);*/

  arr16[0] = arr32[0] ^ arr32[2];
  arr16[1] = arr32[1] ^ arr32[3];


  // printf("C: %.2x %.2x\n", arr16[1], arr16[0]);

  arr8 = arr16[0] ^ arr16[1];

  // printf("C: %.2x \n", arr8);


  mask = arr8 & mask;
  val[0] = mask;
}

void rastBBox_jhash_jit_fix(
  const int64& s_x,
  const int64& s_y,
  const int64& ss_w_lg2,
  int64* jitter_x,
  int64* jitter_y) {
  int64  x = s_x >> 4;
  int64  y = s_y >> 4;
  uchar arr40_1[5];
  uchar arr40_2[5];

  int64* arr40_1_ptr = reinterpret_cast<int64*>(arr40_1);
  int64* arr40_2_ptr = reinterpret_cast<int64*>(arr40_2);

  ushort val_x[1];
  ushort val_y[1];

  *arr40_1_ptr = (y << 20) | x;
  *arr40_2_ptr = (x << 20) | y;

  rastBBox_40t8_hash(arr40_1, val_x, ss_w_lg2);
  rastBBox_40t8_hash(arr40_2, val_y, ss_w_lg2);

  *jitter_x = static_cast<int64>(val_x[0]);
  *jitter_y = static_cast<int64>(val_y[0]);
}


