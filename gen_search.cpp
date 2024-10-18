#include <bits/stdc++.h>
using namespace std;

#define int long long

signed main() {
  ios::sync_with_stdio(false); cin.tie(nullptr);

  int from = 1e12;
  int to = 2e12;
  int step = 1e11;

  for (int i = from; i < to; i+=step) {
    cout << "./launch.sh " << i << ' ' << i+(long long)step << endl;
  }
}
