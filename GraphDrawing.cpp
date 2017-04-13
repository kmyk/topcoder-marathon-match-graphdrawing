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
int clamp(int a, int l, int r) { return min(max(a, l), r); } // [l, r]
constexpr double eps = 1e-10;

class GraphDrawing { public: vector<int> plot(int, vector<int>); };

struct point_t {
    int y, x;
};
bool operator <  (point_t a, point_t b) { return a.y != b.y ? a.y < b.y : a.x < b.x; }
bool operator == (point_t a, point_t b) { return a.y == b.y and a.x == b.x; }
int dist_squared(point_t a, point_t b) { return sq(a.y - b.y) + sq(a.x - b.x); }

struct edge_t {
    int from, to;
    int dist;
};

constexpr int length_min = 1;
constexpr int length_max = 991; // inclusive
constexpr int position_min = 0;
constexpr int position_max = 700; // inclusive

pair<double, double> calculate_updated_ratio_squared(vector<point_t> const & p, int i, point_t p_i, vector<edge_t> const & edges, vector<vector<int> > const & g) { // O(deg(i))
    double min_ratio_squared = + INFINITY;
    double max_ratio_squared = - INFINITY;
    for (int eid : g[i]) {
        auto e = edges[eid];
        int j = e.to != i ? e.to : e.from;
        setmin(min_ratio_squared, dist_squared(p_i, p[j]) /(double) sq(e.dist));
        setmax(max_ratio_squared, dist_squared(p_i, p[j]) /(double) sq(e.dist));
    }
    return { min_ratio_squared, max_ratio_squared };
}

pair<int, int> find_bounding_edges(vector<point_t> const & p, vector<edge_t> const & edges) { // O(E)
    double min_ratio_squared = + INFINITY;
    double max_ratio_squared = - INFINITY;
    int min_eid = -1;
    int max_eid = -1;
    repeat (i, edges.size()) {
        auto e = edges[i];
        double ratio_squared = dist_squared(p[e.from], p[e.to]) /(double) sq(e.dist);
        if (ratio_squared < min_ratio_squared) {
            min_ratio_squared = ratio_squared;
            min_eid = i;
        }
        if (max_ratio_squared < ratio_squared) {
            max_ratio_squared = ratio_squared;
            max_eid = i;
        }
    }
    return { min_eid, max_eid };
}

double calculate_ratio_squared(int eid, vector<point_t> const & p, vector<edge_t> const & edges) { // O(1)
    edge_t e = edges[eid];
    return dist_squared(p[e.from], p[e.to]) /(double) sq(e.dist);
}
double calculate_score(int min_eid, int max_eid, vector<point_t> const & p, vector<edge_t> const & edges) { // O(1)
    double min_ratio_squared = calculate_ratio_squared(min_eid, p, edges);
    double max_ratio_squared = calculate_ratio_squared(max_eid, p, edges);
    return sqrt(min_ratio_squared / max_ratio_squared);
}
double calculate_score(vector<point_t> const & p, vector<edge_t> const & edges) { // O(E)
    int min_eid, max_eid; tie(min_eid, max_eid) = find_bounding_edges(p, edges);
    return calculate_score(min_eid, max_eid, p, edges);
}

template <class T>
bool is_distinct(vector<T> data) { // O(N log N)
    whole(sort, data);
    return whole(unique, data) == data.end();
}

vector<point_t> solve(int n, vector<edge_t> & edges) {
    // prepare
    random_device device;
    default_random_engine gen(device());
    chrono::high_resolution_clock::time_point clock_begin = chrono::high_resolution_clock::now();
    // make graph
    vector<vector<int> > g(n);
    repeat (i, edges.size()) {
        auto e = edges[i];
        g[e.to  ].push_back(i);
        g[e.from].push_back(i);
    }
    // positions
    vector<point_t> p;
    { // initialize positions
        double score = - INFINITY;
        int iteration = 0;
        for (; iteration < 100; ++ iteration) {
            vector<point_t> q(n);
            repeat (i,n) {
                uniform_int_distribution<int> dist(0, 700);
                q[i].y = dist(gen);
                q[i].x = dist(gen);
            }
            double next_score = calculate_score(q, edges);
            if (score < next_score) {
                score = next_score;
                p = q;
                cerr << "[*] init " << iteration << ": updated " << score << endl;
            }
        }
        cerr << "[+] " << iteration << " iterations for initialization" << endl;
    }
    { // hill climbing
        int min_eid, max_eid; tie(min_eid, max_eid) = find_bounding_edges(p, edges);
        double min_ratio_squared = calculate_ratio_squared(min_eid, p, edges);
        double max_ratio_squared = calculate_ratio_squared(max_eid, p, edges);
        int iteration = 0;
        for (; ; ++ iteration) {
            if (iteration % 256 == 0) {
                chrono::high_resolution_clock::time_point clock_end = chrono::high_resolution_clock::now();
                int elapsed = chrono::duration_cast<chrono::milliseconds>(clock_end - clock_begin).count();
                if (elapsed >= 9000) break;
            }
            int choice = uniform_int_distribution<int>(0, 6)(gen);
            int i =
                choice == 0 ? edges[min_eid].from :
                choice == 1 ? edges[min_eid].to :
                choice == 2 ? edges[max_eid].from :
                choice == 3 ? edges[max_eid].to :
                uniform_int_distribution<int>(0, n-1)(gen);
            point_t updated_p_i;
            updated_p_i.y = uniform_int_distribution<int>(position_min, position_max)(gen);
            updated_p_i.x = uniform_int_distribution<int>(position_min, position_max)(gen);
            double updated_min_ratio_squared, updated_max_ratio_squared; tie(updated_min_ratio_squared, updated_max_ratio_squared) = calculate_updated_ratio_squared(p, i, updated_p_i, edges, g);
            if (min_ratio_squared < eps + updated_min_ratio_squared and updated_max_ratio_squared < eps + max_ratio_squared) {
                p[i] = updated_p_i;
                tie(min_eid, max_eid) = find_bounding_edges(p, edges);
                min_ratio_squared = calculate_ratio_squared(min_eid, p, edges);
                max_ratio_squared = calculate_ratio_squared(max_eid, p, edges);
                // double score = sqrt(min_ratio_squared / max_ratio_squared);
                // cerr << "[*] hill " << iteration << ": updated " << score << endl;
            }
        }
        cerr << "[+] " << iteration << " iterations for hill climbing" << endl;
    }
    // assert distinct
    set<point_t> used;
    repeat (i,n) {
        while (used.count(p[i])) {
            uniform_int_distribution<int> dist(-1, 1);
            p[i].y = clamp(p[i].y + dist(gen), position_min, position_max);
            p[i].x = clamp(p[i].x + dist(gen), position_min, position_max);
        }
        used.insert(p[i]);
    }
    return p;
}

vector<int> GraphDrawing::plot(int n, vector<int> edges) {
    // unpack
    assert (edges.size() % 3 == 0);
    int m = edges.size() / 3;
    vector<edge_t> e(m);
    repeat (i,m) {
        int u    = edges[3*i];
        int v    = edges[3*i+1];
        int dist = edges[3*i+2];
        assert (0 <= u and u < n);
        assert (0 <= v and v < n);
        assert (length_min <= dist and dist <= length_max);
        e[i] = (edge_t) { u, v, dist };
    }
    // solve
    vector<point_t> p = solve(n, e);
    // pack
    vector<int> result(2*n);
    repeat (i,n) {
        int y = p[i].y;
        int x = p[i].x;
        assert (position_min <= y and y <= position_max);
        assert (position_min <= x and x <= position_max);
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

