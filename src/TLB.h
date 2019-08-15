#pragma once

#include <vector>
#include <cassert>
#include <cmath>
#include <limits>
#include <iostream>

struct TLBEntry
{
    TLBEntry() = default;
    TLBEntry(void* address, size_t acc_cnt) : address(address), acc_cnt(acc_cnt)
    {

    }

    void update(void* address, size_t acc_cnt)
    {
        this->address = address;
        this->acc_cnt = acc_cnt;
    }

    void* address = nullptr;
    size_t acc_cnt = 0;
};

class TLB
{
public:
    static const size_t INDEX_BITS = 12; // 4K pages

    TLB(size_t entry_count, size_t num_ways) : num_ways_(num_ways), counter_(0), cache_table_(std::max(entry_count, num_ways))
    {
        size_t count = std::max(entry_count, num_ways);
        this->entry_count = count;

        num_rows_ = this->entry_count / num_ways;
        assert(!(this->entry_count & (this->entry_count - 1)));
        this->entry_bits = std::log2(this->entry_count);
    }

    size_t get_index(void* address)
    {
        size_t max = std::numeric_limits<size_t>::max();    // all 1s
        size_t shifted = max >> (64UL - this->entry_bits);  // keep log2(entry_count) 1s
        shifted <<= INDEX_BITS;                             // move 1s to position of address index
        auto value = reinterpret_cast<size_t>(address);
        return (value & shifted) >> INDEX_BITS;             // extract index from address
    }
    bool compare_tag(void* a, void* b)
    {
        size_t shift = INDEX_BITS + this->entry_bits;
        auto vala = reinterpret_cast<size_t>(a);
        auto valb = reinterpret_cast<size_t>(b);
        return (vala >> shift) == (valb >> shift);
    }

    bool get(void* address)
    {
        increment_counter();

        size_t id = this->get_index(address);
        size_t row_id = id % num_rows_;
        size_t cache_table_idx = row_id * num_ways_;
        for (size_t i = cache_table_idx; i < cache_table_idx + num_ways_; i++)
        {
            if (compare_tag(cache_table_[i].address, address))
            {
                cache_table_[i].acc_cnt = counter_;
                return true;
            }
        }
        return false;
    }
    void update(void* address)
    {
        increment_counter();

        size_t id = this->get_index(address);
        size_t row_id = id % num_rows_;
        size_t cache_table_idx = row_id * num_ways_;
        size_t min = std::numeric_limits<unsigned int>::max();
        size_t min_idx = 0;
        bool found_empty = false;
        size_t empty_idx = 0;

        for (size_t i = cache_table_idx; i < cache_table_idx + num_ways_; i++)
        {
            if (compare_tag(cache_table_[i].address, address))
            {
                cache_table_[i].update(address, counter_);
                return;
            }
            else if (cache_table_[i].address == nullptr && !found_empty)
            {
                found_empty = true;
                empty_idx = i;
            }
            else if (cache_table_[i].acc_cnt < min)
            {
                min = cache_table_[i].acc_cnt;
                min_idx = i;
            }
        }

        if (found_empty)
        {
            cache_table_[empty_idx].update(address, counter_);
        }
        else // Reaching here means that eviction is necessary
        {
            cache_table_[min_idx].update(address, counter_);
        }
    }

private:
    void increment_counter()
    {
        counter_++;
    }

    size_t entry_count;
    size_t entry_bits;
    size_t num_ways_;
    size_t num_rows_;

    size_t counter_;

    // Cache table is flattened to a single vector. The rows and ways are enforced by the accessor functions.
    std::vector<TLBEntry> cache_table_;
};
