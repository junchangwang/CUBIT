#ifndef SEG_BTV_H_
#define SEG_BTV_H_

#include <map>

#include "fastbit/bitvector.h"

// using namespace std;

struct btv_seg {
    int id;
    uint64_t start_row, end_row;
    ibis::bitvector *btv;

    btv_seg() : id(-1), start_row(0), end_row(0) {btv = new ibis::bitvector();};
    btv_seg(const btv_seg &rhs) : id(rhs.id), start_row(rhs.start_row), end_row(rhs.end_row) {btv = new ibis::bitvector();};
};

class SegBtv {
    private:
        uint32_t encoded_word_len;
        uint32_t num_segs;
        uint64_t rows_per_seg;
        uint64_t words_per_seg;
        uint64_t n_rows;


        btv_seg getSeg(uint64_t);

        // This function verifies the correctness of the newly-built segmented btv.
        // It runs very very slow.
        int Verify(Table_config *, ibis::bitvector * const);

    public:
        std::map<uint32_t, btv_seg> seg_table{};

        SegBtv(Table_config *, ibis::bitvector *);
        SegBtv(const SegBtv &rhs);
        ~SegBtv();

        int deepCopy(const SegBtv &rhs);

        int getBit(uint64_t, Table_config *);
        void setBit(uint64_t, int, Table_config *);

        uint64_t do_cnt();
        uint64_t do_cnt_parallel(Table_config *);
        void _cnt_in_thread(uint64_t* , int , int);
        uint64_t get_rows() {return n_rows;};
        int adjustSize(uint64_t, uint64_t);
        void decompress();

        int buildIndex(uint64_t);
        int buildAllIndex();

        void _and_in_thread(SegBtv&, int, int);
        ///Perform bitwise exclusive or (XOR).
        void operator^=(SegBtv &rhs);
        void operator|=(SegBtv &rhs);

        ///Perform bitwise XOR and return the result as a new bitvector.
        SegBtv *operator^(SegBtv &rhs);

        std::vector<uint32_t> decode(std::vector<uint32_t> &append_rowids, Table_config *config);
};

#endif



