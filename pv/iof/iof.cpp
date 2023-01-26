#include "iof.hpp"

#include <iostream>
#include <deque>

#include "../data/logicsegment.hpp"

using std::shared_ptr;
using std::deque;

namespace iof {

const size_t BlockSize = 10 * 1024 * 1024;

// TODO: scale to more than 1 channel
// TODO: support analog
// void iof_generate_proto(const std::unordered_set< std::shared_ptr<pv::data::SignalData> >& all_signal_data)
void iof_generate_proto(const shared_ptr<pv::data::Logic>& logic_data)
{
    for (const shared_ptr<pv::data::LogicSegment>& s : logic_data->logic_segments())
    {
        // TODO: store like they do in storesession.c
        const int unit_size = s->unit_size();
        const int samples_per_block = BlockSize / unit_size;

        uint64_t sample_count = s->get_sample_count();
        uint64_t start_sample = 0;

        while (sample_count > 0)
        {
            const uint64_t packet_len = std::min((uint64_t)samples_per_block, sample_count);
            const size_t data_size = packet_len * unit_size;

            uint8_t* data = new uint8_t[data_size];
            s->get_samples(start_sample, start_sample + packet_len, data);

            delete[] data;
            sample_count -= packet_len;
            start_sample += packet_len;
        }
    }
}

}
