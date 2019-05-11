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

#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <map>
#include "memory.h"
#include <cstring>


#define DISTRIBUTE_MIN    1
#define DISTRIBUTE_MAX    4
#define IFSORT(mode, code) if((current_sortmode) == (mode)) { code; }
#define AS_CHILD_SPAN(val, name, parent)   auto val = opentracing::Tracer::Global()->\
StartSpan(name, {opentracing::ChildOf(&parent->context())});
#define SPAN_OK(span) span->Finish();
#define Is_True11(x, y) if(!(x)) { \
          std::cerr << "Assertion failed during Working Mode [" << current_sortmode << "]" << std::endl ;  \
          std::cerr << "Triggered by " << __FILE__ << ":" << __func__ << ":" << __LINE__ << std::endl ;  \
          std::cerr y ; \
          std::cerr << std::endl; \
          exit(6); } ;

#define Is_True(cond, v) if (!(cond)) { printf("\nError encountered in %s at %s : %d; \n ", __FILE__, __func__, __LINE__); printf v; puts("\n"); exit(125); }

using std::vector;
// STL Includings
using std::cout;
using std::cin;
using std::cout;
using std::cin;
using std::endl;

typedef int INT32;
typedef unsigned int UINT32;
typedef int                    INT;
typedef unsigned int           UINT;
typedef long long int          INT64;
typedef bool                   BOOL;
typedef long long unsigned int UINT64;
typedef double                 F8;
typedef char                   CHAR;
typedef char*                  CHPTR;
typedef char**                 CHPPTR;
typedef const char*            CCHPTR;
typedef const char**           CCHPPTR;
typedef unsigned __int128      UINT128; // for hash

typedef const std::unique_ptr<opentracing::Span> & SPTR;
typedef INT32 ELET;
typedef INT64 ELET_OFST;
typedef vector<ELET> ELETVEC;


/*printf parm; getchar(); getchar(); getchar(); exit(1);*/
#define EXTERN extern
#define NULLPTR NULL
#define FALSE ((BOOL) 0)
#define TRUE  ((BOOL) 1)

inline BOOL Echo(){
  return FALSE;
}

inline BOOL EchoKrusKal() {
  return FALSE;
}

extern ELET *values;

inline void swap_num(ELET_OFST left, ELET_OFST right) {
  ELET temp = values[left];
  values[left] = values[right];
  values[right] = temp;
}

#endif //JAEGERTRACING_COMMON_DEF_H
