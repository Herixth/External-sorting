#ifndef EXTSORT_INIT
#define EXTSORT_INIT

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#define APPEND std::ios::app
#define READIN std::ios::in
#define OUTPUT std::ios::out
#define BINARY std::ios::binary

//#define __DEBUG__

typedef unsigned int UI;
typedef float FL;
typedef std::pair<FL, UI> pis;
typedef std::pair<int, int> pii;

const int block_num   = 8;
const int block_bytes = 4096;
const int rec_max     = 512;
const double exps     = 1e-6;
const FL max_ft       = 1e7;

inline bool comp(float, float);

class Buffer;
class Min_heap;
class Extsort_Manager;

class Buffer {
public:
    Buffer();
    ~Buffer();

    bool get_input(std::ifstream &, int = -1);
    bool write_output(std::ofstream &);

    pis get_value();
    int get_curr();
    void append_value(pis);
    void move_to_start();
    void move_to_end();
    void move_to_next();

    void clear();

    bool at_end();
    bool empty();
private:
    pis __buffer[rec_max];
    int __curr;
    int __maxN;
    bool is_empty;
};

class Min_heap {
public:
    Min_heap();
    ~Min_heap();

    void init(Buffer &, std::ifstream &);
    bool empty() const;
    pis remove_add(pis);

    void build_heap();
    int  size() const;
private:
    int __left(int) const;
    int __right(int) const;
    bool __isLeaf(int) const;
    void __siftdown(int);

    pis heap[block_num * rec_max];
    int __maxN;
    int __curr;
};


class Extsort_Manager {
public:
    Extsort_Manager(char **);
    ~Extsort_Manager();

    bool init();
    void create_runfile_RS();
    void merge();

private:
    void __merge(int, std::vector<pii> &);

    std::string rec_fname;
    std::string stat_fname;
    std::string run_fname;

    std::ifstream inFile;
    std::ofstream outFile;
    std::ofstream statFile;

    std::vector<pii> run_idx;
    Buffer input_buffer;
    Buffer output_buffer;

    Buffer buffer_pool[block_num];

    Min_heap min_heap;

    std::clock_t start_clc;
    std::clock_t run_build_cost;
    std::clock_t merge_cost;
};


#endif // !EXTSORT_INIT