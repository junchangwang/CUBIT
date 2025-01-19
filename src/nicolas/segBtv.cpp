#include <map>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>
#include "nicolas/segBtv.h"
#include "fastbit/bitvector.h"

using namespace std;

SegBtv::SegBtv(Table_config *config, ibis::bitvector * btv)
{
    n_rows = config->n_rows;
    uint64_t row_id = n_rows - 1;

    // We want each segment to maintain a fixed number of words.
    // Therefore, we first decompressed btv to simplify the following copy operation.
    btv->decompress();

    encoded_word_len = config->encoded_word_len;
    rows_per_seg = (config->rows_per_seg / encoded_word_len) * encoded_word_len;
    words_per_seg = rows_per_seg / encoded_word_len;
    assert(rows_per_seg % encoded_word_len == 0);
    num_segs = (row_id / rows_per_seg) + (row_id % rows_per_seg != 0); // Get the ceiling

    // Copy the decompressed bitvector btv to segments.
    ibis::bitvector::run x;
    x.it = btv->m_vec.begin();
    for (uint32_t idx = 0; idx < num_segs; idx++) 
    {   
        btv_seg seg;
        seg.id = idx;
        seg.start_row = idx * rows_per_seg;
        seg.end_row = (idx+1) * rows_per_seg -1;
        assert(seg.btv);

        if (x.nWords == 0)
            x.decode();

        uint32_t nw = words_per_seg;
        // The last segment has a portion of words.
        if (idx == num_segs-1) {
            nw = (row_id / encoded_word_len) - (words_per_seg * idx);
        }

        // Copy nw words from btv to seg->btv.
        seg.btv->copy_runs(x, nw);
        assert(!nw);

        // WAH compression
        seg.btv->compress();

        // The last segment contains the active word.
        if (idx == num_segs-1) {
            seg.btv->active.nbits = btv->active.nbits;
            seg.btv->active.val = btv->active.val;
        }
        
        if (config->enable_fence_pointer)
            seg.btv->buildIndex();

        seg_table[idx] = seg;
    }

    // cout << "Succeed in building SegBtv. To verify it." << endl;
    // Verify(config, btv);
    // cout << endl;
    
}

SegBtv::SegBtv(const SegBtv &rhs) : encoded_word_len(rhs.encoded_word_len), num_segs(rhs.num_segs),
    rows_per_seg(rhs.rows_per_seg), words_per_seg(rhs.words_per_seg), n_rows(rhs.n_rows)
{
    for (const auto & [id_t, seg_t] : rhs.seg_table) {
        btv_seg seg = btv_seg(seg_t);
        seg_table[id_t] = seg;
    }
}

SegBtv::~SegBtv() 
{
    for (auto & [id_t, seg_t] : seg_table) 
    {
        delete seg_t.btv;
    }
}

int SegBtv::deepCopy(const SegBtv &rhs)
{
    for (const auto & [id_t, seg_t] : rhs.seg_table) {
        seg_table[id_t].btv->copy(*seg_t.btv);
    }

    return 0;
}

btv_seg SegBtv::getSeg(uint64_t row_id)
{
    btv_seg seg;
    uint32_t idx = row_id / rows_per_seg;

    assert(seg_table.count(idx));
    seg = seg_table[idx];

    assert(seg.btv);
    assert((row_id >= seg.start_row) && (row_id <= seg.end_row));

    return seg;
}

int SegBtv::getBit(uint64_t row, Table_config *config) 
{
    btv_seg seg = getSeg(row);
    uint64_t row_in_seg = row - seg.start_row;

    int ret = seg.btv->getBit(row_in_seg, config);
    
    return ret;
}

void SegBtv::setBit(uint64_t row, int val, Table_config *config)
{
    btv_seg seg = getSeg(row);
    uint64_t row_in_seg = row - seg.start_row;

    seg.btv->setBit(row_in_seg, val, config);

    return;
}

int SegBtv::buildIndex(uint64_t row_id)
{
    int seg_id = row_id / rows_per_seg;

    assert(seg_table.count(seg_id));

    seg_table[seg_id].btv->index.clear();
    seg_table[seg_id].btv->buildIndex();

    return 0;
}

int SegBtv::buildAllIndex()
{
    for (const auto& [id_t, seg_t] : seg_table) {
        seg_t.btv->index.clear();
        seg_t.btv->buildIndex();
    }

    return 0;
}

int SegBtv::adjustSize(uint64_t first, uint64_t second)
{
    if (first >= n_rows) {
        cout << "[WARNING: SegBtv::adjustSize]: first >= n_rows. We do not support this yet." << endl;
        return -1;
    }

    int to_append = second - n_rows;
    if (to_append <= 0)
        return -1;

    uint64_t n_rows_t = n_rows;
    btv_seg seg = getSeg(n_rows_t-1);
    n_rows_t -= seg.start_row;

    if (n_rows_t + to_append <= rows_per_seg) {
        seg.btv->adjustSize(0, n_rows_t + to_append);
    }
    else {
        // Need to create a new segment.
        seg.btv->adjustSize(0, rows_per_seg);
        if (enable_fence_pointer)
            seg.btv->buildIndex();
        to_append = n_rows_t + to_append - rows_per_seg;

        // Do not support appending multiple segments once.
        assert(to_append <= rows_per_seg);

        uint32_t idx = seg_table.size();
        btv_seg seg_new{};
        seg_new.id = idx;
        seg_new.start_row = idx * rows_per_seg;
        seg_new.end_row = (idx+1) * rows_per_seg - 1;
        assert(seg_new.btv);

        seg_new.btv->adjustSize(0, to_append);
        if (enable_fence_pointer)
            seg_new.btv->buildIndex();

        seg_table[idx] = seg_new;
    }

    n_rows = second;

    return 0;
}

void SegBtv::decompress()
{
    for (const auto & [id_t, seg_t] : seg_table) {
        seg_t.btv->decompress();
    }
}

uint64_t SegBtv::do_cnt()
{
    uint64_t cnt = 0UL;

    for (const auto & [id_t, seg_t] : seg_table)
        cnt += seg_t.btv->do_cnt();

    return cnt;
}

uint64_t SegBtv::do_cnt_parallel(Table_config *config) 
{
    uint64_t cnt = 0UL;

    int num_seg = seg_table.size();
    int n_threads = (config->nThreads_for_getval > num_seg) ? num_seg : config->nThreads_for_getval;
    int n_seg_per_thread = num_seg / n_threads;
    int n_left = num_seg % n_threads;

    assert(n_seg_per_thread >= 1);
    
    thread* threads = new thread[n_threads];
    vector<uint64_t> n_cnt(n_threads, 0);       /// record the cnt of different seg
    vector<int> begin(n_threads + 1, 0);   /// record the seg_id run in different threads

    for (int i = 1; i <= n_left; i++)
        begin[i] = begin[i - 1] + n_seg_per_thread + 1;
    for (int i = n_left + 1; i <= n_threads; i++)
        begin[i] = begin[i - 1] + n_seg_per_thread;

    for (int i = 0; i < n_threads; i++) {
        threads[i] = thread(&SegBtv::_cnt_in_thread, this, &n_cnt[i], begin[i], begin[i + 1]);
    }
    for (int i = 0; i < n_threads; i++) {
        threads[i].join();
        cnt += n_cnt[i];
    }
    delete[] threads;
    
    return cnt;
}

void SegBtv::_cnt_in_thread(uint64_t *cnt, int begin, int end) {
    auto iter1 = seg_table.find(begin);
    auto iter2 = seg_table.find(end);
    for (; iter1 != iter2; iter1++)
        *cnt += iter1->second.btv->do_cnt();
}

int SegBtv::Verify(Table_config *config, ibis::bitvector * const btv)
{
    //  uint64_t j = 0;
    //  for (uint64_t i = n_rows-1; i >= 0; i--) {
    //     if (getBit(i, config) != btv->getBit(i, config)) {
    //         cout << "SegBtv::Verify failed on line " << i
    //             << " SegBtv:btv " << getBit(i, config) << " " << btv->getBit(i, config) << endl;
    //         return -1;
    //     }
    //     j++;
    //     if (j % 10000 == 0)
    //         cout << "Successfully verify " << j << " entries." << endl;
    // }

    int N_TEST = 10000;
    for (int idx = 0; idx < N_TEST; idx++) 
    {
        int row_id;
        
        if (idx <= N_TEST/10) {
            row_id = n_rows - 1 - idx;
        } 
        else if (idx <= 2*N_TEST/10) {
            row_id = idx;
        }
        else {
            row_id = rand() % (n_rows-1);
        }
        
        auto t1 = std::chrono::high_resolution_clock::now();
        int val_segbtv = getBit(row_id, config);
        auto t2 = std::chrono::high_resolution_clock::now();
        int val_btv = btv->getBit(row_id, config);
        auto t3 = std::chrono::high_resolution_clock::now();

        assert(val_segbtv == val_btv);

        setBit(row_id, !val_segbtv, config);
        if (config->enable_fence_pointer) {
            buildIndex(row_id);
        }
        auto t4 = std::chrono::high_resolution_clock::now();
        btv->setBit(row_id, !val_btv, config);
        if (config->enable_fence_pointer) {
            btv->index.clear();
            btv->buildIndex();
        }
        auto t5 = std::chrono::high_resolution_clock::now();

        assert(getBit(row_id, config) == btv->getBit(row_id, config));

        cout << "Test " << idx << " on row " << row_id << " :" << endl
            << "[SegBtv] getBit(us): " << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()
            << "     setBit(us): " << std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count() << endl
            << "[Btv]    getBit(us): " << std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count()
            << "     setBit(us): " << std::chrono::duration_cast<std::chrono::microseconds>(t5-t4).count() << endl << endl;
    }

    return 0;
}

void SegBtv::_and_in_thread(SegBtv &rhs, int begin, int end)
{
    auto iter1 = seg_table.find(begin);
    auto iter2 = seg_table.find(end);
    for (; iter1 != iter2; iter1++) {
        if(rhs.seg_table.count(iter1->first)) {
            assert(rhs.seg_table.count(iter1->first));
            ibis::bitvector *btv_1 = iter1->second.btv;
            ibis::bitvector *btv_2 = rhs.seg_table[iter1->first].btv;
            (*btv_1).operator&= (*btv_2);
        }
        else {
            ibis::bitvector tmp_btv;
            ibis::bitvector *btv_1 = iter1->second.btv;
            (*btv_1) &= tmp_btv;
        }
    }

    return;
}

void SegBtv::operator^=(SegBtv &rhs) 
{
    assert(this->encoded_word_len >= rhs.encoded_word_len);
    assert(this->n_rows >= rhs.n_rows);

    for (const auto & [id_t, seg_1] : rhs.seg_table) {
        assert(seg_table.count(id_t));
        ibis::bitvector *btv_1 = seg_table[id_t].btv;
        ibis::bitvector *btv_2 = seg_1.btv;
        if(btv_1->size() > btv_2->size()) {
            ibis::bitvector btv_copy;
            btv_copy.copy(*btv_2);
            (*btv_1).operator^= (btv_copy);
        }
        else {
            (*btv_1).operator^= (*btv_2);
        }
        btv_1->decompress();
    }

    return;
}

void SegBtv::operator|=(SegBtv &rhs) 
{
    assert(this->encoded_word_len >= rhs.encoded_word_len);
    assert(this->n_rows >= rhs.n_rows);

    for (const auto & [id_t, seg_1] : rhs.seg_table) {
        assert(seg_table.count(id_t));
        ibis::bitvector *btv_1 = seg_table[id_t].btv;
        ibis::bitvector *btv_2 = seg_1.btv;
        (*btv_1).operator|= (*btv_2);
    }

    return;
}

SegBtv * SegBtv::operator^(SegBtv &rhs)
{
    assert(this->encoded_word_len == rhs.encoded_word_len);
    assert(this->n_rows == rhs.n_rows);

    SegBtv *segbtv_new = new SegBtv(rhs);
    
    for (auto & [id_t, seg_1] : segbtv_new->seg_table) {
        assert(this->seg_table.count(id_t));
        assert(rhs.seg_table.count(id_t));
        delete seg_1.btv;
        seg_1.btv = *(seg_table[id_t].btv) ^ *(rhs.seg_table[id_t].btv);
    }

    return segbtv_new;
}

vector<uint32_t> SegBtv::decode(vector<uint32_t> &append_rowids, Table_config *config)
{
    for (const auto & [id_t, seg_1] : seg_table) {
        seg_1.btv->decode(append_rowids, config);
    }

    return append_rowids;
}
