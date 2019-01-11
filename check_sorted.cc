#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <cmath>
using namespace std;

#define __TEST__

string filename;
string statFile;

int main(int argc, char *argv[]) {
    filename = argv[1];
    ifstream inFile(filename.c_str(), ios::in | ios::binary);
    if (!inFile.good()) {
        cout << "file open error" << endl;
        exit(-1);
    }
#ifdef __TEST__
    statFile = argv[2];
    ofstream outFile(statFile.c_str(), ios::app);
#endif
    pair<float, unsigned int> rec;
    double prec = -1e7;
    int idx = 0;
    while (inFile.read((char *)&rec, sizeof(rec))) {
        if (rec.first < prec) {
#ifndef __TEST__
            cout << "__________________________________" << endl;
            cout << "file: " << filename << " has not been sorted" << endl;
            cout << "prev-idx: " << idx - 1 << "  key: " << prec << endl;
            cout << "curr-idx: " << idx << "  key: " << rec.first << endl;
            cout << "__________________________________" << endl;
#else
            outFile << ",unsorted" << std::endl;
#endif
            inFile.close();
            return 0;
        }
        prec = rec.first;
        idx ++;
    }

#ifndef __TEST__
    cout << "file: " << filename << " has been sorted" << endl;
    cout << "__________________________________" << endl;
#else
    outFile << ",sorted" << std::endl;
    outFile.close();
#endif
    inFile.close();
    return 0;
}
