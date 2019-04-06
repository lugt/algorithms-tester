//
// Created by xc5 on 2019-03-16.
//

#include <vector>
#include <iostream>
#include "common_def.h"

ELETVEC merge(ELETVEC a, ELETVEC b) {
  ELETVEC res;
  ELET ba = 0;
  ELET bb = 0;

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

ELETVEC mergeSort(ELETVEC arr) {
  auto s = (ELET) arr.size();
  if (s < 2) return arr;
  ELET mid = s / 2;
  ELETVEC front(arr.begin(), arr.begin() + mid);
  ELETVEC back(arr.begin() + mid, arr.end());
  return merge(mergeSort(front), mergeSort(back));
}

void tracedMerge(ELET * values, ELET_OFST length, SPTR parent) {
  AS_CHILD_SPAN(span, "tracedMergeSort", parent);

  AS_CHILD_SPAN(merge_vector_span, "tracedMergeSortVectorize", span);
  vector<ELET> temp(values, values + length);
  SPAN_OK(merge_vector_span);

  AS_CHILD_SPAN(merge_loop_span, "tracedMergeSortLoop", span);
  mergeSort(temp);
  SPAN_OK(merge_loop_span);
}