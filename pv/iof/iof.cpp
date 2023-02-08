#include "iof.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>

#include "../data/logicsegment.hpp"
// TODO: install locally instead of using submodule
#include "../../json/single_include/nlohmann/json.hpp"

using std::shared_ptr;
using std::ios;
using json = nlohmann::json;

namespace iof {

const size_t BLOCK_SIZE = 10 * 1024 * 1024;
const std::string OUTPUT_FILENAME = "../output/output.json";

// TODO: scale to more than 1 channel
// TODO: support analog
void iof_generate_json(const shared_ptr<pv::data::Logic>& logic_data)
{
    json j;
    j["period"] = logic_data->get_samplerate();

    std::vector<uint8_t> results;

    for (const shared_ptr<pv::data::LogicSegment>& s : logic_data->logic_segments())
    {
        // based off storesession.c
        const int unit_size = s->unit_size();
        const int samples_per_block = BLOCK_SIZE / unit_size;

        uint64_t sample_count = s->get_sample_count();
        uint64_t start_sample = 0;

        while (sample_count > 0)
        {
            const uint64_t packet_len = std::min((uint64_t)samples_per_block, sample_count);
            const size_t data_size = packet_len * unit_size;

            uint8_t* data = new uint8_t[data_size];
            s->get_samples(start_sample, start_sample + packet_len, data);

            // TODO: speed this up - avoid copying
            results.insert(results.end(), &data[0], &data[data_size - 1]);

            delete[] data;
            sample_count -= packet_len;
            start_sample += packet_len;
        }
    }

    j["data"] = results;

    std::ofstream o(OUTPUT_FILENAME);
    o << j << std::endl;

    // TODO: verify generated JSON against schema
}

}
