//
// Created by lugt on 2019/5/5.
//

#include "common_def.h"
#include "xiaoxiaole.h"
#include "xiaoxiao_util.h"

#include <list>
#include <unordered_map>

using  std::vector;

static std::unordered_map<INT64, INT> * max_arriving_pts_depth_branch = NULL;
static std::unordered_map<INT64, vector<INT> * > * max_pts_per_depth_num_sum = NULL;


/***
 *  Rejection Challenge Function based on Graph
 * @param p
 * @param c
 * @param moves
 * @return
 */
static BOOL Backtrack_reject_graph(REDUCTION_CTX &p, REDUCTION_NODE &c, REDUCTION_RESULT& moves){
  // 1. 计算c.now 的联通片数，点的总数，边的数量，点片比，联通起来所需要的平均距离、中位距离、最大、最小距离 ， 小于3的边的数量
  // 2. 估计 c.now 的剩余最大分数, 快速排除
  // 3. 估计 c.pts + c.now 的范围， 如果小于全局预测最大值 * 0.9，则拒绝（拒绝大约50%）
  // 4. 和 c.parent 的 now 做对比，如果发现某个值快速下降（片点比）、或 平均联通损耗（快速上升），则拒绝。(拒绝概率)
  printf("Rejecting D:%d, B:%d PTS:%d", c.depth, moves.size, c.pts);
  return true;
}

/***
 * CNN Based rejectivebility prediction
 * PARAMETER:
 *
 * c.now, c.parent, c.dots, c.edges,
 * c.linked-graphs, c.fully-link-cost, c.average-link-cost, c.edge-below-3-quatity.
 *
 * c.depth
 * moves.size
 *
 * p.width
 * p.height
 *
 *
 * OUTCOME:
 * 0. c.is_path_to_max
 * 1. c.delta_max;
 * 2. c.
 *
 * @param p
 * @param c
 * @param moves
 * @return
 */
static BOOL Backtrack_reject_cnn(REDUCTION_CTX &p, REDUCTION_NODE &c, REDUCTION_RESULT& moves){
  // 1. 计算c.now 的联通片数，点的总数，边的数量，点片比，联通起来所需要的平均距离、中位距离、最大、最小距离 ， 小于3的边的数量
  // 2. 估计 c.now 的剩余最大分数, 快速排除
  // 3. 估计 c.pts + c.now 的范围， 如果小于全局预测最大值 * 0.9，则拒绝（拒绝大约50%）
  printf("Rejecting D:%d, B:%d PTS:%d", c.depth, moves.size, c.pts);
  return true;
}



/***
 *
 *  To dump a traced graph, (with rejection info)
 *
 *  Do as follows:
 *  1. Calculate Paths
 *  2. Record <path, {maxpoint, enter_pts, is_rejected?}>
 *
 *  JS Side:
 *  1. Recover Edges by detection common string
 *  2. Calculate Rejection Version By DFS ending with node {path, maxpoint, enter_pts, is_rejected}
 *     and edge {path1, path2}
 *  3. Show edges
 *
 */



/**
 * Estimating the c.pts with c.depth, moves.size
 * @return c.pts's value
 */
INT estimate_func () {
  return -1;
}

/***
 *  Rejection Challenge Function
 * @param p
 * @param c
 * @param moves
 * @return
 */
static BOOL Backtrack_reject(REDUCTION_CTX &p, REDUCTION_NODE &c, REDUCTION_RESULT& moves){
  // no, go on
  if (c.pts <= estimate_func()) {
      printf("Rejecting D:%d, B:%d PTS:%d", c.depth, moves.size, c.pts);
      return true;
  }
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

  REDUCTION_RESULT moves = find_all_possible_swap(
    c.now,
    p.width,
    p.height);

  if (Backtrack_reject(p, c, moves)) {
    // free(c.now);
    // c.now = NULL;
    return c.pts;
  }
  if (Backtrack_accept(p, c)) {
    // Backtrack_record_output(p, c);
  }
  // Traverse kids
  // printf("Backtracking ... \n");

  INT64 total = moves.size;
  INT local_max = c.pts;
  if (total == 0) return local_max;
  for(INT i = 0; i < total; i++){
    // Calculate c'
    REDUCTION_NODE cprime = Generate_c_prime(p, c, moves, i);
    Is_True(cprime.pts >= c.pts, ("Error on generating cprime, got pts decreased\n"));
    INT somemax = Backtrack_finding(p, cprime);
    Is_True(somemax >= cprime.pts, ("Error on Backtrack finding, got pts decreased\n"));
    if(somemax > local_max) {
      local_max = somemax;
    }
    // ADD free.
    free(cprime.now);
  }

  INT local_delta_max = local_max - c.pts;
  printf("D:%d branch:%lld, c.pts: %d, max: %d, dmax: %d\n",
         c.depth, total, c.pts, local_max, local_delta_max);

  INT64 depth_num = (((UINT64) c.depth) << 32) + total;
  if (max_arriving_pts_depth_branch->count(depth_num) < 1) {
    max_arriving_pts_depth_branch->insert(std::make_pair(depth_num, c.pts));
    max_pts_per_depth_num_sum->insert(std::make_pair(depth_num, new vector<INT> ()));
    (*max_pts_per_depth_num_sum)[depth_num]->push_back(c.pts);
    return local_max;
  }
  (*max_pts_per_depth_num_sum)[depth_num]->push_back(c.pts);
  INT depth_num_max = (*max_arriving_pts_depth_branch)[depth_num];
  if (depth_num_max < c.pts) {
    (*max_arriving_pts_depth_branch)[depth_num] = c.pts;
  }
  return local_max;
}


INT Cross_retro_static (ELET *rand_values, ELET_OFST size, SPTR parentSpan) {
  AS_CHILD_SPAN(span, "Cross-static-whole", parentSpan);
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

  max_arriving_pts_depth_branch = new std::unordered_map<INT64, INT>;
  max_pts_per_depth_num_sum = new std::unordered_map<INT64, vector<INT> * >;

  Copy_map(copy, rand_values, width, height);
  refineNodes(copy, width, height, span);
  Backtrack_finding(*ctx, *node);
  printf("Dynamic Global max = %d, times = %d \n", global_max_pts, backtrack_time);

  auto it = max_arriving_pts_depth_branch->begin();
  INT *count = new INT[14 * 14];
  memset(count, 0 , sizeof(INT) * 14 * 14);
  for (; it != max_arriving_pts_depth_branch->end(); it++) {
    INT64 val = it->first;
    UINT depth = (UINT) (val >> 32);
    UINT branch = (UINT) (val & 0x7fffffff);
    if (depth < 14 && branch < 14) {
      //printf("D: %d, B: %d, C.PTS: %d\n", depth, branch, it->second);
      count[depth * 14 + branch] = it->second;
    }
  }

  puts("------------------------------------------------");
  puts("-D- -B- -MAX-");
  for (INT b = 0; b < 14; b++) {
    for (INT d = 0; d < 14; d ++) {
      printf("%3d,%3d,%3d,\n", d, b, count[d * 14 + b]);
    }
  }
  // Start of Reduction Process
  return 0;
}