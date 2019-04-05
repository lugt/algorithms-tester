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
typedef const std::unique_ptr<opentracing::Span> & SPTR;
typedef INT32 ELET;
typedef INT64 ELET_OFST;
typedef vector<ELET> ELETVEC;

extern ELET *values;

extern void tracedInsertion(ELET *values, ELET_OFST length, SPTR parentSpan);

extern void tracedMerge(ELET *values, ELET_OFST length, SPTR parentSpan);

extern void tracedQuick(ELET *values, ELET_OFST length, SPTR parentSpan);

extern void tracedSelection(ELET *values, ELET_OFST length, SPTR parentSpan);

extern void tracedBubble(ELET *values, ELET_OFST length, SPTR parentSpan);

extern void tracedTopK(ELET *values, ELET_OFST length, SPTR parentSpan);

extern void tracedTopKBuffer(ELET *values, ELET_OFST length, SPTR parentSpan);

extern void tracedTopKHeap(ELET *values, ELET_OFST length, SPTR parentSpan);

extern void tracedTopKLRU(ELET *values, ELET_OFST length, SPTR parentSpan);

/** Buffer with Pre-check () */
extern void tracedTopKPckBufferStatic(ELET *values, ELET_OFST length, SPTR parentSpan);

extern void tracedTopKPckBufferDynamic(ELET *values, ELET_OFST length, SPTR parentSpan);

extern void tracedTopKPckHeap(ELET *values, ELET_OFST length, SPTR parentSpan);


inline void swap_num(ELET_OFST left, ELET_OFST right) {
  ELET temp = values[left];
  values[left] = values[right];
  values[right] = temp;
}

#endif //JAEGERTRACING_COMMON_DEF_H
