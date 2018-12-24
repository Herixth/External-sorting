#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <ctime>
using namespace std;

int N;
string filename;

const int maxn = 1e6;
const int block_rec = 512;

int main() {
    srand(time(NULL));
    cout << "Please input the filename: ";
    cin >> filename;
    cout << "Please input N to implement 8N blocks of data: ";
    cin >> N;
    
    ofstream outFile(filename.c_str(), ios::out | ios::binary);
    if (!outFile.good()) {
        cout << "file open error!" << endl;
        exit(-1);
    }
    
    int total_ = block_rec * 8 * N;
    for (int inc = 0; inc < total_; inc ++) {
        pair<float, unsigned int> rec(1.0 * (rand() % maxn) / 11, rand() % maxn);
        outFile.write((char *)&rec, sizeof(rec));
    }
    cout << "_____________________________________________________" << endl;
    cout << "Succeed in writing 8 * 512 * " << N << " = " << 8 * 512 * N << " records in file: " << filename << endl;
    cout << "File's size should be " << 8 * 4096 * N << " bytes" << " (" << 32 * N << " kb)" << endl;
    cout << "_____________________________________________________" << endl;
    outFile.close();
    return 0;
}
