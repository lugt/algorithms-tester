//
// Created by xc5 on 2019-05-05.
//

#ifndef ALGORITHMS_XIAOXIAO_UTIL_H
#define ALGORITHMS_XIAOXIAO_UTIL_H
#include "common_def.h"

EXTERN inline ELET & Get_position(ELET *map, ELET_OFST l, ELET_OFST r, ELET_OFST wid);

EXTERN void pmap(ELET *map, ELET_OFST wid, ELET_OFST height);

EXTERN BOOL check_elimination(ELET *map, ELET_OFST len, ELET_OFST wid, ELET_OFST i, ELET_OFST j);

EXTERN INT swapAndJudge(ELET *m, INT i, INT j, ELET_OFST wid, ELET_OFST height);

EXTERN void findMinSwap(ELET *map, ELET_OFST wid, ELET_OFST height);

EXTERN bool refineNodes(ELET *m, ELET_OFST wid, ELET_OFST height);

EXTERN BOOL Refine_single_node(ELET *map, ELET_OFST len, ELET_OFST wid, ELET_OFST i, ELET_OFST j);

EXTERN INT  Eliminate_nodes(ELET *map, INT cx1, INT cy1, INT cx2, INT cy2,
			    INT wid, INT hei);

EXTERN INT unused_main_xiaoxiao_util(INT argc, const char * argv[]);

#endif //ALGORITHMS_XIAOXIAO_UTIL_H
