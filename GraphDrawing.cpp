#pragma GCC optimize "O3"
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
#include <cassert>
#define repeat(i,n) for (int i = 0; (i) < int(n); ++(i))
#define repeat_from(i,m,n) for (int i = (m); (i) < int(n); ++(i))
#define repeat_reverse(i,n) for (int i = (n)-1; (i) >= 0; --(i))
#define repeat_from_reverse(i,m,n) for (int i = (n)-1; (i) >= int(m); --(i))
#define whole(f,x,...) ([&](decltype((x)) whole) { return (f)(begin(whole), end(whole), ## __VA_ARGS__); })(x)
#define debug(x) #x << " = " << (x) << " "
#ifndef LOCAL
#define cerr if (false) cerr
#endif
using ll = long long;
using namespace std;
template <class T> inline void setmax(T & a, T const & b) { a = max(a, b); }
template <class T> inline void setmin(T & a, T const & b) { a = min(a, b); }
template <class T> T sq(T x) { return x * x; }
template <class T> T clamp(T a, T l, T r) { return min(max(a, l), r); } // [l, r]
constexpr double eps = 1e-10;

double rdtsc() { // in seconds
    constexpr double ticks_per_sec = 2500000000;
    uint32_t lo, hi;
    asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32 | lo) / ticks_per_sec;
}

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
int opposite(int i, edge_t e) {
    return i != e.to ? e.to : e.from;
}

constexpr int length_min = 1;
constexpr int length_max = 991; // inclusive
constexpr int position_min = 0;
constexpr int position_max = 700; // inclusive

template <class Generator>
int random_walk(int base, int delta, Generator & gen) {
    uniform_int_distribution<int> dist(- delta, + delta);
    return clamp(base + dist(gen), position_min, position_max);
}
template <class Generator>
int random_adjacent(int i, vector<edge_t> const & edges, vector<vector<int> > const & g, Generator & gen) {
    int k = uniform_int_distribution<int>(0, g[i].size()-1)(gen);
    int eid = g[i][k];
    int j = opposite(i, edges[eid]);
    return j;
}
template <class Generator>
int random_position(Generator & gen) {
    return uniform_int_distribution<int>(position_min, position_max)(gen);
}

pair<double, double> calculate_updated_ratio_squared(vector<point_t> const & p, int i, point_t p_i, vector<edge_t> const & edges, vector<vector<int> > const & g) { // O(deg(i))
    double min_ratio_squared = + INFINITY;
    double max_ratio_squared = - INFINITY;
    for (int eid : g[i]) {
        auto e = edges[eid];
        int j = opposite(i, e);
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

vector<vector<int> > make_adjacent_list_from_edges(int n, vector<edge_t> & edges) {
    vector<vector<int> > g(n);
    repeat (i, edges.size()) {
        auto e = edges[i];
        g[e.to  ].push_back(i);
        g[e.from].push_back(i);
    }
    return g;
}

template <class Generator>
vector<point_t> compute_good_initial_positions(int iteration_count, int n, vector<edge_t> & edges, Generator & gen) {
    vector<point_t> p;
    double score = - INFINITY;
    int iteration = 0;
    for (; iteration < iteration_count; ++ iteration) {
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
            cerr << "[*] init " << iteration << ": score " << score << endl;
        }
    }
    cerr << "[+] " << iteration << " iterations for initialization" << endl;
    return p;
}

template <class Generator>
vector<point_t> make_positions_distinct(vector<point_t> p, Generator & gen) {
    int n = p.size();
    set<point_t> used;
    repeat (i,n) {
        while (used.count(p[i])) {
            p[i].y = random_walk(p[i].y, 1, gen); // may decreases the score
            p[i].x = random_walk(p[i].x, 1, gen);
        }
        used.insert(p[i]);
    }
    return p;
}

vector<point_t> solve(int n, vector<edge_t> & edges) {
    // prepare
    random_device device;
    default_random_engine gen(device());
    double clock_begin = rdtsc();
    // pre
    vector<vector<int> > g = make_adjacent_list_from_edges(n, edges);
    vector<point_t> p = compute_good_initial_positions(100, n, edges, gen);
    { // hill climbing
        constexpr double break_time = 9.5; // sec
        constexpr double write_time = 8.0; // sec
        double highscore = - INFINITY;
        vector<point_t> best_p;
        int min_eid, max_eid; tie(min_eid, max_eid) = find_bounding_edges(p, edges);
        double min_ratio_squared = calculate_ratio_squared(min_eid, p, edges);
        double max_ratio_squared = calculate_ratio_squared(max_eid, p, edges);
        double t = -1;
        int iteration = 0;
        for (; ; ++ iteration) {
            if (iteration % 65536 == 0) {
                double clock_end = rdtsc();
                t = clock_end - clock_begin;
                if (t > break_time) break;
                if (best_p.empty() and t > write_time) {
                    highscore = - INFINITY;
                    best_p = p;
                }
            }
            int choice = uniform_int_distribution<int>(0, 6)(gen);
            int i =
                choice == 0 ? edges[min_eid].from :
                choice == 1 ? edges[min_eid].to   :
                choice == 2 ? edges[max_eid].from :
                choice == 3 ? edges[max_eid].to   :
                uniform_int_distribution<int>(0, n-1)(gen);
            point_t updated_p_i;
            if (t < 3.0) {
                updated_p_i.y = random_position(gen);
                updated_p_i.x = random_position(gen);
            } else {
                updated_p_i.y = random_walk(p[i].y, 1, gen);
                updated_p_i.x = random_walk(p[i].x, 1, gen);
            }
            double updated_min_ratio_squared, updated_max_ratio_squared; tie(updated_min_ratio_squared, updated_max_ratio_squared) = calculate_updated_ratio_squared(p, i, updated_p_i, edges, g);
            bool acceptable = min_ratio_squared < eps + updated_min_ratio_squared and updated_max_ratio_squared < eps + max_ratio_squared;
            if (acceptable or bernoulli_distribution(0.0001)(gen)) {
                p[i] = updated_p_i;
                bool can_update_score = choice < 4;
                bool is_max = choice & 2;
                if (can_update_score and (is_max ? updated_max_ratio_squared + eps < max_ratio_squared : min_ratio_squared + eps < updated_min_ratio_squared)) {
                    tie(min_eid, max_eid) = find_bounding_edges(p, edges);
                    min_ratio_squared = calculate_ratio_squared(min_eid, p, edges);
                    max_ratio_squared = calculate_ratio_squared(max_eid, p, edges);
                    double score = sqrt(min_ratio_squared / max_ratio_squared);
                    if (highscore + eps < score) {
                        highscore = score;
                        if (t > write_time) best_p = p;
                        cerr << "[*] " << iteration << ": score " << score << endl;
                    }
                }
            }
        }
        cerr << "[+] " << iteration << " iterations for hill climbing" << endl;
        p = best_p;
    }
    // post
    p = make_positions_distinct(p, gen);
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

