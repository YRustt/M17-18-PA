#include <fstream>
#include <iostream>
#include <random>
#include <utility>
#include <climits>
#include <cmath>
#include <vector>
#include <stack>
#include <map>

//#define DEBUG
#define COUNT_ITER 100


std::vector<std::vector<int>> graph;
std::vector<std::vector<int>> graph_reverse;
std::vector<std::vector<int>> graph_condensation;
std::vector<int> order, components, count_v;
std::vector<double> rands, sums;
std::vector<bool> used;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

long long MAX_N = INT_MAX;

void step1(int v) {
    used[v] = true;
    for (size_t i = 0; i < graph[v].size(); ++i) {
        if (!used[graph[v][i]]) {
            step1(graph[v][i]);
        }
    }
    order.push_back(v);
}

void step2(int v, int idx) {
    used[v] = true;
    components[v] = idx;
    for (size_t i = 0; i < graph_reverse[v].size(); ++i) {
        if (!used[graph_reverse[v][i]]) {
            step2(graph_reverse[v][i], idx);
        }
    }
}


int main() {
    std::ifstream fin("reachability.in");

    int n, m, v, u;
    fin >> n >> m;

    graph = std::vector<std::vector<int>> (n, std::vector<int>());
    graph_reverse = std::vector<std::vector<int>> (n, std::vector<int>());
    components = std::vector<int> (n, -1);

    for (size_t i = 0; i < m; ++i) {
        fin >> v >> u;
        graph[v - 1].push_back(u - 1);
        graph_reverse[u - 1].push_back(v - 1);
    }

    used = std::vector<bool>(n, false);
    for (size_t i = 0; i < n; ++i) {
        if (!used[i]) {
            step1(i);
        }
    }

    used = std::vector<bool>(n, false);
    int idx = 0;
    for (size_t i = 0; i < n; ++i) {
        int v = order[n - 1 - i];
        if (!used[v]) {
            step2(v, idx);
            idx++;
        }
    }

#ifdef DEBUG
    std::cout << "<components>\n";
    for (size_t i = 0; i < n; ++i) {
        std::cout << i << " " << components[i] << std::endl;
    }
    std::cout << "</componenets>\n";
#endif

    graph_condensation = std::vector<std::vector<int>> (idx, std::vector<int>());
    for (size_t u = 0; u < n; ++u) {
        for (size_t i = 0; i < graph[u].size(); ++i) {
            if (components[u] != components[graph[u][i]]) {
                graph_condensation[components[u]].push_back(components[graph[u][i]]);
            }
        }
    }

#ifdef DEBUG
    std::cout << "<graph_condensation>\n";
    for (size_t u = 0; u < idx; ++u) {
        std::cout << u << ": ";
        for (size_t i = 0; i < graph_condensation[u].size(); ++i) {
            std::cout << graph_condensation[u][i] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "</graph_condenstation>\n";
#endif

    count_v = std::vector<int> (idx, 0);
    for (size_t i = 0; i < n; ++i) {
        count_v[components[i]] += 1;
    }

#ifdef DEBUG
    for (size_t i = 0; i < n; ++i) {
        std::cout << i << " " << components[i] << " " << count_v[components[i]] << std::endl;
    }
#endif

    rands = std::vector<double> (idx, 0);
    sums = std::vector<double> (idx, 0.0);
    double r, min_;
    for (size_t iter = 1; iter < COUNT_ITER; ++iter) {
        for (int i = 0; i < idx; ++i) {
            r = 1.0;
            for (int j = 0; j < count_v[i]; ++j) {
                r = std::min(r, dis(gen));
            }
            rands[i] = r;
        }

        for (int i = idx - 1; i >= 0; --i) {
            min_ = rands[i];
            for (int j = 0; j < graph_condensation[i].size(); ++j) {
                min_ = std::min(min_, rands[graph_condensation[i][j]]);
            }
            rands[i] = min_;

            sums[i] += std::log(1. - rands[i]);
        }
    }

    for (int i = 0; i < idx; ++i) {
#ifdef DEBUG
        std::cout << i << " " << -COUNT_ITER / sums[i] << std::endl;
#endif
        count_v[i] = std::max(std::min((int) (-(COUNT_ITER / sums[i]) + 1), n), 1);
    }

#ifdef DEBUG
    for (int i = 0; i < n; ++i) {
        std::cout << i + 1 << " " << components[i] << " " << count_v[components[i]] << std::endl;
    }
#endif

    std::ofstream fout("reachability.out");
    for (int i = 0; i < n; ++i) {
        fout << count_v[components[i]] << ((i != n - 1)? "\n": "");
    }

    return 0;
}