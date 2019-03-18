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

#define DISTRIBUTE_MIN    1
#define DISTRIBUTE_MAX    99999999
#define IFSORT(mode, code) if((current_sortmode) == (mode)) { code; }
#define AS_CHILD_SPAN(val, name, parent)   auto val = opentracing::Tracer::Global()->\
StartSpan(name, {opentracing::ChildOf(&parent->context())});
#define SPAN_OK(span) span->Finish();

using std::vector;

typedef int INT32;
typedef long long INT64;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef vector<INT32> I32VEC;
typedef const std::unique_ptr<opentracing::Span> & SPTR;

extern INT32 *values;

extern void tracedInsertion(INT32 *values, INT32 length, SPTR parentSpan);

extern void tracedMerge(INT32 *values, INT32 length, SPTR parentSpan);

extern void tracedQuick(INT32 *values, INT32 length, SPTR parentSpan);

extern void tracedSelection(INT32 *values, INT32 length, SPTR parentSpan);

extern void tracedBubble(INT32 *values, INT32 length, SPTR parentSpan);

inline void swap_num(INT32 left, INT32 right) {
  INT32 temp = values[left];
  values[left] = values[right];
  values[right] = temp;
}

#endif //JAEGERTRACING_COMMON_DEF_H
