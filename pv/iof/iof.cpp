#include "iof.hpp"

#include <iostream>
#include <fstream>
#include <string>

#ifdef ENABLE_DECODE
#include <libsigrokdecode/libsigrokdecode.h> /* First, so we avoid a _POSIX_C_SOURCE warning. */
#endif

#include "../data/logicsegment.hpp"
#include "../../proto/logic.pb.h"

using std::shared_ptr;
using std::ios;

namespace iof {

const size_t BLOCK_SIZE = 10 * 1024 * 1024;
const std::string OUTPUT_FILENAME = "../output/logic_proto.bin";

// TODO: scale to more than 1 channel
// TODO: support analog
// void iof_generate_proto(const std::unordered_set< std::shared_ptr<pv::data::SignalData> >& all_signal_data)
void iof_generate_proto(const shared_ptr<pv::data::Logic>& logic_data)
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    logic_proto::session logic_proto_session;
    // TODO: set period properly
    logic_proto_session.set_period(0);

    for (const shared_ptr<pv::data::LogicSegment>& s : logic_data->logic_segments())
    {
        // TODO: store like they do in storesession.c
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

            for(size_t i = 0; i < data_size; i++)
            {
                logic_proto_session.add_data(data[i]);
            }

            delete[] data;
            sample_count -= packet_len;
            start_sample += packet_len;
        }
    }

    // Write to disk
    std::fstream output(OUTPUT_FILENAME, ios::out | ios::trunc | ios::binary);
    if (!logic_proto_session.SerializeToOstream(&output))
    {
        std::cerr << "Proto write error: " << std::strerror(errno) << "\n";
    }

    google::protobuf::ShutdownProtobufLibrary();

#ifdef ENABLE_DECODE
		// Destroy libsigrokdecode
		srd_exit();
#endif
    // TODO: Crashing terminal and not getting "Cleaning up all drivers." message
    exit(0);
}

}
