#ifndef DATA_TYPES_HPP
#define DATA_TYPES_HPP

#include <utility>
#include <string>
#include <vector>
#include <map>

using namespace std;

namespace Kaco
{

    // legend
    /*
        P, PA: pair
        S, STR: string
        V, VEC: vector
        MAP: map
        S2: <string, string>
    */

    using PS2 = pair<string, string>;
    using VEC_PS2 = vector<PS2>;
    using MAP_STR_VPS2 = map<string, VEC_PS2>;
    using PA_MAP_SVPS2 = pair<MAP_STR_VPS2, MAP_STR_VPS2>;
    using PA_VEC_PS2 = pair<VEC_PS2, VEC_PS2>;

} // namespace Kaco

#endif