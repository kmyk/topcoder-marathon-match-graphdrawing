#pragma GCC optimize "O3"
#pragma GCC target "avx"
#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <map>
#include <queue>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <random>
#include <chrono>
#include <cassert>
#define repeat(i,n) for (int i = 0; (i) < int(n); ++(i))
#define repeat_from(i,m,n) for (int i = (m); (i) < int(n); ++(i))
#define repeat_reverse(i,n) for (int i = (n)-1; (i) >= 0; --(i))
#define repeat_from_reverse(i,m,n) for (int i = (n)-1; (i) >= int(m); --(i))
#define whole(f,x,...) ([&](decltype((x)) whole) { return (f)(begin(whole), end(whole), ## __VA_ARGS__); })(x)
#define debug(x) #x << " = " << (x) << " "
using ll = long long;
using namespace std;
template <class T> inline void setmax(T & a, T const & b) { a = max(a, b); }
template <class T> inline void setmin(T & a, T const & b) { a = min(a, b); }
int sq(int x) { return x * x; }

class GraphDrawing { public: vector<int> plot(int, vector<int>); };

struct point_t {
    int y, x;
};
bool operator <  (point_t a, point_t b) { return a.y != b.y ? a.y < b.y : a.x < b.x; }
bool operator == (point_t a, point_t b) { return a.y == b.y and a.x == b.x; }
int dist_squared(point_t a, point_t b) { return sq(a.y - b.y) + sq(a.x - b.x); }

struct edge_t {
    int to;
    int dist;
};
bool operator <  (edge_t a, edge_t b) { return a.to != b.to ? a.to < b.to : a.dist < b.dist; }
bool operator == (edge_t a, edge_t b) { return a.to == b.to and a.dist == b.dist; }

double calculate_score(vector<point_t> const & p, vector<vector<edge_t> > const & g) {
    int n = p.size();
    double min_ratio_squared = + INFINITY;
    double max_ratio_squared = - INFINITY;
    repeat (i,n) {
        for (auto e : g[i]) {
            int j = e.to;
            double ratio_squared = dist_squared(p[i], p[j]) /(double) sq(e.dist);
            setmin(min_ratio_squared, ratio_squared);
            setmax(max_ratio_squared, ratio_squared);
        }
    }
    return sqrt(min_ratio_squared / max_ratio_squared);
}


template <class T>
bool is_distinct(vector<T> data) {
    whole(sort, data);
    return whole(unique, data) == data.end();
}

vector<point_t> solve(vector<vector<edge_t> > & g) {
    int n = g.size();
    vector<point_t> best;
    double best_score = - INFINITY;
    random_device device;
    default_random_engine gen(device());
    uniform_int_distribution<int> dist(0, 700);
    chrono::high_resolution_clock::time_point clock_begin = chrono::high_resolution_clock::now();
    while (true) {
        chrono::high_resolution_clock::time_point clock_end = chrono::high_resolution_clock::now();
        if (chrono::duration_cast<chrono::milliseconds>(clock_end - clock_begin).count() >= 2000) break;
        vector<point_t> p(n);
        repeat (i,n) {
            p[i].y = dist(gen);
            p[i].x = dist(gen);
        }
        double score = calculate_score(p, g);
        if (best_score < score) {
            if (not is_distinct(p)) continue;
            best_score = score;
            best = p;
        }
    }
    return best;
}

vector<int> GraphDrawing::plot(int n, vector<int> edges) {
    // unpack
    vector<vector<edge_t> > g(n);
    assert (edges.size() % 3 == 0);
    int m = edges.size() / 3;
    repeat (i,m) {
        int u    = edges[3*i];
        int v    = edges[3*i+1];
        int dist = edges[3*i+2];
        assert (0 <= u and u < n);
        assert (0 <= v and v < n);
        assert (1 <= dist and dist <= 991);
        g[u].push_back((edge_t) { v, dist });
        g[v].push_back((edge_t) { u, dist });
    }
    // solve
    vector<point_t> p = solve(g);
    // pack
    vector<int> result(2*n);
    repeat (i,n) {
        int y = p[i].y;
        int x = p[i].x;
        assert (0 <= y and y <= 700);
        assert (0 <= x and x <= 700);
        result[2*i]   = y;
        result[2*i+1] = x;
    }
    assert (is_distinct(p));
    return result;
}


// -------8<------- end of solution submitted to the website -------8<-------


int main() {
    GraphDrawing solver;
    int n; cin >> n;
    int e; cin >> e;
    vector<int> edges(e);
    repeat (i, e) cin >> edges[i];
    vector<int> result = solver.plot(n, edges);
    cout << result.size() << endl;
    repeat (i, 2*n) cout << result[i] << endl;
    cout.flush();
    return 0;
}

