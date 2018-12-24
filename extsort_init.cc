#include "extsort_init.h"


inline bool comp(float objA, float objB) {
    return objB - objA > exps;
}

Extsort_Manager::Extsort_Manager(char **argv) {
    rec_fname  = argv[1];
    stat_fname = argv[2];
    run_fname  = "run_file.tmp";
}

Extsort_Manager::~Extsort_Manager() {
    statFile.close();
}

bool Extsort_Manager::init() {
    std::srand(std::time(NULL));
    start_clc = std::clock();
    std::remove(("cp_" + rec_fname).c_str());
    std::rename(rec_fname.c_str(), ("cp_" + rec_fname).c_str());
    inFile.open(("cp_" + rec_fname).c_str(), READIN | BINARY);
    outFile.open(run_fname.c_str(), OUTPUT | BINARY);
    statFile.open(stat_fname.c_str(), APPEND);
    return inFile.good() && outFile.good() && statFile.good();
}

void Extsort_Manager::create_runfile_RS() {
    inFile.seekg(0, std::ios::end);
    int total_block_num = inFile.tellg() / block_bytes;
    inFile.seekg(0, std::ios::beg);
    min_heap.init(input_buffer, inFile);
    output_buffer.clear();

    int start_idx = 0;
    for (int idx = block_num; idx < total_block_num; idx++) {
        input_buffer.get_input(inFile);
        for (input_buffer.move_to_start(); !input_buffer.at_end();
            input_buffer.move_to_next()) {
            pis run_one = min_heap.remove_add(input_buffer.get_value());
            output_buffer.append_value(run_one);
            if (min_heap.empty()) {
                run_idx.push_back(pii(start_idx, output_buffer.get_curr()));
                output_buffer.write_output(outFile);
                start_idx = std::ceil(1.0 * outFile.tellp() / block_bytes);
                min_heap.build_heap();
            }
            if (output_buffer.at_end()) {
                output_buffer.write_output(outFile);
            }
        }
    }

    int last_size = min_heap.size();
    while (!min_heap.empty()) {
        if (output_buffer.at_end()) {
            output_buffer.write_output(outFile);
        }
        pis run_one = min_heap.remove_add(pis(-1.0, 0));
        output_buffer.append_value(run_one);
    }

    run_idx.push_back(pii(start_idx, output_buffer.get_curr()));
    output_buffer.write_output(outFile);
    start_idx = std::ceil(1.0 * outFile.tellp() / block_bytes);

    if (last_size < block_num * rec_max) {
        min_heap.build_heap();
        while (!min_heap.empty()) {
            if (output_buffer.at_end()) {
                output_buffer.write_output(outFile);
            }
            pis run_one = min_heap.remove_add(pis(-2.0, 0));
            if (run_one.first != -1.0)
                output_buffer.append_value(run_one);
        }

        run_idx.push_back(pii(start_idx, output_buffer.get_curr()));
        output_buffer.write_output(outFile);
        start_idx = std::ceil(1.0 * outFile.tellp() / block_bytes);
    }

    run_idx.push_back(pii(start_idx, 0));

#ifdef __DEBUG__
    int total_rec = 0;
    std::cout << "run: " << run_idx.size() << std::endl;
    for (int inc = 0; inc < run_idx.size(); inc++) {
        std::cout << "idx: " << run_idx[inc].first << "  remain: " << run_idx[inc].second << std::endl;
        if (inc) {
            total_rec += (run_idx[inc].first - run_idx[inc - 1].first - 1) * 512 + run_idx[inc - 1].second;
        }
    }

    std::cout << "total: " << total_rec << std::endl;

#endif // __DEBUG__
    statFile << "_________________ * _________________" << std::endl;
    statFile << "File size:   " << total_block_num * 4 << " kb" << " (" << total_block_num * 4.0 / 1024 << "mb)" << std::endl;
    statFile << "records num: " << total_block_num * 512 << std::endl;
    statFile << "total runs:  " << run_idx.size() - 1 << std::endl;
    inFile.close();
    outFile.close();

    run_build_cost = std::clock() - start_clc;
}

void Extsort_Manager::merge() {
    min_heap.~Min_heap();
    std::string infName = run_fname;
    std::string outfName = rec_fname;
    int merge_times = std::ceil(std::log(run_idx.size() - 1) / std::log(block_num));
    statFile << "Merge times: " << merge_times << std::endl;
    for (int inc = 0; inc < merge_times; inc ++) {
        int loop_times = std::ceil((1.0 * run_idx.size() - 1) / block_num);
        inFile.open(infName, READIN | BINARY);
        outFile.open(outfName, OUTPUT | BINARY);
        std::vector<pii> new_run_idx;
        for (int inc = 0; inc < loop_times; inc++) {
            __merge(inc * block_num, new_run_idx);
        }
        run_idx.assign(new_run_idx.begin(), new_run_idx.end());
        run_idx.push_back(pii(std::ceil(1.0 * outFile.tellp() / block_bytes), 0));
        inFile.close();
        outFile.close();
        std::swap(infName, outfName);
    }
    if (merge_times & 0x1) {
        std::remove(run_fname.c_str());
    }
    else {
        std::remove(rec_fname.c_str());
        std::rename(run_fname.c_str(), rec_fname.c_str());
    }

    merge_cost = std::clock() - start_clc - run_build_cost;

    statFile << "Time cost: (ms)" << std::endl;
    statFile << ">> run build cost: " << run_build_cost << std::endl;
    statFile << ">> merge cost    : " << merge_cost << std::endl;
    statFile << ">> total cost    : " << run_build_cost + merge_cost << std::endl;
    statFile << "____________*____ * ____*____________" << std::endl;
    std::cout << "_________\nextsort success!" << std::endl;
}

void Extsort_Manager::__merge(int idx, std::vector<pii> &new_idx) {
    int each_run_size[block_num] = { };
    int start = outFile.tellp() / block_bytes;
    int end_idx = std::min(idx + block_num, int(run_idx.size() - 1));
    //init buffer pool
    int check = 0;
    for (int inc = 0; inc < end_idx - idx; inc++) {
        buffer_pool[inc].get_input(inFile, run_idx[inc + idx].first * block_bytes);
        each_run_size[inc] = \
            (run_idx[inc + 1 + idx].first - run_idx[inc + idx].first - 1) * rec_max\
            + run_idx[inc + idx].second;
        buffer_pool[inc].move_to_start();
        check += each_run_size[inc];
    }
    
    new_idx.push_back(pii(start, check % rec_max == 0 ? rec_max : check % rec_max));

    output_buffer.clear();
    int min_idx = -1;
    while (check --) {
        // pick the minimun one
        for (int inc = 0; inc < end_idx - idx; inc++) {
            if (each_run_size[inc] > 0 &&
                (min_idx == -1 || comp(buffer_pool[inc].get_value().first,
                 buffer_pool[min_idx].get_value().first))) {
                min_idx = inc;
            }
        }
        
        output_buffer.append_value(buffer_pool[min_idx].get_value());
        buffer_pool[min_idx].move_to_next();

        each_run_size[min_idx] --;

        if (output_buffer.at_end())
            output_buffer.write_output(outFile);

        if (buffer_pool[min_idx].at_end() && each_run_size[min_idx] > 0) {
            buffer_pool[min_idx].get_input(inFile, ++run_idx[min_idx + idx].first * block_bytes);
            buffer_pool[min_idx].move_to_start();
        }
        min_idx = -1;
    }
    // output remain
    output_buffer.write_output(outFile);

}

//________________________________________________

Min_heap::Min_heap() {
    __maxN = block_num * rec_max;
    __curr = 0;
}

Min_heap::~Min_heap() { }

bool Min_heap::empty() const {
    return !__curr;
}

int Min_heap::__left(int pos) const {
    return 2 * pos + 1;
}

int Min_heap::__right(int pos) const {
    return 2 * pos + 2;
}

bool Min_heap::__isLeaf(int pos) const {
    return pos >= __curr / 2 && pos < __curr;
}

int Min_heap::size() const {
    return __curr;
}

void Min_heap::__siftdown(int pos) {
    while (!__isLeaf(pos)) {
        int next_level = __left(pos);
        int tmp_level = __right(pos);
        if (tmp_level < __curr && comp(heap[tmp_level].first, heap[next_level].first))
            next_level = tmp_level;
        if (comp(heap[pos].first, heap[next_level].first))
            return;
        std::swap(heap[pos], heap[next_level]);
        pos = next_level;
    }
}

void Min_heap::build_heap() {
    __curr = __maxN;
    for (int idx = __curr / 2; idx >= 0; idx--)
        __siftdown(idx);
}

void Min_heap::init(Buffer &buffer, std::ifstream &inFile) {
    __curr = 0;
    for (int idx = 0; idx < block_num; idx++) {
        buffer.get_input(inFile);
        for (buffer.move_to_start(); !buffer.at_end(); buffer.move_to_next())
            heap[__curr++] = buffer.get_value();
    }
    build_heap();
}

pis Min_heap::remove_add(pis rec) {
    pis min_ = heap[0];
    heap[0] = rec;
    if (comp(rec.first, min_.first)) {
        std::swap(heap[0], heap[--__curr]);
    }
    if (!empty())
        __siftdown(0);
    return min_;
}

//________________________________________________

Buffer::Buffer() {
    __maxN = rec_max;
    __curr = 0;
    is_empty = true;
}

Buffer::~Buffer() { }

bool Buffer::get_input(std::ifstream &inFile, int move) {
    try {
        if (move != -1)
            inFile.seekg(move, std::ios::beg);
        inFile.read((char *)__buffer, sizeof(__buffer));
        is_empty = false;
    }
    catch (const std::exception&) {
        return false;
    }
    return true;
}

bool Buffer::write_output(std::ofstream &outFile) {
    if (is_empty) return false;
    try {
        outFile.write((char *)__buffer, sizeof(__buffer));
        is_empty = true;
    }
    catch (const std::exception&) {
        return false;
    }
    clear();
    return true;
}

pis Buffer::get_value() {
    return __buffer[__curr];
}

void Buffer::append_value(pis rec) {
    if (__curr < __maxN) {
        __buffer[__curr++] = rec;
    }
    is_empty = false;
}

void Buffer::move_to_start()  { 
    __curr = 0;
}

void Buffer::move_to_next() {
    __curr += (__curr < __maxN);
}

void Buffer::move_to_end() { 
    __curr = __maxN - 1; 
}

bool Buffer::empty() { 
    return is_empty; 
}

bool Buffer::at_end() {
    return __curr == __maxN;
}

int Buffer::get_curr() {
    return __curr;
}

void Buffer::clear() {
    __curr = 0;
    is_empty = true;
}