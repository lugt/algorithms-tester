//
// Created by xc5 on 2019-04-06.
//
#include "common_def.h"

typedef struct xy_pair {
  ELET x;
  ELET y;
} XYPAIR;


/******************************************************************************
 * Check if a is less then b
 * @param a
 * @param b
 * @return
 *****************************************************************************/
bool cmpPts(const XYPAIR &a, const XYPAIR &b) {
  if(a.x < b.x) return true;
  else if (a.x > b.x) return false;
  else if (a.y <= b.y) return true;
  else return false;
}


/******************************************************************************
 * Check if a is less then b in x ASC, y ASC order
 * @param a
 * @param b
 * @return
 *****************************************************************************/
int compPair(const void *pa, const void *pb) {
  XYPAIR &a = *((XYPAIR *) pa);
  XYPAIR &b = *((XYPAIR *) pb);
  if(a.x < b.x) return -2;
  else if (a.x > b.x) return 2;
  else if (a.y < b.y) return -1;
  else if (a.y > b.y) return 1;
  else return 0;
}

/******************************************************************************
 * Check if a is less then b in y ASC, order
 * @param a
 * @param b
 * @return
 *****************************************************************************/
int cmpY(const void *pa, const void *pb) {
  XYPAIR &a = *((XYPAIR *) pa);
  XYPAIR &b = *((XYPAIR *) pb);
  if (a.y < b.y) return -1;
  else if (a.y == b.y) return 0;
  else return 1;
}

/******************************************************************************
 * Check if a is less then b
 * @param a
 * @param b
 * @return
 *****************************************************************************/
bool cmpY(const XYPAIR &a, const XYPAIR &b) {
  if (a.y <= b.y) return true;
  else return false;
}


/*******************************************************************************
 * Return a sorted XYPair List with Y-sorted, and then X-stable sorted
 * @param xvalues_ptr
 * @param yvalues_ptr
 * @param length
 * @return
 ******************************************************************************/
XYPAIR * sortFromXY (ELET **xvalues_ptr, ELET **yvalues_ptr, ELET_OFST length){
  ELET * org_x = *xvalues_ptr;
  ELET * org_y = *yvalues_ptr;
  XYPAIR * xy_pair = new XYPAIR[length];
  ELET_OFST j = 0;
  ELET * i = NULL;
  for (i = org_x, j = 0; j < length; i++, j++) {
    xy_pair[j].x = org_x[j];
    xy_pair[j].y = org_y[j];
  }

  Is_True (xy_pair[0].x >= 0 && xy_pair[length - 1].y >= 0,
    << "found errorenout data in xypairs, length = " << length
    << ", " << xy_pair[0].x
    << ", " << xy_pair[length - 1].y << endl);

  Is_True (std::distance(xy_pair, xy_pair + length) == length, << "std::distance reported not exact length needed" << endl);
  // std::sort(xy_pair, xy_pair + length, cmpPts);
  qsort (xy_pair, length, sizeof(XYPAIR), compPair);
  return xy_pair;
}

/*******************************************************************************
 * Calculate the square of distance between points
 ******************************************************************************/
INT64 calcDistanceQuad(XYPAIR &a, XYPAIR &b){
  return quad(LL(a.x) - LL(b.x)) + quad(LL(a.y) - LL(b.y));
}

static ELET_OFST find_first_inside_seam (XYPAIR * xypair,
                                             ELET_OFST mid,
                                             ELET_OFST lbound,
                                             ELET_OFST ubound,
                                             INT64 delta) {
  ELET_OFST it = mid;
  ELET mid_x = xypair[mid].x;
  while (it > lbound) {
    if (xypair[it].x < mid_x - delta) return it + 1;
    it --;
  }
  // it == lbound
  return it;
}

static ELET_OFST find_last_inside_seam (XYPAIR * xypair,
                                        ELET_OFST mid,
                                        ELET_OFST lbound,
                                        ELET_OFST ubound,
                                        INT64 delta) {
  ELET_OFST it = mid;
  ELET mid_x = xypair[mid].x;
  while (it < ubound) {
    if (xypair[it].x > mid_x + delta) return it - 1;
    it ++;
  }
  // it == ubound
  return ubound - 1;
}

static inline ELET_OFST find_first_on_vertical_line (XYPAIR * xy_pair,
  ELET_OFST mid, ELET_OFST begin){
  ELET x = xy_pair[mid].x;
  while (mid != begin) {
    if (xy_pair[mid].x != x) return mid + 1;
    mid --;
  }
  return mid;
}

static inline ELET_OFST find_next_vertical_line (XYPAIR * xy_pair,
                                                 ELET_OFST mid, ELET_OFST end){
  ELET x = xy_pair[mid].x;
  while (mid != end) {
    if (xy_pair[mid].x != x) return mid;
    mid ++;
  }
  return mid;
}


XYPAIR * middle = nullptr;

void mergePoints(XYPAIR * xypair, ELET_OFST size_a, ELET_OFST size_all) {
  if (middle == nullptr) {
    middle = new XYPAIR[MAX_INDEX];
  }

  ELET_OFST ba = 0;
  ELET_OFST bb = size_a;
  ELET_OFST outpos = 0;
  ELET_OFST size_b = size_all - size_a;

  while (ba < size_a && bb < size_b) {
    if (xypair[ba].y <= xypair[bb].y) {
      middle[outpos].y = xypair[ba].y;
      middle[outpos++].x = xypair[ba++].x;
    } else {
      middle[outpos].y = xypair[bb].y;
      middle[outpos++].x = xypair[bb++].x;
    }
  }

  for(ELET_OFST pa = 0; pa < size_all; pa++) {
    xypair[pa].x = middle[pa].x;
    xypair[pa].y = middle[pa].y;
  }
}

// i_end 是可以访问的最后一个
INT64 calcdivideByAxisSeq(XYPAIR *xypair, ELET_OFST i_begin, ELET_OFST i_end, std::unique_ptr<opentracing::Span> & parentSpan){

  std::unique_ptr<opentracing::Span> span = nullptr;

  if (i_end <= i_begin) {
    return DISTANCE_MAX;
  }

  if (i_end  == i_begin + 1) {
    return calcDistanceQuad(xypair[i_end], xypair[i_begin]);
  }

  if (i_end == i_begin + 2) {
    INT64 d1 = calcDistanceQuad(xypair[i_begin + 1], xypair[i_begin]);
    INT64 d2 = calcDistanceQuad(xypair[i_begin + 2], xypair[i_begin]);
    INT64 d3 = calcDistanceQuad(xypair[i_begin + 1], xypair[i_begin + 2]);
    return std::min(std::min(d1, d2), d3);
  }

  if (i_end  - i_begin > 1000000) {
    std::ostringstream os;
    os << "i_end : " << i_end;
    std::ostringstream osb;
    osb << "i_begin : " << i_begin;
    std::ostringstream oss;
    oss << "size : " << (i_end - i_begin);
    if (parentSpan.get() == nullptr) {
      span = opentracing::Tracer::Global()->StartSpan(
        "DARecurse_Nul");
    } else {
      span = opentracing::Tracer::Global()->StartSpan(
        "DARecurse", {opentracing::ChildOf(&parentSpan->context())});
    }
    span->SetBaggageItem("ibegin", osb.str());
    span->SetBaggageItem("iend", os.str());
    span->SetBaggageItem("size", oss.str());
  }

  ELET_OFST pre_mid = (i_end + i_begin) / 2;

  ELET_OFST mid = pre_mid;

//  ELET_OFST mid = find_first_on_vertical_line(xypair, pre_mid, i_begin);
//
//  if (mid == i_begin ||  mid== i_end) {
//    mid = find_next_vertical_line(xypair, pre_mid, i_end);
//  }

  INT64 dl = calcdivideByAxisSeq(xypair, i_begin, mid - 1, span);
  INT64 dr = calcdivideByAxisSeq(xypair, mid, i_end, span);

  // The two-side minimum in distance between point pairs
  dl = std::min(dl, dr);

  // Locating the seam
  // first and last are visitable offsets
  // This is done when data is x ASC, y ASC ordered
  ELET_OFST first = find_first_inside_seam(xypair, mid, i_begin, i_end, dl);
  ELET_OFST last = find_last_inside_seam(xypair, mid, i_begin, i_end, dl);

  // ELET_OFST first = i_begin;
  // ELET_OFST last = i_end;

  // O(N)
  // Data after this point will be only y ASC ordered.
  mergePoints(xypair + first, (mid - first), (last - first) + 1);

  ELET_OFST it = first;
  INT64 distance = dl;
  for (; it != last; it++) {
    // Check over six items
    for (INT32 ofst = 1; ofst < 6 && ofst <= last - it; ofst ++) {
      if(((distance = calcDistanceQuad(xypair[it], xypair[it + ofst]))
          < dl)) {
        dl = distance;
      }
    }
  }

  // N * Log(N)
  // Data after this point will be only x ASC, y ASC ordered.
  // qsort(xypair + first, (size_t) (last - first), sizeof(XYPAIR), compPair);

  // cout << "one-recurse-found : " << dl << endl;

  return dl;
}

void divideByAxisSeq(XYPAIR *xypair,
                     INT32 seq,
                     ELET_OFST whole_length,
                     ELET_OFST i_begin, ELET_OFST i_end,
                     opentracing::Span * pSpan) {
  // Not allowing multi-core at this point.
  if(seq != 0) return;
  std::unique_ptr<opentracing::Span> span = nullptr;
  if (pSpan != nullptr) {
     span =
      opentracing::Tracer::Global()->StartSpan("tracedDA_Thread",
                                               {opentracing::ChildOf(
                                                 &pSpan->context())});
  } else {
    span =
      opentracing::Tracer::Global()->StartSpan("tracedDA_Thread_Nptr");
  }

  Is_True(whole_length % 100000 == 0, << "error on length not a multiplex of 100000" << endl);
  INT64 dl = calcdivideByAxisSeq(xypair, 0, whole_length - 1, span);
  minimums[seq] = dl;

  // Output Result:
  std::ostringstream os;
  os << "min_num : " << dl;
  span->SetBaggageItem("result", os.str());
  if(tracing) cout << os.str() << endl;
}

/******************************************************************************
 * Divide by a vector / use vector to calculate BC given AB and AC
 * @param values
 * @param length
 * @param parentSpan
 *****************************************************************************/
void tracedDivAxis (ELET *values, ELET_OFST length, SPTR parentSpan) {

  auto spanPtr = opentracing::Tracer::Global()->
    StartSpan("tracedDivideAxis_All", {opentracing::ChildOf(&parentSpan->context())});
  opentracing::Span *span = spanPtr.get();

  initializeMinimums();

  ELET * xvalues = values;
  ELET * yvalues = values + length;

  std::unique_ptr<opentracing::Span> sort_span = opentracing::Tracer::Global()->
    StartSpan("tracedDivideAxis_sort", {opentracing::ChildOf(&(spanPtr->context()))});

  XYPAIR * xypair = sortFromXY(&xvalues, &yvalues, length);
  sort_span->Finish();

  INT64 minimum = ((long) 0x07ffffff << 32) + 0xffffffff;

  std::thread ** th_arr = new std::thread* [DIVISION];

  // Creating DIVISION threads
  for (INT32 seq = 0; seq < DIVISION; seq ++) {
    ELET_OFST part_len = length / DIVISION;

    std::thread *thread_this = new std::thread(divideByAxisSeq, xypair, seq,
                                               length,
                                               0,
                                               length,
                                               span);
    th_arr[seq] = thread_this;
  }

  // Joining all threads (wait for finishing)
  for (INT32 seq = 0; seq < DIVISION; seq ++) {
    th_arr[seq]->join();
  }

  // Merging results.
  for (INT32 seq = 0; seq < DIVISION; seq++) {
    if (minimums[seq] < minimum) {
      if(tracing) std::cout << "new_global_minimum : " << minimum  << ", on seq: " << seq << endl;
      minimum = minimums[seq];
    }
  }

  finalizeMinimums();
  spanPtr->Finish();
  if(tracing) std::cout << "global_minimum : " << minimum << endl;
};



// i_end 是可以访问的最后一个
INT64 calcdivideBySieveSeq(XYPAIR *xypair, ELET_OFST i_begin, ELET_OFST i_end, std::unique_ptr<opentracing::Span> & parentSpan){

  std::unique_ptr<opentracing::Span> span = nullptr;

  if (i_end <= i_begin) {
    return DISTANCE_MAX;
  }

  if (i_end  == i_begin + 1) {
    return calcDistanceQuad(xypair[i_end], xypair[i_begin]);
  }

  if (i_end == i_begin + 2) {
    INT64 d1 = calcDistanceQuad(xypair[i_begin + 1], xypair[i_begin]);
    INT64 d2 = calcDistanceQuad(xypair[i_begin + 2], xypair[i_begin]);
    INT64 d3 = calcDistanceQuad(xypair[i_begin + 1], xypair[i_begin + 2]);
    return std::min(std::min(d1, d2), d3);
  }

  if (i_end  - i_begin > 1000000) {
    std::ostringstream os;
    os << "i_end : " << i_end;
    std::ostringstream osb;
    osb << "i_begin : " << i_begin;
    std::ostringstream oss;
    oss << "size : " << (i_end - i_begin);
    if (parentSpan.get() == nullptr) {
      span = opentracing::Tracer::Global()->StartSpan(
        "DARecurse_Nul");
    } else {
      span = opentracing::Tracer::Global()->StartSpan(
        "DARecurse", {opentracing::ChildOf(&parentSpan->context())});
    }
    span->SetBaggageItem("ibegin", osb.str());
    span->SetBaggageItem("iend", os.str());
    span->SetBaggageItem("size", oss.str());
  }

  std::vector<XYPAIR> s;
  std::vector<XYPAIR> s_i_plus_1;
  // ... get pairs ... , where s is a copy of pairs


#if 0
  ELET_OFST iter = 1;
  ELET_OFST size_of_s = i_end - i_begin;
  ELET_OFST whole_length = i_end - i_begin;
  HASH * hashes = new HASH[size_of_s];
  INT64 s_minimum = DISTANCE_MAX;

  while (s.size() != 0) {
    std::default_random_engine generator;
    std::uniform_int_distribution<ELET> distribution(0, size_of_s);
    auto dice = std::bind(distribution, generator);
    // size must be the size of s
    // Brute force compute d(x)
    ELET_OFST pnt = dice();
    for (ELET_OFST i = 0; i < size_of_s; i++) {
      if (i == pnt) continue;
      INT64 dc = calcDistanceQuad(xypair[pnt], xypair[i]);
      if (dc < s_minimum) s_minimum = dc;
    }
    // do a hash,
    for(ELET_OFST i = 0; i < size_of_s; i++) {
      // calc hash
      hashes[i] = hash_point_to_mesh(s[i], s_minimum);
    }
    // Remove points no longer needed
    for(ELET_OFST i = 0; i < size_of_s; i++) {
      // calc hash
      hashes[i] = hash_point_to_mesh(s[i], s_minimum);
    }
    // remove points that are singled out
    for(ELET_OFST i = 0; i < size_of_s; i++) {
      // calc hash
      hashes[i];
      // remove points
    }
    if (...) break;
    s_i_plus_1.push();
    iter ++;
  }
#else
  ELET_OFST dl = 0;
#endif

  // final di
  // calc mesh with size di / 3
  // per hash - calc distance within the

  // This step takes O(N) = N * Constant time
  // Const T0

  return dl;
}

void divideBySieveSeq(XYPAIR *xypair,
                     INT32 seq,
                     ELET_OFST whole_length,
                     ELET_OFST i_begin, ELET_OFST i_end,
                     opentracing::Span * pSpan) {
  // Not allowing multi-core at this point.
  if(seq != 0) return;
  std::unique_ptr<opentracing::Span> span = nullptr;
  if (pSpan != nullptr) {
    span =
      opentracing::Tracer::Global()->StartSpan("tracedDS_Thread",
                                               {opentracing::ChildOf(
                                                 &pSpan->context())});
  } else {
    span =
      opentracing::Tracer::Global()->StartSpan("tracedDS_Thread_Nptr");
  }

  Is_True(whole_length % 100000 == 0, << "error on length not a multiplex of 100000" << endl);
  INT64 dl = calcdivideByAxisSeq(xypair, 0, whole_length - 1, span);
  minimums[seq] = dl;

  // Output Result:
  std::ostringstream os;
  os << "min_num : " << dl;
  span->SetBaggageItem("result", os.str());
  if(tracing) cout << os.str() << endl;
}


/******************************************************************************
 * Divide by a SIEVE function
 * @param values
 * @param length
 * @param parentSpan
 *****************************************************************************/
void tracedDivXYSpit (ELET *values, ELET_OFST length, SPTR parentSpan) {


  auto spanPtr = opentracing::Tracer::Global()->
    StartSpan("tracedDivideSieve_All", {opentracing::ChildOf(&parentSpan->context())});
  opentracing::Span *span = spanPtr.get();

  initializeMinimums();

  ELET * xvalues = values;
  ELET * yvalues = values + length;

  std::unique_ptr<opentracing::Span> sort_span = opentracing::Tracer::Global()->
    StartSpan("tracedDivideSieve_sort", {opentracing::ChildOf(&(spanPtr->context()))});

  XYPAIR * xypair = sortFromXY(&xvalues, &yvalues, length);
  sort_span->Finish();

  INT64 minimum = ((long) 0x07ffffff << 32) + 0xffffffff;

  std::thread ** th_arr = new std::thread* [DIVISION];

  // Creating DIVISION threads
  for (INT32 seq = 0; seq < DIVISION; seq ++) {
    ELET_OFST part_len = length / DIVISION;

    std::thread *thread_this = new std::thread(divideByAxisSeq, xypair, seq,
                                               length,
                                               0,
                                               length,
                                               span);
    th_arr[seq] = thread_this;
  }

  // Joining all threads (wait for finishing)
  for (INT32 seq = 0; seq < DIVISION; seq ++) {
    th_arr[seq]->join();
  }

  // Merging results.
  for (INT32 seq = 0; seq < DIVISION; seq++) {
    if (minimums[seq] < minimum) {
      if(tracing) std::cout << "new_global_minimum : " << minimum  << ", on seq: " << seq << endl;
      minimum = minimums[seq];
    }
  }

  finalizeMinimums();
  spanPtr->Finish();
  if(tracing) std::cout << "global_minimum : " << minimum << endl;

};
