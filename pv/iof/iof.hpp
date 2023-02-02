#ifndef IOF_HPP
#define IOF_HPP

#include <unordered_set>

#include "../data/logic.hpp"

namespace iof {

void iof_generate_json(const shared_ptr<pv::data::Logic>& logic_data);

}

#endif // IOF_HPP
