#include <iostream>
#include <sstream>
#include <sys/mman.h>
#include <tuple>
#include <chrono>

#include "fastbit/bitvector.h"
#include "nicolas/base_table.h"
#include "table.h"

using namespace cubit;
using namespace std;

bool Cubit::init_sequential_test()
{
    int old_val = -1;
    int cnt0, cnt1, cnt2;

    rcu_register_thread();

    rcu_read_lock();

    int default_merge_thresh = merge_threshold;

    int ROW_ID;
    
    if (config->encoding == EE) 
    {
        merge_threshold = 4;

        ROW_ID = 10000;

        RUB last_rub = RUB{0, TYPE_INV, {}};
        
        // get_val()
        old_val = get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp), last_rub);
        assert(last_rub.type == TYPE_INV);
        assert(old_val != -1);
        assert(old_val == get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp), last_rub));

        // update()
        int to_val = (old_val + 1) % cardinality;
        cnt0 = evaluate(0, old_val);
        cnt1 = evaluate(0, to_val);
        update(/*tid*/ 0, /*rowid*/ ROW_ID, to_val);
        // The initial value of timestamp of DBx1000 is 1. We stick to this.
        // Therefore, a successful Update increments it to 2.
        assert(READ_ONCE(g_timestamp) == 2);
        cnt2 = evaluate(0, old_val);
        // assert(cnt2 == cnt0);  // This is because of the odd behavior of fastbit,
        // which returns the total number of bits (rather than 1's)
        // when ->do_cnt() is invoked. OMG.
        // Actually, what we need is the value in nset,
        // which unfortunately is thread-unsafe. 
        // Leave reading nset and performing assert(cnt2 == cnt0-1) as future work.

        // merge()
        if (config->segmented_btv) {
            assert(bitmaps[old_val]->seg_btv->getBit(ROW_ID, config) == 1);
            assert(bitmaps[to_val]->seg_btv->getBit(ROW_ID, config) == 0);
        }
        else {
            assert(bitmaps[old_val]->btv->getBit(ROW_ID, config) == 1);
            assert(bitmaps[to_val]->btv->getBit(ROW_ID, config) == 0);
        }
        assert(get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp)-1, last_rub) == old_val);
        assert(last_rub.type == TYPE_INV);
        assert(get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp), last_rub) == to_val);
        assert(last_rub.type != TYPE_INV);

        merge_threshold = 1;

        cnt2 = evaluate(0, old_val);
        std::this_thread::sleep_for(1s);
        assert(READ_ONCE(g_timestamp) == 3);

        cnt2 = evaluate(0, to_val);
        std::this_thread::sleep_for(1s);
        assert(READ_ONCE(g_timestamp) == 4);

        if (config->segmented_btv) {
            assert(bitmaps[old_val]->seg_btv->getBit(ROW_ID, config) == 0);
            assert(bitmaps[to_val]->seg_btv->getBit(ROW_ID, config) == 1);
        }
        else {
            assert(bitmaps[old_val]->btv->getBit(ROW_ID, config) == 0);
            assert(bitmaps[to_val]->btv->getBit(ROW_ID, config) == 1);
        }

        // MVCC
        assert(get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp)-3, last_rub) == old_val);
        assert(last_rub.type != TYPE_INV);
        assert(get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp)-2, last_rub) == to_val);
        assert(last_rub.type != TYPE_INV);
        assert(get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp)-1, last_rub) == to_val);
        assert(last_rub.type != TYPE_INV);
        assert(get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp), last_rub) == to_val);
        assert(last_rub.type != TYPE_INV);

        // append()
        merge_threshold = 4;
        int l_nor = g_number_of_rows;
        append(0, to_val);
        cnt2 = evaluate(0, to_val);    // does not perform merge
        std::this_thread::sleep_for(1s);
        assert(READ_ONCE(g_timestamp) == 5);

        // assert(cnt2 == cnt0);
        if (config->segmented_btv) {
            assert(bitmaps[old_val]->seg_btv->getBit(l_nor, config) == 0);
            assert(bitmaps[to_val]->seg_btv->getBit(l_nor, config) == 0);
        }
        else {
            assert(bitmaps[old_val]->btv->getBit(l_nor, config) == 0);
            assert(bitmaps[to_val]->btv->getBit(l_nor, config) == 0);
        }

        // merge() after append()
        merge_threshold = 1;
        cnt2 = evaluate(0, to_val); 
        std::this_thread::sleep_for(1s);
        assert(READ_ONCE(g_timestamp) == 6);

        if (config->segmented_btv) {
            assert(bitmaps[old_val]->seg_btv->getBit(l_nor, config) == 0);
            assert(bitmaps[to_val]->seg_btv->getBit(l_nor, config) == 1);
        }
        else {
            assert(bitmaps[old_val]->btv->getBit(l_nor, config) == 0);
            assert(bitmaps[to_val]->btv->getBit(l_nor, config) == 1);
        }
        // TMVCC after merge() after append()
        assert(get_value_rcu(l_nor, READ_ONCE(g_timestamp)-2, last_rub) == -1);
        assert(get_value_rcu(l_nor, READ_ONCE(g_timestamp)-1, last_rub) == to_val);
        assert(get_value_rcu(l_nor, READ_ONCE(g_timestamp), last_rub) == to_val);

        // delete()
        merge_threshold = 4;
        assert(get_value_rcu(ROW_ID, READ_ONCE(g_timestamp), last_rub) == to_val);
        remove(0, ROW_ID);
        assert(READ_ONCE(g_timestamp) == 7);

        // Before merge
        // assert(bitmaps[to_val]->btv->getBit(ROW_ID, config) == 1);
        assert(get_value_rcu(ROW_ID, READ_ONCE(g_timestamp), last_rub) == -1);
        // merge() after delete()
        merge_threshold = 1;
        cnt2 = evaluate(0, to_val);
        std::this_thread::sleep_for(1s);
        assert(READ_ONCE(g_timestamp) == 8);
        if (config->segmented_btv) {
            assert(bitmaps[to_val]->seg_btv->getBit(ROW_ID, config) == 0);
        }
        else {
            assert(bitmaps[to_val]->btv->getBit(ROW_ID, config) == 0);
        }

        assert(get_value_rcu(ROW_ID, READ_ONCE(g_timestamp)-2, last_rub) == to_val);
        assert(get_value_rcu(ROW_ID, READ_ONCE(g_timestamp)-1, last_rub) == -1);
        assert(get_value_rcu(ROW_ID, READ_ONCE(g_timestamp), last_rub) == -1);
    }
    else if (config->encoding == RE || config->encoding == AE) 
    {
        RUB last_rub = RUB{0, TYPE_INV, {}};

        merge_threshold = 4;

        ROW_ID = 10000;

        int TAG = 2;
        
        // get_val()
        old_val = get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp), last_rub);
        assert(last_rub.type == TYPE_INV);
        assert(old_val != -1);
        assert(old_val == get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp), last_rub));

        // update()
        int to_val = (old_val + TAG) % cardinality;
        cnt0 = evaluate(0, old_val);
        cnt1 = evaluate(0, to_val);
        update(/*tid*/ 0, /*rowid*/ ROW_ID, to_val);
        // The initial value of timestamp of DBx1000 is 1. We stick to this.
        // Therefore, a successful Update increments it to 2.
        assert(READ_ONCE(g_timestamp) == 2);
        cnt2 = evaluate(0, old_val);
        // assert(cnt2 == cnt0);  // This is because of the odd behavior of fastbit,
        // which returns the total number of bits (rather than 1's)
        // when ->do_cnt() is invoked. OMG.
        // Actually, what we need is the value in nset,
        // which unfortunately is thread-unsafe. 
        // Leave reading nset and performing assert(cnt2 == cnt0-1) as future work.

        // merge()
        if (config->segmented_btv) {
            assert(bitmaps[old_val]->seg_btv->getBit(ROW_ID, config) == 1);
        }
        else {
            assert(bitmaps[old_val]->btv->getBit(ROW_ID, config) == 1);
        }
        assert(get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp)-1, last_rub) == old_val);
        assert(last_rub.type == TYPE_INV);
        assert(get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp), last_rub) == to_val);
        assert(last_rub.type != TYPE_INV);

        merge_threshold = 1;

        cnt2 = evaluate(0, old_val);
        std::this_thread::sleep_for(1s);
        assert(READ_ONCE(g_timestamp) == 3);

        if (config->segmented_btv) {
            assert(bitmaps[old_val]->seg_btv->getBit(ROW_ID, config) == 0);
        }
        else {
            assert(bitmaps[old_val]->btv->getBit(ROW_ID, config) == 0);
        }

        // MVCC
        assert(get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp)-2, last_rub) == old_val);
        assert(last_rub.type != TYPE_INV);
        assert(get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp)-1, last_rub) == to_val);
        assert(last_rub.type != TYPE_INV);
        assert(get_value_rcu(/*rowid*/ ROW_ID, READ_ONCE(g_timestamp), last_rub) == to_val);
        assert(last_rub.type != TYPE_INV);

        // append()
        merge_threshold = 4;
        int l_nor = g_number_of_rows;
        append(0, to_val);
        cnt2 = evaluate(0, to_val);    // does not perform merge
        std::this_thread::sleep_for(1s);
        assert(READ_ONCE(g_timestamp) == 4);

        if (config->segmented_btv) {
            assert(bitmaps[old_val]->seg_btv->getBit(l_nor, config) == 0);
        }
        else {
            assert(bitmaps[old_val]->btv->getBit(l_nor, config) == 0);
        }

        // merge() after append()
        merge_threshold = 1;
        cnt2 = evaluate(0, to_val); 
        std::this_thread::sleep_for(1s);
        assert(READ_ONCE(g_timestamp) == 5);

        if (config->segmented_btv) {
            assert(bitmaps[to_val]->seg_btv->getBit(l_nor, config) == 1);
        }
        else {
            assert(bitmaps[to_val]->btv->getBit(l_nor, config) == 1);
        }
        // TMVCC after merge() after append()
        assert(get_value_rcu(l_nor, READ_ONCE(g_timestamp)-2, last_rub) == -1);
        assert(get_value_rcu(l_nor, READ_ONCE(g_timestamp)-1, last_rub) == to_val);
        assert(get_value_rcu(l_nor, READ_ONCE(g_timestamp), last_rub) == to_val);

        // delete()
        merge_threshold = 4;
        assert(get_value_rcu(ROW_ID, READ_ONCE(g_timestamp), last_rub) == to_val);
        remove(0, ROW_ID);
        assert(READ_ONCE(g_timestamp) == 6);

        // Before merge
        if (config->segmented_btv) {
            assert(bitmaps[to_val]->seg_btv->getBit(ROW_ID, config) == 1);
        }
        else {
            assert(bitmaps[to_val]->btv->getBit(ROW_ID, config) == 1);
        }
        assert(get_value_rcu(ROW_ID, READ_ONCE(g_timestamp), last_rub) == -1);
        
        // merge() after delete()
        merge_threshold = 1;
        cnt2 = evaluate(0, to_val);
        std::this_thread::sleep_for(1s);
        assert(READ_ONCE(g_timestamp) == 7);
        if (config->segmented_btv) {
            assert(bitmaps[to_val]->seg_btv->getBit(ROW_ID, config) == 0);
        }
        else {
            assert(bitmaps[to_val]->btv->getBit(ROW_ID, config) == 0);
        }
        
        assert(get_value_rcu(ROW_ID, READ_ONCE(g_timestamp)-2, last_rub) == to_val);
        assert(get_value_rcu(ROW_ID, READ_ONCE(g_timestamp)-1, last_rub) == -1);
        assert(get_value_rcu(ROW_ID, READ_ONCE(g_timestamp), last_rub) == -1);
    }

    rcu_read_unlock();

    rcu_unregister_thread();

    merge_threshold = default_merge_thresh;

    cout << "======================" << endl;
    cout << "== End of seq. test ==" << endl;
    cout << "======================" << endl;

    return true;
}
