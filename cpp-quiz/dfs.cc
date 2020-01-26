#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

struct Node {
  int arrival;
  int departure;
};

static int count = 0;

void dfs(unordered_map<int, vector<int>> &g, unordered_map<int, Node> &node,
         int s) {
  auto it = g.find(s);
  if (it != end(g)) {
    node[s].arrival = count++;
    for (const auto n : it->second) {
      dfs(g, node, n);
    }
    node[s].departure = count++;
  }
};

int main() {
  unordered_map<int, vector<int>> g;
  g[0] = {1, 2};
  g[1] = {};
  g[2] = {3, 4};
  g[3] = {5};
  g[4] = {};
  g[5] = {};
  g[6] = {7};
  g[7] = {};

  unordered_map<int, Node> node;
  count = 0;

  dfs(g, node, 0);
  dfs(g, node, 6);

  for (const auto &n : node) {
    cout << n.first << " => " << n.second.arrival << " / " << n.second.departure
         << endl;
  }
}
