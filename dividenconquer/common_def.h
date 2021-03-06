//
// Created by xc5 on 2019-03-16.
//

#ifndef JAEGERTRACING_COMMON_DEF_H
#define JAEGERTRACING_COMMON_DEF_H

#include <random>
#include <vector>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <jaegertracing/Tracer.h>
#include <stdlib.h>
#include <array>

#define DISTRIBUTE_MIN    1
#define DISTRIBUTE_MAX    999999
#define MAX_INDEX  2000000
#define DISTANCE_MAX ((INT64) 0x07ffffff << 32) + 0xffffffff
#define IFSORT(mode, code) if((current_sortmode) == (mode)) { code; }
#define AS_CHILD_SPAN(val, name, parent)   auto val = opentracing::Tracer::Global()->\
StartSpan(name, {opentracing::ChildOf(&parent->context())});
#define SPAN_OK(span) span->Finish();
#define Is_True(x, y) if(!(x)) { \
          std::cerr << "Assertion failed during Working Mode [" << current_sortmode << "]" << std::endl ;  \
          std::cerr << "Triggered by " << __FILE__ << ":" << __func__ << ":" << __LINE__ << std::endl ;  \
          std::cerr y ; \
          std::cerr << std::endl; \
          exit(6); } ;

using std::vector;
using std::endl;
using std::cout;
using std::cerr;

enum WorkingModule {
  NP_Brute,
  NP_Div_Axis,
  NP_Div_XYSpit,
  NP_Div_Circle,
  NP_Div_Elliptic,
  NP_Div_Vectors
};

typedef int INT32;
typedef long long INT64;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef const std::unique_ptr<opentracing::Span> & SPTR;
typedef INT32 ELET;
typedef INT64 ELET_OFST;
typedef float F4;
typedef double F8;
typedef vector<ELET> ELETVEC;
typedef bool BOOL;

extern ELET *values;

extern WorkingModule current_sortmode;

static BOOL tracing = false;

/******************************************************************************
 *  Multi-Core Utilities
 ******************************************************************************/
extern INT32 DIVISION;
extern INT64 *minimums;
extern void initializeMinimums();
extern void finalizeMinimums();

/******************************************************************************
 * Algorithms
 ******************************************************************************/

extern void tracedBrute (ELET *values, ELET_OFST length, SPTR parentSpan);
extern void tracedDivAxis (ELET *values, ELET_OFST length, SPTR parentSpan);
extern void tracedDivXYSpit (ELET *values, ELET_OFST length, SPTR parentSpan);
extern void tracedDivCircle (ELET *values, ELET_OFST length, SPTR parentSpan);
extern void tracedDivElliptic (ELET *values, ELET_OFST length, SPTR parentSpan);
extern void tracedDivVectors (ELET *values, ELET_OFST length, SPTR parentSpan);

/******************************************************************************
 * MISC Utilities
 ******************************************************************************/

inline void swap_num(ELET_OFST left, ELET_OFST right) {
  ELET temp = values[left];
  values[left] = values[right];
  values[right] = temp;
}

inline INT64 LL(INT32 x) {
  return (INT64) x;
}

inline INT64 long_mul(INT64 x, INT64 y) {
  return x * y;
}

inline INT64 quad(INT64 x) {
  return x * x;
}

/******************************************************************************
 * Distance Calculations
 * @param x
 * @param x2
 * @param y
 * @param y2
 * @return
 ******************************************************************************/
inline F8 fDist(ELET x, ELET x2, ELET y, ELET y2) {
  return sqrt(quad(LL(x) - LL(x2)) + quad(LL(y) - LL(y2)));
}

inline INT64 fDistQuad(ELET x, ELET x2, ELET y, ELET y2) {
  return quad(LL(x) - LL(x2)) + quad(LL(y) - LL(y2));
}

inline void swapNum(ELET &l, ELET &r) {
  ELET temp = l;
  l = r;
  r = temp;
}

inline INT64 fBresenham(ELET x0, ELET x1, ELET y0, ELET y1) {
  INT64 count = 0;
  BOOL steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swapNum(x0, y0);
    swapNum(x1, y1);
  }
  if (x0 > x1) {
    swapNum(x0, x1);
    swapNum(y0, y1);
  }
  INT64 deltax = x1 - x0;
  INT64 deltay = abs(y1 - y0);
  INT64 error = deltax / 2;
  INT64 ystep;
  INT64 y = y0;
  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  };
  for (ELET x = x0; x <= x1; x++) {
    count++;
    error = error - deltay;
    if (error < 0)
    {
      y = y + ystep;
      error = error + deltax;
    }
  }
  return count;
}

#endif //JAEGERTRACING_COMMON_DEF_H
