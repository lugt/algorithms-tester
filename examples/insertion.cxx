//
// Created by xc5 on 2019-03-16.
//

#include "common_def.h"

void tracedSelection(INT32 *values, INT32 track_length, SPTR parent) {
  AS_CHILD_SPAN(span, "tracedSelection", parent);
  for (INT32 i = 0; i < track_length; i++) {
    INT32 smallest = i, sm_num = values[i];
    for (INT32 j = i + 1; j < track_length; j++) {
      if (values[j] < sm_num) smallest = i;
    }
    swap_num(smallest, i);
  }
}


void insertion_sort(INT32 arr[], INT32 length) {
  INT32 i, j, tmp;
  for (i = 1; i < length; i++) {
    j = i;
    while (j > 0 && arr[j - 1] > arr[j]) {
      tmp = arr[j];
      arr[j] = arr[j - 1];
      arr[j - 1] = tmp;
      j--;
    }
  }
}

void insertion_stl(std::vector<int> &vec)
{
  for (auto it = vec.begin(); it != vec.end(); it++)
  {
    // Searching the upper bound, i.e., first
    // element greater than *it from beginning
    auto const insertion_point =
      std::upper_bound(vec.begin(), it, *it);

    // Shifting the unsorted part
    std::rotate(insertion_point, it, it+1);
  }
}

void tracedInsertion(INT32 *values, INT32 track_length, SPTR parent) {
  AS_CHILD_SPAN(span, "tracedInsertion", parent);
  insertion_sort(values, track_length);
}