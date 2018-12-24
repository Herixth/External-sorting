#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <cmath>
using namespace std;

string filename;

int main(int argc, char *argv[]) {
    filename = argv[1];
    ifstream inFile(filename.c_str(), ios::in | ios::binary);
    if (!inFile.good()) {
        cout << "file open error" << endl;
        exit(-1);
    }
    pair<float, unsigned int> rec;
    double prec = -1e7;
    int idx = 0;
    cout << "__________________________________" << endl;
    while (inFile.read((char *)&rec, sizeof(rec))) {
        if (rec.first < prec) {
            cout << "file: " << filename << " has not beed sorted" << endl;
            cout << "prev-idx: " << idx - 1 << "  key: " << prec << endl;
            cout << "curr-idx: " << idx << "  key: " << rec.first << endl;
            cout << "__________________________________" << endl;
            return 0;
        }
        prec = rec.first;
        idx ++;
    }

    cout << "file: " << filename << " has been sorted" << endl;
    cout << "__________________________________" << endl;
    return 0;
}
