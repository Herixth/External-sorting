#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <cstdlib>
using namespace std;

const int maxn = 10000;

int N, filesize, rec, cnt;
const string state = "sorted";
double data[maxn][4]; // runs run-build run-merge total

double sita[4];
double mid[4];

void readInfo(ifstream &inFile) {
    string line;
    
    while(getline(inFile, line)) {
        if (line.substr(line.find_last_of(',') + 1) != state) {
            exit(-1);
        }
        
        sscanf(line.c_str(), "%d,%lf,%d,%lf,%lf,%lf,%lf",\
                &N, &filesize, &rec, &data[cnt][0], &data[cnt][1],\
                &data[cnt][2], &data[cnt][3]);
        for (int inc = 0; inc < 4; inc ++)
            mid[inc] += data[cnt][inc];
        cnt++;
    }
    for (int inc = 0; inc < 4; inc ++)
        mid[inc] /= cnt;
}

void calc_sita() {
    for (int inc = 0; inc < cnt; inc ++) {
        for (int snc = 0; snc < 4; snc ++) {
            sita[snc] += pow(data[inc][snc] - mid[snc], 2);
        }
    }

    for (int inc = 0; inc < 4; inc ++)
        sita[inc] /= cnt;
}

void writeInfor(ofstream &outFile) {
    outFile << N << "," << filesize << "," << rec << ",";
    for (int inc = 0; inc < 4; inc ++) {
        outFile << mid[inc] << "," << sita[inc] << ",";
    }
    outFile << state << endl;
}

int main(int argc, char *argv[]) {
    ifstream inFile(argv[1]);
    ofstream outFile(argv[2], ios::app);
    
    readInfo(inFile);

    calc_sita();

    writeInfor(outFile);
    inFile.close();
    outFile.close();
    return 0;
}

