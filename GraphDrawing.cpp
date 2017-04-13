// C++11
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

class GraphDrawing {
public:
    vector<int> plot(int N, vector<int> edges) {
        vector<int> ret;
        srand(123);
        for (int i = 0; i < 2 * N; ++i) {
            ret.push_back(rand() % 701);
        }
        return ret;
    }
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
    for (int i = 0; i < v.size(); ++i)
        cin >> v[i];
}

int main() {
    GraphDrawing gd;
    int N;
    cin >> N;
    int E;
    cin >> E;
    vector<int> edges(E);
    getVector(edges);
    
    vector<int> ret = gd.plot(N, edges);
    cout << ret.size() << endl;
    for (int i = 0; i < (int)ret.size(); ++i)
        cout << ret[i] << endl;
    cout.flush();
}
