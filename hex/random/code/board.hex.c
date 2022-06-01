#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "board.hex.h"

const int Offsets[6] = { -Np2, 1, N, 2, N, 1 };

void init_locations(int L[]) {int j;
  for (j=0; j<Nsq; j++)   L[j] = fatten(j);
}

//shuffle a vector
void shuffle(int X[Nsq]) {
  int j,k,tmp;
  for (k = Nsq-1; k > 0; k--) {
    j = myrand(k+1);  //printf("rand %d\n", j);
    tmp = X[j]; X[j] = X[k]; X[k] = tmp;
  }
}

//shuffle a fat vector
void fatShuffle(int X[Np2sq]) {
  int j,k,tmp;
  for (k = Nsq-1; k > 0; k--) {
    j = myrand(k+1);  //printf("rand %d\n", j);
    tmp = X[fatten(j)]; X[fatten(j)] = X[fatten(k)]; X[fatten(k)] = tmp;
  }
}

// display vector as fat/guarded hex board
void showAsFatBoard(int X[]) { int j,k;
  for (j = 0; j < Np2; j++) {
    for (k = 0; k < j; k++) 
      printf(" ");
    for (k = 0; k < Np2; k++) 
      printf("%2d", X[j*Np2+k]);
    printf("\n");
  }
}

void showAsBoard(int X[]) { int j,k;
  for (j = 1; j < Np1; j++) {
    for (k = 1; k < j; k++) 
      printf(" ");
    for (k = 1; k < Np1; k++) 
      printf("%2d", X[j*Np2+k]);
    printf("\n");
  }
}

void showUnguarded(int X[]) { int j,k;
  for (j = 0; j < N; j++) {
    for (k = 0; k < j; k++) 
      printf(" ");
    for (k = 0; k < N; k++) 
      printf("%d ", X[j*N+k]);
    printf("\n");
  }
}

void showForGnuplot(int X[], int s) { int j;
  for (j = N; j <= K; j++) 
    printf("%d %2.5f \n", 2*j-1, X[j]*1.0/(s*1.0));
}

void dfs(int M[], int fatV[], int v){ int t;
  M[v]=1; // mark v
  for (t=0; t<6; t++) {
    v += Offsets[t];
    if ((BLACK==fatV[v]) && (!M[v]))
      dfs(M,fatV,v);
  }
}

int UFConnect(int fatV[], int Par[Np2sq]){ int j,t,v;
  for (j = Np2+1; j < Np2sq-Np2; j++) {
    if (BLACK==fatV[j]) {
      v =j;
      for (t=0; t<6; t++) {
        v += Offsets[t];
        if (BLACK==fatV[v])
          myUnion(Par,j,v);
      }
    }
  }
  //showAsFatBoard(Par); printf("\n");
  return (myFind(Par,FIRST_TOP) == myFind(Par,LAST_BTM)); 
}

int dfsConnect(int fatV[]){ int Mark[Np2sq]; int j;
  for (j = 0; j < Np2sq; j++) Mark[j]=0;    
  for (j = Np3; j < N+Np3; j++) // search from first (non-guard) row
    if ((BLACK==fatV[j]) && (!Mark[j])) 
      dfs(Mark,fatV,j);
  for (j = Np1*Np1; j<Np1*Np1+N; j++) 
    if (Mark[j]) return 1;  // any last row cells marked?
  return 0;
}

void init_guard_brd(int fV[Np2sq]) { int j;
  for (j=0; j<Np2sq; j++) 
    fV[j] = EMPTY;
}

void put_black_stones(int fV[Np2sq]) { int k;
  for (k=0; k<K; k++) fV[fatten(k)] = BLACK;
}

void init_UF_guard_brd(int fV[Np2sq], int P[Np2sq]) { int j;
  for (j=0; j<Np2sq; j++) 
    fV[j] = EMPTY;
  for (j=1; j<Np1; j++) { // top/bottom guard rows black
    fV[j] = BLACK;
    fV[j+Np2*Np1] = BLACK;
  }
  for (j = 0; j < Np2sq; j++) P[j]=j;    
  for (j = 1; j < Np1; j++) P[j] = FIRST_TOP; // connect top border
  for (j = Np2*Np1+1; j < Np2sq-1; j++) P[j] = LAST_BTM; // " btm "
}

int myFind(int Parents[Np2sq], int x) {
  int px = Parents[x];
  if (x==px) return x;
  int gx = Parents[px];
  if (px==gx) return px;
  Parents[x] = gx; //printf("find parent %d <- %d\n",x,gx);
  return myFind(Parents, gx);
}

void myUnion(int Parents[Np2sq], int x, int y) { 
  //printf("union %d %d\n",x,y);
  int xRoot = myFind(Parents, x);
  int yRoot = myFind(Parents, y);
  Parents[xRoot] = yRoot; //printf("parent %d <- %d\n",xRoot,yRoot);
}
