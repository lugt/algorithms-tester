//
// Created by xc5 on 2019-05-05.
//
#include "xiaoxiao_util.h"

using namespace std;


#define CHECK_BELOW_X_3(len, is, js, map, wid) ((is >= 0) && (is + 2) < len \
    && Get_position(map, is, js, wid) != 0 \
    && Get_position(map, is, js, wid) == Get_position(map, is + 1, js, wid)  \
    && Get_position(map, (is + 1), js, wid) == Get_position(map, is + 2, js, wid))


#define CHECK_BELOW_Y_3(len, is, js, map, wid) ((js >= 0) && ((js + 2) < len) \
    && Get_position(map, is, js, wid) != 0 \
    && Get_position(map, is, js, wid) == Get_position(map, is, js + 1, wid)  \
    && Get_position(map, is, js + 1, wid) == Get_position(map, is, js + 2, wid))

inline ELET & Get_position(ELET *map, ELET_OFST l, ELET_OFST r, ELET_OFST wid) {
  return map[(l * wid) + r];
}

void pmap(ELET *map, ELET_OFST wid, ELET_OFST height)
{
  cout << " ---  PMAP:  ---" << endl;
  for (INT i = 0; i < height; ++i)
  {
    for (INT j = 0; j < wid; ++j)
    {
      cout << (ELET) Get_position(map, i, j, wid) << " ";
    }
    cout << endl;
  }
}

BOOL check_elimination(ELET *map, ELET_OFST hei, ELET_OFST wid, ELET_OFST i, ELET_OFST j)
{
  /* i,j elimination
     i-1>=0 && i+1<len && map[i-1][j]==map[i][j]&&map[i][j]==map[i+1][j])
    || (j-1>=0 && j+1<len && map[i][j-1]==map[i][j]&&map[i][j]==map[i][j+1])
    || (i-2>=0 && map[i-2][j]==map[i-1][j]&&map[i-1][j]==map[i][j])
    || (j-2>=0 && map[i][j-2]==map[i][j-1]&&map[i][j-1]==map[i][j])
    || (i+2<len && map[i+2][j]==map[i+1][j]&&map[i+1][j]==map[i][j])
    || (j+2<len && map[i][j+2]==map[i][j+1]&&map[i][j+1]==map[i][j]
   */
  if (CHECK_BELOW_X_3(hei, i - 2, j, map, wid)) {
    return true;
  }
  if (CHECK_BELOW_X_3(hei, i - 1, j, map, wid)) {
    return true;
  }
  if (CHECK_BELOW_X_3(hei, i, j, map, wid)) {
    return true;
  }
  if (CHECK_BELOW_Y_3(wid, i, j - 2, map, wid)) {
    return true;
  }
  if (CHECK_BELOW_Y_3(wid, i, j - 1, map, wid)) {
    return true;
  }
  if (CHECK_BELOW_Y_3(wid, i, j, map, wid)) {
    return true;
  }
  return false;
  /*return CHECK_BELOW_X_3(len, i - 2, j, map, wid) ||
         CHECK_BELOW_X_3(len, i - 1, j, map, wid) ||
         CHECK_BELOW_X_3(len, i, j, map, wid) ||
         CHECK_BELOW_Y_3(len, i, j - 2, map, wid) ||
         CHECK_BELOW_Y_3(len, i, j - 1, map, wid) ||
         CHECK_BELOW_Y_3(len, i, j, map, wid);*/
}

BOOL Refine_single_node(ELET *map, ELET_OFST hei, ELET_OFST wid, ELET_OFST i, ELET_OFST j)
{
  /* i,j elimination
     i-1>=0 && i+1<len && map[i-1][j]==map[i][j]&&map[i][j]==map[i+1][j])
    || (j-1>=0 && j+1<len && map[i][j-1]==map[i][j]&&map[i][j]==map[i][j+1])
    || (i-2>=0 && map[i-2][j]==map[i-1][j]&&map[i-1][j]==map[i][j])
    || (j-2>=0 && map[i][j-2]==map[i][j-1]&&map[i][j-1]==map[i][j])
    || (i+2<len && map[i+2][j]==map[i+1][j]&&map[i+1][j]==map[i][j])
    || (j+2<len && map[i][j+2]==map[i][j+1]&&map[i][j+1]==map[i][j]
   */
  // Change this node, not 
  if(    CHECK_BELOW_X_3(hei, i - 2, j, map, wid) ||
         CHECK_BELOW_X_3(hei, i - 1, j, map, wid) ||
         CHECK_BELOW_X_3(hei, i, j, map, wid) ||
         CHECK_BELOW_Y_3(wid, i, j - 2, map, wid) ||
         CHECK_BELOW_Y_3(wid, i, j - 1, map, wid) ||
         CHECK_BELOW_Y_3(wid, i, j, map, wid) ) {
    Get_position(map, i, j, wid) = ((Get_position(map, i, j, wid) + 1) % DISTRIBUTE_MAX) + 1;
    return true;
  }
  return false;
}

INT cnt = 0;

void Elim_remove(ELET *map, CHPTR state, INT x, int y, INT hei, INT wid, ELET target)
{
  if (!( x >= 0 && x < wid && y >= 0 && y < hei)) return;
  if (state [x + wid * y] != 0) return;
  if (map [x + wid * y] != target) return;
    state[x + wid * y] = 1;
    cnt++;
    Elim_remove(map, state, x + 1, y, hei, wid, target);
    Elim_remove(map, state, x - 1, y, hei, wid, target);
    Elim_remove(map, state, x, y + 1, hei, wid, target);
    Elim_remove(map, state, x, y - 1, hei, wid, target);
}

void Elim_adjustment(ELET *map, CHPTR state, INT wid, INT hei)
{
    for (int j = 0; j < wid; j++)
    {
        std::vector<int> tmp;
        for (int i = hei - 1; i >= 0; --i)
        {
            if (!state[i * wid + j]) tmp.push_back( map[i * wid + j] );

        }
        int r = hei - 1;
        for (int i = 0; i < tmp.size(); i++)
        {
            map[r * wid + j] = tmp[i];
            state[r * wid + j] = 0;
            r--;
        }
        for (; r >= 0; r--)
        {
            state[r * wid + j] = 1;
	    map [r * wid + j] = 0;
        }
    }
    // puts("Adjusted situation");
    // pmap(map, wid, hei);
}

INT  Eliminate_nodes(ELET *map, INT cx1, INT cy1, INT cx2, INT cy2,
		     INT wid, INT hei) {
  // To-cancel
  INT targ1 = Get_position(map, cx1, cy1, wid) ;
  INT targ2 = Get_position(map, cx2, cy2, wid) ;
  INT len = hei * wid;
  INT solved = 0;
  CHPTR state = new char[len];
  memset(state, 0, sizeof(char) * len);
  cnt = 0;
  if (check_elimination(map, hei, wid, cx1, cy1)) {
    Elim_remove(map, state, cy1, cx1, hei, wid, targ1);
  }
  if (check_elimination(map, hei, wid, cx2, cy2)) {
    Elim_remove(map, state, cy2, cx2, hei, wid, targ2);
  }
  solved = cnt;
  // Got cnt removed;
  Elim_adjustment(map, state, wid, hei);
  // Post - adjustment recheck
  memset(state, 0, sizeof(char) * len);
  INT flag = 0;
  do {
    flag = 0;
    for (INT ii = 0; ii < hei && flag == 0; ii++) {
      for (INT jj = 0; jj < wid && flag == 0; jj++) {
	cnt = 0;
	BOOL cpt = false;
	if ((cpt = check_elimination(map, hei, wid, ii, jj))) {
	  flag += 1;
	  targ1 = Get_position(map, ii, jj, wid);
	  /*printf("Auto-removing the block (%d, %d), cnt = %d, cpt = %d \n"
		 "targ1 = %d \n",
		 ii, jj, cnt, (INT) cpt, targ1);*/
	  // pmap(map, wid, hei);
	  cnt = 0;
	  Elim_remove(map, state, jj, ii, hei, wid, targ1);
	  Elim_adjustment(map, state, wid, hei);
	  // printf("Removed block = %d \n", cnt);
	  memset(state, 0, sizeof(char) * len);
	  break;
	}
      }
    }
  } while((/*printf("Re-adjusting map with flag = %d \n", flag), */ flag));
  delete[] state;
  return solved;
}


INT swapAndJudge(ELET *m, INT i, INT j, ELET_OFST wid, ELET_OFST hei)
// 保证i、j不越界, 应该对被swap的两个点都做纵向和横向的检查
{
  ELET      *map = m;
  INT        ret = 0;
  /*
  ELET      *map = malloc(sizeof(ELET) *wid * height);
  for (INT ii = 0; ii < height; ++ii)
  {
    for (INT jj = 0; jj < wid; ++jj)
    {
      Get_position(map, ii, jj, wid) = Get_position(m, ii, jj, wid);
    }
    }*/
  // 原来就可以消除
  if (check_elimination(map, hei, wid, i, j))
  {
    printf("[ERROR] no need to swap at (%d, %d)\n", i, j);
    pmap(map, wid, hei);
    exit(1);
    return 1;
  }
  // 只需要向下换和向右换
  // 向下换
  if (i + 1 < hei)
  {
    swap(Get_position(map, i+1, j, wid), Get_position(map, i, j, wid));

    if (check_elimination(map, hei, wid, i, j))
    {
      //printf("# swap and sweap! (%d, %d)\n", i, j);
      ret |= 4;
    }
    if (check_elimination(map, hei, wid, i+1, j))
    {
      //printf("# swap and sweap! (%d, %d)\n", i+1, j);
      ret |= 8;
    }

    // Swap back
    swap(Get_position(map, i+1, j, wid), Get_position(map, i, j, wid));
  }

  // 向右换
  if (j + 1 < wid)
  {
    // In - place revert of swapping
    swap(Get_position(map, i, j + 1, wid), Get_position(map, i, j, wid));
    if (check_elimination(map, hei, wid, i, j))
    {
      //printf("# swap and sweap! (%d, %d)\n", i, j);
      ret |= 16;
    }
    if (check_elimination(map, hei, wid, i, j+1))
    {
      //printf("# swap and sweap! (%d, %d)\n", i, j+1);
      ret |= 32;
    }
    swap(Get_position(map, i, j + 1, wid), Get_position(map, i, j, wid));
    // 换回来
  }

  return ret;

}

bool refineNodes(ELET *m, ELET_OFST wid, ELET_OFST height)
{
  //  ELET_OFST  len = wid * height;
  ELET      *map = m;
  INT        flag = 0;
  do {
    flag = 0;
    for (INT ii = 0; ii < height; ++ii)
      {
	for (INT jj = 0; jj < wid; ++jj)
	  {
	    if (Refine_single_node(map, height, wid, ii, jj))
	      {
		printf(" refining (%d, %d) ... ", ii, jj);
		// OK, refine the five nodes.
		flag++;
	      }
	  }
      }
  } while((printf(" ++ Upon refining, got %d \n", flag), flag > 0));
  return true;
}



void findMinSwap(ELET *map, ELET_OFST wid, ELET_OFST height)
{
  for (int i = 0; i < height; ++i)
  {
    for (int j = 0; j < wid; ++j)
    {
      if (swapAndJudge(map, i, j, wid, height))
      {
        printf("gotcha! (%d, %d)\n", i, j);
      }
    }
  }
}

INT unused_main_xiaoxiao_util(INT argc, const char * argv[]) {
  // insert code here...
//    std::cout << "Hello, World!\n";
  ELET_OFST  wid    = 10;
  ELET_OFST  height = 10;
  ELET      *map    = new ELET[wid * height];
  srand(unsigned(time(0)));

  for (INT i = 0; i < height; ++i)
  {
    for (INT j = 0; j < wid; ++j)
    {
      map[i * wid + j] = rand() % 5;
    }
  }
  cout << "xiaoxiaole!\n";
  findMinSwap(map, wid, height);
  pmap(map, wid, height);
  return 0;
}
