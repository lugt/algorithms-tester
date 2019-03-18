//
// Created by xc5 on 2019-03-16.
//

#include <vector>
#include <iostream>
#include "common_def.h"

I32VEC merge(I32VEC a, I32VEC b) {
  I32VEC res;
  INT32 ba = 0;
  INT32 bb = 0;

  while (ba < a.size() && bb < b.size()) {
    if (a[ba] <= b[bb]) {
      res.push_back(a[ba++]);
    } else {
      res.push_back(b[bb++]);
    }
  }

  if (ba == a.size()) {
    while (bb < b.size()) res.push_back(b[bb++]);
  } else if (bb == b.size()) {
    while (ba < a.size()) res.push_back(a[ba++]);
  }

  return res;
}

I32VEC mergeSort(I32VEC arr) {
  auto s = (INT32) arr.size();
  if (s < 2) return arr;
  INT32 mid = s / 2;
  I32VEC front(arr.begin(), arr.begin() + mid);
  I32VEC back(arr.begin() + mid, arr.end());
  return merge(mergeSort(front), mergeSort(back));
}

void tracedMerge(INT32 * values, INT32 length, SPTR parent) {
  AS_CHILD_SPAN(span, "tracedMergeSort", parent);

  AS_CHILD_SPAN(merge_vector_span, "tracedMergeSortVectorize", span);
  vector<INT32> temp(values, values + length);
  SPAN_OK(merge_vector_span);

  AS_CHILD_SPAN(merge_loop_span, "tracedMergeSortLoop", span);
  mergeSort(temp);
  SPAN_OK(merge_loop_span);
}