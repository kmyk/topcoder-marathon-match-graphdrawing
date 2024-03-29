#pragma GCC optimize "O3"
#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <array>
#include <bitset>
#include <set>
#include <map>
#include <queue>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <limits>
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

pair<double, double> calculate_ratio_squared_around(int i, vector<point_t> const & p, vector<edge_t> const & edges, vector<vector<int> > const & g) { // O(deg(i))
    double min_ratio_squared = + INFINITY;
    double max_ratio_squared = - INFINITY;
    for (int eid : g[i]) {
        auto e = edges[eid];
        setmin(min_ratio_squared, dist_squared(p[e.from], p[e.to]) /(double) sq(e.dist));
        setmax(max_ratio_squared, dist_squared(p[e.from], p[e.to]) /(double) sq(e.dist));
    }
    return { min_ratio_squared, max_ratio_squared };
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
        array<bitset<position_max+1>, position_max+1> used = {}; // result must be distinct
        repeat (i,n) {
            do {
                uniform_int_distribution<int> dist(0, 700);
                q[i].y = dist(gen);
                q[i].x = dist(gen);
            } while (used[q[i].y][q[i].x]);
            used[q[i].y][q[i].x] = true;
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

vector<point_t> clamp_positions(int margin, vector<point_t> p) {
    int n = p.size();
    int min_y = numeric_limits<int>::max();
    int min_x = numeric_limits<int>::max();
    int max_y = numeric_limits<int>::min();
    int max_x = numeric_limits<int>::min();
    repeat (i,n) {
        setmin(min_y, p[i].y);
        setmin(min_x, p[i].x);
        setmax(max_y, p[i].y);
        setmax(max_x, p[i].x);
    }
    double a = (position_max - position_min - 2*margin) /(double) max(max_y - min_y, max_x - min_x);
    int b_y = margin - min_y;
    int b_x = margin - min_x;
    repeat (i,n) {
        p[i].y = a * (p[i].y - min_y) + b_y;
        p[i].x = a * (p[i].x - min_x) + b_x;
    }
    return p;
}

template <class Generator>
vector<point_t> make_positions_distinct(vector<point_t> p, vector<edge_t> const & edges, Generator & gen) {
    double original_score = calculate_score(p, edges);
    int n = p.size();
    vector<double> absolute_ratio_squared(edges.size());
    repeat (eid, edges.size()) {
        double ratio_squared = calculate_ratio_squared(eid, p, edges);
        absolute_ratio_squared[eid] = ratio_squared >= 1 ? ratio_squared : 1 / ratio_squared;
    }
    vector<int> eids(edges.size());
    whole(iota, eids, 0);
    whole(sort, eids, [&](int i, int j) { return absolute_ratio_squared[i] < absolute_ratio_squared[j]; });
    whole(reverse, eids);
    set<point_t> used;
    vector<bool> fixed(n);
    for (int eid : eids) {
        edge_t e = edges[eid];
        for (int i : { e.to, e.from }) {
            if (fixed[i]) continue;
            fixed[i] = true;
            for (int dist = 1; ; ++ dist) { // may decreases the score
                point_t q;
                q.y = random_walk(p[i].y, sqrt(dist), gen);
                q.x = random_walk(p[i].x, sqrt(dist), gen);
                if (not used.count(q)) {
                    p[i] = q;
                    break;
                }
            }
            used.insert(p[i]);
        }
    }
    double updated_score = calculate_score(p, edges);
    if (updated_score + eps < original_score) {
        cerr << "[!] score decreased for distinctness: " << original_score << " -> " << updated_score << endl;
    }
    return p;
}

vector<point_t> solve(int n, vector<edge_t> & edges) {
    // prepare
    random_device device;
    default_random_engine gen(device());
    double clock_begin = rdtsc();

    // data
    vector<vector<int> > g = make_adjacent_list_from_edges(n, edges);
    vector<point_t> p = compute_good_initial_positions(100, n, edges, gen);

    // simulated annealing
    double highscore_squared = - INFINITY;
    vector<point_t> best_p;
    int min_eid, max_eid; tie(min_eid, max_eid) = find_bounding_edges(p, edges);
    double min_ratio_squared = calculate_ratio_squared(min_eid, p, edges);
    double max_ratio_squared = calculate_ratio_squared(max_eid, p, edges);
    double t = -1;
    int iteration = 0;
    auto check_time = [&](double time_limit) {
        if (iteration % 8192 == 0) {
            double clock_end = rdtsc();
            t = clock_end - clock_begin;
            return t <= time_limit;
        }
        return true;
    };
    double updated_min_ratio_squared, updated_max_ratio_squared; // global variables
    auto is_score_preserved = [&]() {
        return min_ratio_squared < eps + updated_min_ratio_squared and updated_max_ratio_squared < eps + max_ratio_squared;
    };
    auto is_score_increased = [&](int i) {
        bool is_min = i == edges[min_eid].from or i == edges[min_eid].to;
        bool is_max = i == edges[max_eid].from or i == edges[max_eid].to;
        return
            (is_max and updated_max_ratio_squared + eps < max_ratio_squared) or
            (is_min and min_ratio_squared + eps < updated_min_ratio_squared) ;
    };

    // first
    {
        vector<double> ratio_squared(edges.size());
        repeat (i, edges.size()) ratio_squared[i] = calculate_ratio_squared(i, p, edges);
        vector<int> edges_sorted(edges.size());
        whole(iota, edges_sorted, 0);
        for (; check_time(8.0); ++ iteration) {
            if (iteration % 256 == 0) {
                whole(sort, edges_sorted, [&](int i, int j) { return ratio_squared[i] < ratio_squared[j]; });
            }
            // change
            int i; {
                int choice = uniform_int_distribution<int>(0, 7-1)(gen);
                int eid_rank =
                    choice == 0 ? uniform_int_distribution<int>(0,                min(4,   int(edges.size()))-1)(gen) :
                    choice == 1 ? uniform_int_distribution<int>(0,                min(40,  int(edges.size()))-1)(gen) :
                    choice == 2 ? uniform_int_distribution<int>(0,                min(400, int(edges.size()))-1)(gen) :
                    choice == 3 ? uniform_int_distribution<int>(max(0, int(edges.size()) -   4), edges.size()-1)(gen) :
                    choice == 4 ? uniform_int_distribution<int>(max(0, int(edges.size()) -  40), edges.size()-1)(gen) :
                    choice == 5 ? uniform_int_distribution<int>(max(0, int(edges.size()) - 400), edges.size()-1)(gen) :
                    uniform_int_distribution<int>(0, edges.size()-1)(gen);
                edge_t e = edges[edges_sorted[eid_rank]];
                i = uniform_int_distribution<int>(0, 1)(gen) ? e.from : e.to;
            }
            point_t saved_p_i = p[i];
            if ((t < 2.0 and (iteration & 0x1) == 0) or (iteration & 0xf) == 0) {
                p[i].y = random_position(gen);
                p[i].x = random_position(gen);
            } else {
                p[i].y = random_walk(p[i].y, 16, gen);
                p[i].x = random_walk(p[i].x, 16, gen);
            }
            // evaluate
            tie(updated_min_ratio_squared, updated_max_ratio_squared) = calculate_ratio_squared_around(i, p, edges, g);
            bool score_preserved = is_score_preserved();
            bool force_accepted = not score_preserved and bernoulli_distribution((10-t) * 0.00001)(gen);
            if (score_preserved or force_accepted) {
                for (int eid : g[i]) ratio_squared[eid] = calculate_ratio_squared(eid, p, edges);
                if (force_accepted or is_score_increased(i)) {
                    whole(sort, edges_sorted, [&](int i, int j) { return ratio_squared[i] < ratio_squared[j]; });
                    min_eid = edges_sorted.front();
                    max_eid = edges_sorted.back();
                    min_ratio_squared = ratio_squared[min_eid];
                    max_ratio_squared = ratio_squared[max_eid];
                    double score_squared = min_ratio_squared / max_ratio_squared;
                    if (highscore_squared + eps < score_squared) {
                        highscore_squared = score_squared;
                        best_p = p;
                        cerr << "[*] " << iteration << " " << t << "s : score " << sqrt(score_squared) << endl;
                    }
                }
            } else {
                p[i] = saved_p_i;
            }
        }
        cerr << "[*] done: score " << sqrt(highscore_squared) << endl;
    }

    // second
    p = best_p;
    p = clamp_positions(50, p);
    cerr << "[*] clamp: score " << calculate_score(p, edges) << endl;
    p = make_positions_distinct(p, edges, gen);
    cerr << "[*] distinct: score " << calculate_score(p, edges) << endl;
    best_p = p;
    highscore_squared = sq(calculate_score(p, edges));
    array<bitset<position_max+1>, position_max+1> used = {};
    repeat (i,n) used[p[i].y][p[i].x] = true;
    for (; check_time(9.5); ++ iteration) {
        // change
        int choice = uniform_int_distribution<int>(0, 10-1)(gen);
        int i =
            choice == 0 ? edges[min_eid].from :
            choice == 1 ? edges[min_eid].to   :
            choice == 2 ? edges[max_eid].from :
            choice == 3 ? edges[max_eid].to   :
            choice == 4 ? random_adjacent(edges[min_eid].from, edges, g, gen) :
            choice == 5 ? random_adjacent(edges[min_eid].to,   edges, g, gen) :
            choice == 6 ? random_adjacent(edges[max_eid].from, edges, g, gen) :
            choice == 7 ? random_adjacent(edges[max_eid].to,   edges, g, gen) :
            uniform_int_distribution<int>(0, n-1)(gen);
        point_t saved_p_i = p[i];
        p[i].y = random_walk(p[i].y, 1, gen);
        p[i].x = random_walk(p[i].x, 1, gen);
        do {
            p[i].y = random_walk(p[i].y, 1, gen);
            p[i].x = random_walk(p[i].x, 1, gen);
        } while (used[p[i].y][p[i].x]);
        // evaluate
        tie(updated_min_ratio_squared, updated_max_ratio_squared) = calculate_ratio_squared_around(i, p, edges, g);
        bool score_preserved = is_score_preserved();
        bool force_accepted = not score_preserved and bernoulli_distribution(0.00001)(gen);
        if (score_preserved or force_accepted) {
            used[saved_p_i.y][saved_p_i.x] = false;
            used[p[i].y][p[i].x] = true;
            if (force_accepted or is_score_increased(i)) {
                tie(min_eid, max_eid) = find_bounding_edges(p, edges);
                min_ratio_squared = calculate_ratio_squared(min_eid, p, edges);
                max_ratio_squared = calculate_ratio_squared(max_eid, p, edges);
                double score_squared = min_ratio_squared / max_ratio_squared;
                if (highscore_squared + eps < score_squared) {
                    highscore_squared = score_squared;
                    best_p = p;
                    cerr << "[*] " << iteration << " " << t << "s : score " << sqrt(score_squared) << endl;
                }
            }
        } else {
            p[i] = saved_p_i;
        }
    }

    // done
    cerr << "[+] " << iteration << " iterations for simulated annealing" << endl;
    return best_p;
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

