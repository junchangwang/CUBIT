
#include "fastbit/bitvector.h"

#include <iostream>
#include <sstream>
#include <sys/mman.h>
#include <tuple>

#include "fastbit/bitvector.h"
#include "table_lf.h"

using namespace nbub;
using namespace std;

bool Nbub::init_sequential_test()
{
    uint64_t last_rub = 0UL;
    int old_val = -1;
    int cnt0, cnt1, cnt2;

    rcu_register_thread();

    rcu_read_lock();
    
    // Save default merge threshhold.
    int default_merge_thresh = MERGE_THRESH;
    MERGE_THRESH = 4;

    #define ROW_ID (10000)
    
    // Test get_val()
    old_val = get_value_rcu(/*rowid*/ ROW_ID, g_timestamp, &last_rub);
    assert(last_rub == 0);
    assert(old_val != -1);
    assert(old_val == get_value_rcu(/*rowid*/ ROW_ID, g_timestamp, &last_rub));

    // Test update()
    int to_val = (old_val + 1) % cardinality;
    cnt0 = evaluate(0, old_val);
    cnt1 = evaluate(0, to_val);
    update(/*tid*/ 0, /*rowid*/ ROW_ID, to_val);
    assert(g_timestamp == 1);
    cnt2 = evaluate(0, old_val);
    assert(cnt2 == cnt0);  // This is because of the odd behavior of fastbit,
    // which returns the total number of bits (rather than 1's)
    // when ->do_cnt() is invoked. OMG.
    // Actually, what we need is the value in nset,
    // which unfortunately is thread-unsafe. 
    // Leave reading nset and performing assert(cnt2 == cnt0-1) as future work.

        // Test merge
    assert(bitmaps[old_val]->btv->getBit(ROW_ID, config) == 1);
    assert(bitmaps[to_val]->btv->getBit(ROW_ID, config) == 0);
    assert(get_value_rcu(/*rowid*/ ROW_ID, g_timestamp-1, &last_rub) == old_val);
    assert(last_rub == 0);
    assert(get_value_rcu(/*rowid*/ ROW_ID, g_timestamp, &last_rub) == to_val);
    assert(last_rub != 0);

    MERGE_THRESH = 1;

    cnt2 = evaluate(0, old_val);
    assert(g_timestamp == 2);

    cnt2 = evaluate(0, to_val);
    assert(g_timestamp == 3);

    assert(bitmaps[old_val]->btv->getBit(ROW_ID, config) == 0);
    assert(bitmaps[to_val]->btv->getBit(ROW_ID, config) == 1);

        // Test MVCC
    assert(get_value_rcu(/*rowid*/ ROW_ID, g_timestamp-3, &last_rub) == old_val);
    assert(last_rub != 0);
    assert(get_value_rcu(/*rowid*/ ROW_ID, g_timestamp-2, &last_rub) == to_val);
    assert(last_rub != 0);
    assert(get_value_rcu(/*rowid*/ ROW_ID, g_timestamp-1, &last_rub) == to_val);
    assert(last_rub != 0);
    assert(get_value_rcu(/*rowid*/ ROW_ID, g_timestamp, &last_rub) == to_val);
    assert(last_rub != 0);

    // Test append
    MERGE_THRESH = 4;
    int l_nor = g_number_of_rows;
    append(0, to_val);
    cnt2 = evaluate(0, to_val);    // does not perform merge
    assert(g_timestamp == 4);

    assert(cnt2 == cnt0);
    assert(bitmaps[old_val]->btv->getBit(l_nor, config) == 0);
    assert(bitmaps[to_val]->btv->getBit(l_nor, config) == 0);

        // Test Merge
    MERGE_THRESH = 1;
    cnt2 = evaluate(0, to_val); 
    assert(g_timestamp == 5);

    assert(bitmaps[old_val]->btv->getBit(l_nor, config) == 0);
    assert(bitmaps[to_val]->btv->getBit(l_nor, config) == 1);
        // Test MVCC
    assert(get_value_rcu(l_nor, g_timestamp-2, &last_rub) == -1);
    assert(get_value_rcu(l_nor, g_timestamp-1, &last_rub) == to_val);
    assert(get_value_rcu(l_nor, g_timestamp, &last_rub) == to_val);

    // Test delete
    MERGE_THRESH = 4;
    assert(get_value_rcu(ROW_ID, g_timestamp, &last_rub) == to_val);
    remove(0, ROW_ID);
    assert(g_timestamp == 6);

        // Before merge
    assert(bitmaps[to_val]->btv->getBit(ROW_ID, config) == 1);
    assert(get_value_rcu(ROW_ID, g_timestamp, &last_rub) == -1);
        // After merge
    MERGE_THRESH = 1;
    cnt2 = evaluate(0, to_val);
    assert(g_timestamp == 7);
    assert(bitmaps[to_val]->btv->getBit(ROW_ID, config) == 0);

    assert(get_value_rcu(ROW_ID, g_timestamp-2, &last_rub) == to_val);
    assert(get_value_rcu(ROW_ID, g_timestamp-1, &last_rub) == -1);
    assert(get_value_rcu(ROW_ID, g_timestamp, &last_rub) == -1);

    rcu_read_unlock();

    rcu_unregister_thread();

    MERGE_THRESH = default_merge_thresh;

    cout << "======================" << endl;
    cout << "== End of seq. test ==" << endl;
    cout << "======================" << endl;

    return true;
}
