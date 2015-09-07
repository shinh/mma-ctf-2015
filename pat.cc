#include <stdio.h>

#include <algorithm>
#include <queue>
#include <vector>

using namespace std;

static const int N = 3;

int POS(int x, int y) {
  return x + y * N;
}

int X(int p) {
  return p % N;
}

int Y(int p) {
  return p / N;
}

int sign(int v) {
  return v > 0 ? 1 : v < 0 ? -1 : 0;
}

struct State {
  int pos;
  vector<int> route;

  bool has(int i) const {
    return find(route.begin(), route.end(), i) != route.end();
  }
};

int main() {
  queue<State> q;
  for (int i = 0; i < N * N; i++) {
    State st;
    st.pos = i;
    st.route.push_back(i);
    q.push(st);
  }

  int cnt = 0;
  while (!q.empty()) {
    State st = q.front();
    q.pop();
    if (st.route.size() >= 4)
      cnt++;

    int px = X(st.pos);
    int py = Y(st.pos);
    for (int i = 0; i < N * N; i++) {
      int x = X(i);
      int y = Y(i);
      int sx = x - px;
      int sy = y - py;
      int dx = abs(sx);
      int dy = abs(sy);

      if (dx == 0 && dy == 0)
        continue;

      if (dx == 0 && dy != 1 ||
          dy == 0 && dx != 1 ||
          dx == dy && dx != 1) {
        bool ok = true;
        int cx = px;
        int cy = py;
        while (true) {
          cx += sign(sx);
          cy += sign(sy);
          if (cx == x && cy == y)
            break;
          if (!st.has(POS(cx, cy))) {
            ok = false;
            break;
          }
        }
        if (!ok)
          continue;
      }

      if (st.has(i))
        continue;

      State nst = st;
      nst.pos = i;
      nst.route.push_back(i);
      q.push(nst);
    }
  }

  printf("%d\n", cnt);
}
