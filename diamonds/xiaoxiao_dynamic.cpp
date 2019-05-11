//
// Created by lugt on 2019/5/5.
//

#include "common_def.h"
#include "xiaoxiaole.h"
#include "xiaoxiao_util.h"

#include <map>
#include <vector>
#include <list>

using std::unordered_map;

static std::unordered_map<INT64, INT> * dynamic_remain_max;
static std::unordered_map<UINT128, INT> * max_pts_per_node;
static std::unordered_map<INT64, vector<INT> * > * dynamic_arrive_pts;

static BOOL Backtrack_reject(REDUCTION_CTX &p, REDUCTION_NODE &c){
  // no, go on
  return false;
}

static BOOL Backtrack_accept(REDUCTION_CTX &p, REDUCTION_NODE &c){
  return true;
}

/**
 * procedure bt(c)
  if reject(P,c) then return
  if accept(P,c) then output(P,c)
  s ← first(P,c)
  while s ≠ Λ do
    bt(s)
    s ← next(P,s)
 */
static INT Backtrack_finding(REDUCTION_CTX &p, REDUCTION_NODE &c) {
  backtrack_time ++;
  if (Backtrack_reject(p, c)) {
    // free(c.now);
    // c.now = NULL;
    return 0;
  }
  if (Backtrack_accept(p, c)) {
    // Backtrack_record_output(p, c);
  }
  // Traverse kids
  // printf("Backtracking ... \n");
  REDUCTION_RESULT moves = find_all_possible_swap(
    c.now,
    p.width,
    p.height);

  INT64 total = moves.size;
  INT local_max = 0;
  if (total == 0) return 0;
  for(INT i = 0; i < total; i++){
    // Calculate c'
    REDUCTION_NODE cprime = Generate_c_prime(p, c, moves, i);
    INT somemax = Backtrack_finding(p, cprime);
    if(somemax > local_max) {
      local_max = somemax;
    }
    // ADD free.
    free(cprime.now);
  }

  INT local_delta_max = local_max - c.pts;
  if(c.depth <= 4) {
    printf("D:%d branch:%lld, max: %d, dmax: %d\n",
           c.depth, total, local_max, local_delta_max);
  }

  // Calculate the somemax at this point;
  // Map -> Map_Remain_max
  // Retro + Dynamic way

  Is_True(false, ("Unfinished paths.\n"));

  INT64 depth_num = 0;
  if (dynamic_remain_max->count(depth_num) < 1) {
    dynamic_remain_max->insert(std::make_pair(depth_num, local_delta_max));
    (*dynamic_arrive_pts)[depth_num] = new vector<INT> ();
    (*dynamic_arrive_pts)[depth_num]->push_back(c.pts);
    return local_delta_max;
  }
  (*dynamic_arrive_pts)[depth_num]->push_back(local_delta_max);
  INT depth_num_max = (*dynamic_remain_max)[depth_num];
  if (depth_num_max < local_delta_max) {
    (*dynamic_remain_max)[depth_num] = local_delta_max;
  }
  return local_max;
}


INT Cross_Dynamic (ELET *rand_values, ELET_OFST size, SPTR parentSpan) {
  AS_CHILD_SPAN(span, "Cross-dynamic-whole", parentSpan);
  // INT64 correct_size = size >> 8;
  // Generate Random Assemble
  // +Refine Assemble
  INT width = 4;
  INT height = 8;
  ELET_OFST       len       = width * height;
  REDUCTION_CTX  *ctx       = new REDUCTION_CTX;
  ELET           *copy      = (ELET *) malloc(len * sizeof(ELET));
  REDUCTION_NODE *node      = new REDUCTION_NODE;
  ctx->value                = rand_values;
  ctx->width                = width;
  ctx->height               = height;
  node->now                 = copy;
  dynamic_remain_max = new std::unordered_map<INT64, INT>;
  Copy_map(copy, rand_values, width, height);
  refineNodes(copy, width, height, span);
  Backtrack_finding(*ctx, *node);
  printf("Dynamic Global max = %d, times = %d \n", global_max_pts, backtrack_time);
  // Start of Reduction Process
  return 0;
}
