#include "iof.hpp"

#include <iostream>

namespace iof {

// TODO: scale to more than 1 channel
// TODO: support analog
void iof_generate_proto(const shared_ptr<pv::data::Logic>& logic_data)
{
    for(const auto& d : logic_data->logic_segments())
    {
        std::cout << "segment \n";
    }
}

}
