//
// Created by xc5 on 2019-04-06.
//

#include "common_def.h"


INT32 DIVISION = 8;

static INT64 *minimums;

void quaterProblem (ELET * xvalues, ELET * yvalues, INT32 seq, ELET_OFST whole_length, ELET_OFST i_begin, ELET_OFST i_end) {
  INT64 result = 0;
  INT64 minimum = ((INT64) 0x07ffffff << 32) + 0xffffffff;

  for (ELET_OFST i = i_begin; i < i_end; i++) {
    for (ELET_OFST j = i + 1; j < whole_length; j++)
    {
      if ((result = fBresenham(xvalues[i], xvalues[j], yvalues[i], yvalues[j])) < minimum) {
        minimum = result;
      }
    }
  }

  minimums[seq] = minimum;
}

void initializeMinimums(){
  INT32 num = DIVISION;
#if defined(TARGET_OS_MAC) || !(defined(_WIN32))
  num = std::thread::hardware_concurrency();
  if (num < DIVISION) {
    num = DIVISION;
  }
#endif
  if (num <= 0) {
    num = 8;
  }
  DIVISION = num;
  Is_True(DIVISION > 0, << "CPU core num must be > 0" << std::endl);
  minimums = new INT64[DIVISION];
}

void finalizeMinimums(){
  free(minimums);
}

/******************************************************************************
 * Divide by a vector / use vector to calculate BC given AB and AC
 * @param values
 * @param length
 * @param parentSpan
 *****************************************************************************/
void tracedBrute (ELET *values, ELET_OFST length, SPTR parentSpan) {

  AS_CHILD_SPAN(span, "tracedBrute_Quad", parentSpan)

  initializeMinimums();

  ELET * xvalues = values;
  ELET * yvalues = values + length;

  INT64 result = 0;
  INT64 minimum = ((long) 0x07ffffff << 32) + 0xffffffff;

  std::thread ** th_arr = new std::thread* [DIVISION];

  // Creating DIVISION threads
  for (INT32 seq = 0; seq < DIVISION; seq ++) {
    ELET_OFST part_len = length / DIVISION;
    std::thread *thread_this = new std::thread(quaterProblem, xvalues, yvalues, seq, length, seq * part_len, (seq + 1) * part_len);
    th_arr[seq] = thread_this;
  }

  // Joining all threads (wait for finishing)
  for (INT32 seq = 0; seq < DIVISION; seq ++) {
    th_arr[seq]->join();
  }

  // Merging results.
  for (INT32 seq = 0; seq < DIVISION; seq++) {
    if (minimums[seq] < minimum) {
      minimum = minimums[seq];
    }
  }

  finalizeMinimums();
  std::cout << minimum;

};