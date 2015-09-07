#include <math.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <map>
#include <queue>
#include <set>
#include <vector>

using namespace std;

static const int N = 4;

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
  int done;
  double len;

  State() {
    pos = -1;
    done = 0;
    len = 0;
  }

  void set(int p) {
    pos = p;
    done |= 1 << p;
  }

  bool has(int i) const {
    return (done >> i) & 1;
  }

  int id() const {
    return done * 16 + pos;
  }
};

int main(int argc, char* argv[]) {
  queue<State> q;

  {
    int i = atoi(argv[1]);
    State st;
    st.set(i);
    q.push(st);
  }

  map<int, double> done;

  double max_len = 0;
  int cnt = 0;
  while (!q.empty()) {
    State st = q.front();
    q.pop();

    auto p = done.emplace(st.id(), st.len);
    if (!p.second) {
      if (p.first->second > st.len)
        continue;
      p.first->second = st.len;
    }

    if (max_len < st.len) {
      max_len = st.len;
      fprintf(stderr, "max=%f cnt=%d\n", max_len, cnt);
    }
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

      State nst(st);
      nst.set(i);
      nst.len += sqrt(dx * dx + dy * dy);
      q.push(nst);
    }
  }

  printf("%d\n", cnt);
}
