//
// Created by xc5 on 2019-03-16.
//

#include "common_def.h"

ELET_OFST partition(ELET arr[], ELET_OFST left, ELET_OFST right) {
  ELET pivot = arr[right];
  //choose the rightest element as the pivot
  ELET_OFST curr = left;
  for (ELET_OFST i = left; i < right; i++) {
    if (pivot > arr[i]) {
      //always put smaller element in the left
      std::swap(arr[curr], arr[i]);
      curr++;
    }
  }
  std::swap(arr[curr], arr[right]);
  //at last, put pivot element in the position it should be
  return curr;
}

void quicksort(ELET arr[], ELET_OFST left, ELET_OFST right) {
  if (left < right) {
    ELET_OFST p = partition(arr, left, right);
    quicksort(arr, left, p - 1);
    quicksort(arr, p + 1, right);
  }
}

void tracedQuick(ELET *values, ELET_OFST length, SPTR parent) {
  AS_CHILD_SPAN(span, "tracedQuick", parent);
  quicksort(values, 0, length - 1);
}

template<typename Iterator, typename Comparator>
void bubble_sort(Iterator begin, Iterator end, Comparator cmp) {
  for (auto j=end; j != begin; --j) {
    for (auto i = begin + 1; i != j; ++i) {
      auto& val0 = *i;
      auto& val1 = *(i - 1);
      if (cmp(val0, val1)) {
        std::swap(val1, val0);
      }
    }
  }
}

template<typename Iterator>
void bubble_sort(Iterator begin, Iterator end) {
  bubble_sort(begin, end, [](const typename Iterator::value_type &v0,
                             const typename Iterator::value_type &v1) {
    return v0 < v1;
  });
}

void tracedBubble(ELET *values, ELET_OFST length, SPTR parent) {
  AS_CHILD_SPAN(span, "tracedBubble", parent);
  ELETVEC temp(values, values + length);
  bubble_sort(temp.begin(), temp.end());
}