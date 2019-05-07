//
// Created by lugt on 2019/5/5.
//

#ifndef GRAPH_XIAOXIAOLE_H
#define GRAPH_XIAOXIAOLE_H

#include "common_def.h"

#define ASSEMBLE_ETHEREAL 0

typedef struct Move_t {

  INT   depth = 0;
  CHPTR assemble_hash;
  INT   move_left;
  INT   move_right;

} MOVE;

typedef std::stack<MOVE> PATHSTACK;

typedef struct Assemble_t {

} ASSEMBLE;

typedef struct Reduction_t {
  INT x1[100];
  INT y1[100];
  INT x2[100];
  INT y2[100];
  INT size = 0;
} REDUCTION_RESULT;

typedef struct Reduction_node_t {
  ELET       *now = (ELET *) 0x3f3f3f3f3f3f3f3f;
  INT32       pts = 0;
  INT         depth = 0;
} REDUCTION_NODE;

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

using boost::graph_traits;
using boost::adjacency_list;
using boost::vecS;
using boost::bidirectionalS;
using boost::concepts::EdgeListGraph;
using boost::concepts::EdgeListGraphConcept;

typedef adjacency_list<vecS, vecS, bidirectionalS> Graph;

/*******************************************************************
 *
 *  Reduction Context
 *  Origin Assemble, Path, History Score
 *
 ******************************************************************/
typedef struct Reduction_ctx_t {
  // Tree info
  INT64      history_pts;
  ELET       *value         = NULL;
  ELET_OFST   width         = 4;
  ELET_OFST   height        = 8;
  INT         color_count   = 3;
} REDUCTION_CTX;

typedef INT    CROSS_NODE;

EXTERN INT main (INT args, CHPPTR argv);
EXTERN INT Cross_retro (ELET *values, ELET_OFST size, SPTR parentSpan);
EXTERN INT Cross_graph (ELET *values, ELET_OFST size, SPTR parentSpan);
EXTERN void *Cross_malloc(INT size);
EXTERN REDUCTION_RESULT Reduce_assemble(ASSEMBLE *assemble);
EXTERN CROSS_NODE *Create_rectangle(INT width, INT height);
EXTERN void Cross_memory_push();
EXTERN void Cross_memory_pop();

EXTERN INT  Backtrack_finding (REDUCTION_CTX &p, REDUCTION_NODE &c);
EXTERN REDUCTION_NODE Generate_c_prime (REDUCTION_CTX &p, REDUCTION_NODE &c, REDUCTION_RESULT & moves, INT num);

#endif //GRAPH_XIAOXIAOLE_H
