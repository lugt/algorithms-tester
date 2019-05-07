//
// Created by xc5 on 2019-03-22.
//

#include "common_def.h"

void tracedTopK(ELET *values, ELET_OFST length, SPTR parentSpan) {
  AS_CHILD_SPAN(span, "tracedTopK", parentSpan);
  // Top K
  std::sort(&values[0], &values[length]);
  ELET topList[10] = {0};
  for (ELET_OFST j = 0; j < 10 && length - j >= 0; j ++){
    topList[j] = values[length - j];
  }
}



static inline void insertToList(ELETVEC *list, ELET_OFST len, ELET num) {
  if(list->empty()) {
    list->push_back(num);
  } else if (list->size() < 10) {
    for (auto it = list->begin(); it != list->end(); it++) {
      if (num > *it) {
        list->insert(it, num);
        break;
      }
    }
  } else {
    for (auto it = list->begin(); it != list->end(); it++) {
      if (num > *it) {
        list->insert(it, num);
        list->pop_back();
        break;
      }
    }
  }
}

static inline void insertToListPck(ELETVEC *list, ELET_OFST len, ELET num, ELET Qtile) {
  if(num < Qtile) return;
  if(list->empty()) {
    list->push_back(num);
  } else if (list->size() < 10) {
    for (auto it = list->begin(); it != list->end(); it++) {
      if (num > *it) {
        list->insert(it, num);
        break;
      }
    }
  } else {
    for (auto it = list->begin(); it != list->end(); it++) {
      if (num > *it) {
        list->insert(it, num);
        list->pop_back();
        break;
      }
    }
  }
}

void tracedTopKBuffer(ELET *values, ELET_OFST length, SPTR parentSpan) {
  AS_CHILD_SPAN(span, "tracedTopKBuffer", parentSpan);
  ELETVEC topList;
  // Top K
  for(ELET_OFST i = length - 1; i >= 0; i--) {
    insertToList(&topList, 10, values[i]);
  }
  std::cout << topList[0] << ", "
  << topList[1] << ", "
  << topList[2] << ", "
  << topList[3] << ", "
  << topList[4] << ", "
  << topList[5] << ", "
  << topList[6] << ", "
  << topList[7] << ", "
  << topList[8] << ", "
  << topList[9] << std::endl;
}

void tracedTopKHeap(ELET *values, ELET_OFST length, SPTR parentSpan) {

}

void tracedTopKLRU(ELET *values, ELET_OFST length, SPTR parentSpan) {

}


/** Buffer with Pre-check () */
void tracedTopKPckBufferStatic(ELET *values, ELET_OFST length, SPTR parentSpan) {
  AS_CHILD_SPAN(span, "tracedTopKPckBuffer", parentSpan);
  ELETVEC topList;
  AS_CHILD_SPAN(restPart, "restPart", span)
  // Top K
  for(ELET_OFST i = length - 1; i >= 0; i--) {
    insertToListPck(&topList, 10, values[i], 99999900);
  }
  std::cout << topList[0] << ", "
            << topList[1] << ", "
            << topList[2] << ", "
            << topList[3] << ", "
            << topList[4] << ", "
            << topList[5] << ", "
            << topList[6] << ", "
            << topList[7] << ", "
            << topList[8] << ", "
            << topList[9] << std::endl;
}

/** Buffer with Pre-check () */
void tracedTopKPckBufferDynamic(ELET *values, ELET_OFST length, SPTR parentSpan) {
  AS_CHILD_SPAN(span, "tracedTopKPckBufferDynamic", parentSpan);
  ELETVEC topList;
  ELET Qtile = 0;

  AS_CHILD_SPAN(qtilePart, "quantilePart", span)
  ELET maxVal = 0;
  for (ELET_OFST i = 100000; i >= 0; i--) {
    if(values[i] > maxVal) {
      maxVal = values[i];
    }
  }

  Qtile = (ELET)(maxVal * 0.999);

  AS_CHILD_SPAN(restPart, "restPart", span)
  // Top K
  for(ELET_OFST i = length - 1; i >= 0; i--) {
    insertToListPck(&topList, 10, values[i], Qtile);
  }
  std::cout << topList[0] << ", "
            << topList[1] << ", "
            << topList[2] << ", "
            << topList[3] << ", "
            << topList[4] << ", "
            << topList[5] << ", "
            << topList[6] << ", "
            << topList[7] << ", "
            << topList[8] << ", "
            << topList[9] << std::endl;
}

void tracedTopKPckHeap(ELET *values, ELET_OFST length, SPTR parentSpan) {

}


