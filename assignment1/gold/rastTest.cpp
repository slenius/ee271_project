
#include "zbuff.h"
#include "helper.h"
#include "rastBBox_fix.h"
#include <stdlib.h>
#include <stdio.h>


// Helper functions for building triangles and quadrilaterals
void buildTri(u_Poly < int64, ushort >& poly,
              int64 x0, int64 y0,
              int64 x1, int64 y1,
              int64 x2, int64 y2,
              int64 r_shift){
  poly.vertices = 3;
  poly.v[0].x[0] = x0  <<  (r_shift - 2);
  poly.v[1].x[0] = x1  <<  (r_shift - 2);
  poly.v[2].x[0] = x2  <<  (r_shift - 2);

  poly.v[0].x[1] = y0  <<  (r_shift - 2);
  poly.v[1].x[1] = y1  <<  (r_shift - 2);
  poly.v[2].x[1] = y2  <<  (r_shift - 2);
}

void buildQuad(u_Poly < int64, ushort >& poly,
               int64 x0, int64 y0,
               int64 x1, int64 y1,
               int64 x2, int64 y2,
               int64 x3, int64 y3,
               int64 r_shift){
  poly.vertices = 4;
  poly.v[0].x[0] = x0  <<  (r_shift - 2);
  poly.v[1].x[0] = x1  <<  (r_shift - 2);
  poly.v[2].x[0] = x2  <<  (r_shift - 2);
  poly.v[3].x[0] = x3  <<  (r_shift - 2);

  poly.v[0].x[1] = y0  <<  (r_shift - 2);
  poly.v[1].x[1] = y1  <<  (r_shift - 2);
  poly.v[2].x[1] = y2  <<  (r_shift - 2);
  poly.v[3].x[1] = y3  <<  (r_shift - 2);
}

void RunSTest(u_Poly < int64, ushort >& poly,
                        const char* test_name,
              int64 s_x, int64 s_y,
              int64 r_shift,
              bool expect_pass){

  printf("\033[34mRunning %s\033[0m\n", test_name);

  s_x <<=  (r_shift - 2);
  s_y <<=  (r_shift - 2);
  int hit = rastBBox_stest_fix(poly, s_x, s_y);

  if (expect_pass){
    if (hit == 0){
      printf("\033[32m\t\tPASS %s\033[0m\n", test_name);
    }else{
      printf("\033[31m\t\tFAIL %s\033[0m\n", test_name);
    }
  }else{
    if (hit != 0){
      printf("\033[32m\t\tPASS %s\033[0m\n", test_name);
    }else{
      printf("\033[31m\t\tFAIL %s\033[0m\n", test_name);
    }
  }
}

void RunSTestExpectFail(u_Poly < int64, ushort >& poly,
                        const char* test_name,
                        int64 s_x, int64 s_y,
                        int64 r_shift){
  RunSTest(poly, test_name, s_x, s_y, r_shift, 0);
}

void RunSTestExpectPass(u_Poly < int64, ushort >& poly,
                        const char* test_name,
                        int64 s_x, int64 s_y,
                        int64 r_shift){
  RunSTest(poly, test_name, s_x, s_y, r_shift, 1);
}

/*

   Test Rast
     This is a test function with a
     few cases with known results.

     These tests should act as a rough
     test of whether the bounding box
     and sample test operation have
     been configured correctly.

     This function also demonstrates
     how some of the primitives are defined.
*/
bool testRast() {

  /* Lets Describe the Screen Space */
  long r_shift = 10;   //This is the number of fractional bits
  long r_val   = 1024;  //This is the value of 1  <<  r_shift

  long screen_w = 1024  <<  r_shift;  // 1024 pixels wide (on x axis)
  long screen_h = 1024  <<  r_shift;  // 1024 pixels tall (on y axis)

  int ss_w_lg2 = 2;  // This is the log_2 ( sqrt( MSAA ) )

  u_Poly< long, ushort > poly;  //This is a micropolygon
  poly.v[0].x[0] = 556  <<  (r_shift - 2);   // v0.x
  poly.v[0].x[1] = 679  <<  (r_shift - 2);   // v0.y

  poly.v[1].x[0] = 562  <<  (r_shift - 2);   // v1.x
  poly.v[1].x[1] = 660  <<  (r_shift - 2);   // v1.y

  poly.v[2].x[0] = 557  <<  (r_shift - 2);   // v2.x
  poly.v[2].x[1] = 661  <<  (r_shift - 2);   // v2.y

  poly.v[3].x[0] = 561  <<  (r_shift - 2);   // v3.x
  poly.v[3].x[1] = 680  <<  (r_shift - 2);   // v3.y

  poly.vertices = 3;  //The number of vertices
  // Three is a triangle
  // Four is a quadrilateral
  // Note that there are other parameters in
  // a micropolygon, but they are more or less
  // irrelevant for this portion of the algorithm

  long ll_x;
  long ll_y;
  long ur_x;
  long ur_y;
  bool valid;


  printf("Test 1: Bounding Box Test\n");
  poly.vertices = 3;

  rastBBox_bbox_fix(poly, ll_x, ll_y, ur_x, ur_y, ss_w_lg2,
                    screen_w, screen_h, valid, r_shift, r_val);

  if(! valid) {
    abort_("Fail Test 1 Condition 1");
  }
  if(ll_x != (556  <<  (r_shift - 2))) {
    abort_("Fail Test 1 Condition 2");
  }
  if(ll_y != (660  <<  (r_shift - 2))) {
    abort_("Fail Test 1 Condition 3");
  }
  if(ur_x != (562  <<  (r_shift - 2))) {
    abort_("Fail Test 1 Condition 4");
  }
  if(ur_y != (679  <<  (r_shift - 2))) {
    abort_("Fail Test 1 Condition 5");
  }

  printf("\t\tPass Test 1\n");

  /* more tests for boundary box */

  // bbox extra test 1 - upoly is a quad && clipping area beyond screen
  poly.v[0].x[0] = -1  <<  ( r_shift - 2 );  // v0.x
  poly.v[0].x[1] = 1220  <<  ( r_shift );  // v0.y

  poly.v[1].x[0] = 562  <<  ( r_shift - 2 );  // v1.x
  poly.v[1].x[1] = 660  <<  ( r_shift - 2 );  // v1.y

  poly.v[2].x[0] = 557  <<  ( r_shift - 2 );  // v2.x
  poly.v[2].x[1] = 661  <<  ( r_shift - 2 );  // v2.y

  poly.v[3].x[0] = 580  <<  ( r_shift - 2 );  // v3.x
  poly.v[3].x[1] = 681  <<  ( r_shift - 2 );  // v.y

  poly.vertices = 4;

  rastBBox_bbox_fix(  poly, ll_x, ll_y, ur_x, ur_y, ss_w_lg2,
          screen_w, screen_h, valid, r_shift, r_val);

  if( ! valid ){
    abort_("Fail Test 1-1");
  }
  if( ll_x != (0  <<  ( r_shift - 2 )) ){
    abort_("Fail Test 1-1");
  }
  if( ll_y != (660  <<  ( r_shift - 2 )) ){
    abort_("Fail Test 1-1");
  }
  if( ur_x != (580  <<  ( r_shift - 2 )) ){
    abort_("Fail Test 1-1");
  }
  if( ur_y != (1024  <<  r_shift ) ){
    abort_("Fail Test 1-1");
  }

  printf( "\t\tPass Test 1-1\n");

  // bbox extra test 2 - invalid bbox left of screen
  poly.v[0].x[0] = -1  <<  ( r_shift - 2 );  // v0.x
  poly.v[0].x[1] = 1220  <<  ( r_shift );  // v0.y

  poly.v[1].x[0] = -10  <<  ( r_shift - 2 );  // v1.x
  poly.v[1].x[1] = 660  <<  ( r_shift - 2 );  // v1.y

  poly.v[2].x[0] = -60  <<  ( r_shift - 2 );  // v2.x
  poly.v[2].x[1] = 661  <<  ( r_shift - 2 );  // v2.y

  poly.vertices = 3;

  rastBBox_bbox_fix(  poly, ll_x, ll_y, ur_x, ur_y, ss_w_lg2,
          screen_w, screen_h, valid, r_shift, r_val);

  if( valid ){
    abort_("Fail Test 1-2");
  }

  printf( "\t\tPass Test 1-2\n");

  // bbox extra test 3 - invalid bbox top of screen
  poly.v[0].x[0] = 1  <<  ( r_shift - 2 );  // v0.x
  poly.v[0].x[1] = 1220  <<  ( r_shift );  // v0.y

  poly.v[1].x[0] = 10  <<  ( r_shift - 2 );  // v1.x
  poly.v[1].x[1] = 1660  <<  ( r_shift );  // v1.y

  poly.v[2].x[0] = 60  <<  ( r_shift - 2 );  // v2.x
  poly.v[2].x[1] = 1661  <<  ( r_shift );  // v2.y

  poly.vertices = 3;

  rastBBox_bbox_fix(  poly, ll_x, ll_y, ur_x, ur_y, ss_w_lg2,
          screen_w, screen_h, valid, r_shift, r_val);

  if( valid ){
    abort_("Fail Test 1-3");
  }

  printf( "\t\tPass Test 1-3\n");

  // bbox extra test 4 - invalid bbox complicated
  poly.v[0].x[0] = -1  <<  ( r_shift - 2 );  // v0.x
  poly.v[0].x[1] = -10  <<  ( r_shift - 2 );  // v0.y

  poly.v[1].x[0] = -10  <<  ( r_shift - 2 );  // v1.x
  poly.v[1].x[1] = -1660  <<  ( r_shift - 2);  // v1.y

  poly.v[2].x[0] = -60  <<  ( r_shift - 2 );  // v2.x
  poly.v[2].x[1] = -1661  <<  ( r_shift - 2 );  // v2.y

  poly.vertices = 3;

  rastBBox_bbox_fix(  poly, ll_x, ll_y, ur_x, ur_y, ss_w_lg2,
          screen_w, screen_h, valid, r_shift, r_val);

  if( valid ){
    abort_("Fail Test 1-4");
  }

  printf( "\t\tPass Test 1-4\n");

  /*
     If you are having trouble determining if your bounding
     box function is correct, you can add more test cases
     here.
  */


  r_shift = 2;

  u_Poly< long, ushort > quad1;
  u_Poly< long, ushort > quad2;
  u_Poly< long, ushort > tri1;
  u_Poly< long, ushort > tri2;

  /*
     1,5       5,5
     *-------->*
     ^         |
     |         |
     |         |
     | *2,2    v
     *<--------*
     1,1       5,1
  */

  buildQuad(quad1,
            1, 1,
            1, 5,
            5, 5,
            5, 1,
            r_shift);

  buildQuad(quad2,
            556, 679,
            562, 660,
            557, 661,
            561, 680,
            r_shift);

  /*
     * 1,5     * 10,10
     ^\
     |  \  * 3,3
     |    \
     | *2,2 >
     *<------*
     1,1          5,1
  */

  buildTri(tri1,
           1, 1,
           1, 5,
           5, 1,
           r_shift);

  buildTri(tri2,
           556, 679,
           562, 660,
           557, 661,
           r_shift);

  RunSTestExpectPass(tri1, "Tri 1 In", 2, 2, r_shift);
  RunSTestExpectFail(tri1, "Tri 1 Out 1", 10, 10, r_shift);
  RunSTestExpectFail(tri1, "Tri 1 Out 2", 4, 4, r_shift);
  RunSTestExpectPass(tri1, "Tri 1 On Line X", 1, 3, r_shift);
  RunSTestExpectPass(tri1, "Tri 1 On Line Y", 3, 1, r_shift);
  RunSTestExpectPass(tri1, "Tri 1 On Point", 1, 1, r_shift);

  RunSTestExpectPass(quad1, "Quad 1 Inside", 2, 2, r_shift);
  RunSTestExpectFail(quad1, "Quad 1 Outside", 10, 10, r_shift);
  RunSTestExpectPass(quad1, "Quad 1 On Line", 1, 1, r_shift);

  RunSTestExpectPass(tri2, "Sample Test Tri 2", 559, 662, r_shift);
  RunSTestExpectFail(tri2, "Sample Test Tri 3", 560, 678, r_shift);

  RunSTestExpectFail(quad2, "Sample Test Quad 2", 560, 678, r_shift);

  return true;
}



int main(int argc, char **argv) {

  if(! testRast()) {
    abort_("Test Failed");
  }

  if(argc != 3) {
    abort_("Usage: program_name <file_out> <vector>");
  }

  // Set Screen and Subsample
  int w;
  int h;
  int ss;

  // Initialize the polygon soup
  vector< u_Poly< long, ushort > > polys;

  // Read in the list of polygons
  read_in_vector(polys, argv[2],&w, &h, &ss);

  int size_s = (int)polys.size();

  // Print filename
  printf("file = %s\n", argv[2]);

  // Report Number of polygons
  printf("n_poly = %i\n", size_s);

  // Initialize a Depth Buffer
  zbuff z(w,h,ss);

  // Rasterize the Scene
  rastBBox_vec_fix(polys, z);

  // Write the Zbuffer to a png
  z.write_ppm(argv[1]);

  return 0;
}
