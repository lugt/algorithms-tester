//
// Created by lugt on 2019/5/5.
//

#include "common_def.h"
#include "xiaoxiaole.h"
#include "xiaoxiao_util.h"

#include <list>

typedef std::list<INT> LINTLIST;
//std::unordered_map<ASSEMBLE_TOKEN, INT remain_count> all_map = 0;

INT global_max_pts = 0;
INT backtrack_time = 0;

REDUCTION_RESULT find_all_possible_swap(ELET *map, ELET_OFST wid, ELET_OFST height)
{
  REDUCTION_RESULT res;
  INT flag = 0;
  res.size = 0;
  for (int i = 0; i < height; ++i)
  {
    for (int j = 0; j < wid; ++j)
    {
      // printf ("[FIND_SWAP] checking on %d, %d \n", i, j);
      flag = swapAndJudge(map, i, j, wid, height); 
      if (flag & (4 | 8))
      {
	res.x1[res.size] = i;
	res.y1[res.size] = j;
	res.x2[res.size] = i + 1;
	res.y2[res.size] = j;
  	res.size ++;
        // printf("[FIND_SWAP] possible swap : (%d, %d, DOWN) \n", i, j);
      } else if (flag & (16 | 32)) {
	res.x1[res.size] = i;
	res.y1[res.size] = j;
	res.x2[res.size] = i;
	res.y2[res.size] = j + 1;
  	res.size ++;
        //printf("[FIND_SWAP] possible swap : (%d, %d, RIGHT) \n", i, j);
      }
    }
  }
  return res;
}

inline void Copy_map(ELET *targ, ELET *src, ELET_OFST w, ELET_OFST h) {
  memcpy(targ, src, w * h * sizeof (ELET));
}

BOOL Backtrack_reject(REDUCTION_CTX &p, REDUCTION_NODE &c){
  // no, go on
  return false;
}

BOOL Backtrack_accept(REDUCTION_CTX &p, REDUCTION_NODE &c){
  return true;
}

void Backtrack_record_output (REDUCTION_CTX &p, REDUCTION_NODE &c) {
  cout << "Found result on somewhere" << endl;
  pmap(c.now, p.width, p.height);
}


inline ELET & Get_position(ELET *map, ELET_OFST l, ELET_OFST r, ELET_OFST wid) {
  return map[l * wid + r];
}


REDUCTION_NODE Generate_c_prime(REDUCTION_CTX &p, REDUCTION_NODE &c,
				REDUCTION_RESULT & moves, INT num){
  INT width = p.width;
  INT height = p.height;
  ELET_OFST       len       = width * height;
  ELET * copy = (ELET *) malloc (len * sizeof(ELET));
  Copy_map(copy, c.now, width, height);
  REDUCTION_NODE pNode;
  pNode.now = copy;
  pNode.pts = c.pts;
  pNode.depth = c.depth + 1;
  INT cx1 = moves.x1[num];
  INT cx2 = moves.x2[num];
  INT cy1 = moves.y1[num];
  INT cy2 = moves.y2[num];
  // Eliminate:
  // printf("Exchanging Step %d ... (%d, %d) with (%d, %d) \n", num, cx1, cy1, cx2, cy2);
  std::swap (Get_position(copy, cx1, cy1, width), Get_position(copy, cx2, cy2, width));
  // puts("Exchanged.\n");
  INT cnt = Eliminate_nodes(copy, cx1, cy1, cx2, cy2, width, height);
  pNode.pts += cnt;
  //  printf("New cnode : %d \n", pNode.pts);
  if (pNode.pts > global_max_pts){
    global_max_pts = pNode.pts;
  }
  return pNode;
}


REDUCTION_NODE Generate_c_prime(REDUCTION_CTX &p, REDUCTION_NODE &c,
				REDUCTION_RESULT & moves, INT num, SPTR parent){
  AS_CHILD_SPAN(span, "generate-c-prime", parent);
  return Generate_c_prime(p, c, moves, num);
}

INT Backtrack_finding(REDUCTION_CTX &p, REDUCTION_NODE &c, SPTR parentSpan) {
  AS_CHILD_SPAN (span, "backtrack-find", parentSpan);
  
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
  AS_CHILD_SPAN(possible_swap, "possible-swap", span);
  REDUCTION_RESULT moves = find_all_possible_swap(
    c.now,
    p.width,
    p.height);

  possible_swap->Finish();
  
  INT64 total = moves.size;
  //  printf("D:%d sub: %lld\n", c.depth, total);
  std::ostringstream oss;
  oss << "depth : " << c.depth << ", max : " << total;
  span->SetBaggageItem("p", oss.str());

  AS_CHILD_SPAN(all_kids, "traverse-kids", span);
  if (total == 0) return 0;
  if (c.depth < 1) {
    for(INT i = 0; i < total; i++){
      // Calculate c'
      REDUCTION_NODE cprime = Generate_c_prime(p, c, moves, i, span);   
      Backtrack_finding(p, cprime, span);
      // ADD free.
      free(cprime.now);
    }
  } else {
    for(INT i = 0; i < total; i++){
      // Calculate c'
      REDUCTION_NODE cprime = Generate_c_prime(p, c, moves, i);   
      Backtrack_finding(p, cprime);
      // ADD free.
      free(cprime.now);
    }

  }
  // Free of data
  // puts("Backtracking finish ...");
  return total;
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
INT Backtrack_finding(REDUCTION_CTX &p, REDUCTION_NODE &c) {
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
  printf("D:%d sub: %lld\n", c.depth, total);
  if (total == 0) return 0;
  for(INT i = 0; i < total; i++){
    // Calculate c'
    REDUCTION_NODE cprime = Generate_c_prime(p, c, moves, i);
    Backtrack_finding(p, cprime);
    // ADD free.
    free(cprime.now);
  }
  // Free of data
  // puts("Backtracking finish ...");
  return total;
}

CROSS_NODE *Create_rectangle(INT width, INT height) {
  return (CROSS_NODE *) Cross_malloc(width * height * sizeof(CROSS_NODE));
}


//=============================================================================
//
//  Memory Status Save
//
//=============================================================================
void Cross_memory_push() {
  // Create a queue
}

//=============================================================================
//
//  Memory Status Restore
//
//=============================================================================
void Cross_memory_pop() {
  // free();
}

//=============================================================================
//
//  Memory Allocate New Portion of memory
//
//=============================================================================
void *Cross_malloc(INT size) {
  return malloc(size);
}


bool refineNodes(ELET *m, ELET_OFST wid, ELET_OFST height, SPTR parentSpan) {
  AS_CHILD_SPAN(span, "refine-node", parentSpan);
  return refineNodes(m, wid, height);
}


INT Cross_retro (ELET *rand_values, ELET_OFST size, SPTR parentSpan) {
  AS_CHILD_SPAN(span, "Cross-retro-whole", parentSpan);
  // INT64 correct_size = size >> 8;
  // Generate Random Assemble
  // +Refine Assemble
  INT width   = size / 2;
  INT height  = size;
  ELET_OFST       len       = width * height;
  REDUCTION_CTX  *ctx       = new REDUCTION_CTX;
  ELET           *copy      = (ELET *) malloc(len * sizeof(ELET));
  REDUCTION_NODE *node      = new REDUCTION_NODE;
  ctx->value                = rand_values;
  ctx->width                = width;
  ctx->height               = height;
  node->now                 = copy;
  global_max_pts            = 0;
  Copy_map(copy, rand_values, width, height);
  refineNodes(copy, width, height, span);
  Backtrack_finding(*ctx, *node, span);
  printf("Global max = %d, times = %d \n", global_max_pts, backtrack_time);
  // Start of Reduction Process
  return 0;
}


INT Cross_graph (ELET *rand_values, ELET_OFST size, SPTR parentSpan) {
  AS_CHILD_SPAN(span, "Cross-graph-whole", parentSpan);
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
  Copy_map(copy, rand_values, width, height);
  Backtrack_finding(*ctx, *node);
  // Start of Reduction Process
  return 0;
}
