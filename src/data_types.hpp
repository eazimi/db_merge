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
        S3: <string, string, string>
        T: tuple
    */

    using PS2 = pair<string, string>;
    using VEC_PS2 = vector<PS2>;
    using MAP_STR_VPS2 = map<string, VEC_PS2>;
    using TS3 = tuple<string, string, string>;
    using T_VS3 = tuple<vector<string>, vector<string>, vector<string>>;
    using T_MAP_SVPS2 = tuple<MAP_STR_VPS2, MAP_STR_VPS2, MAP_STR_VPS2>; 
    using T_VEC_PS2 = tuple<VEC_PS2, VEC_PS2, VEC_PS2>; 
    using VEC_TS3 = vector<TS3>;
    using PA_VS2 = pair<vector<string>, vector<string>>;
    using PA_MAP_S2 = pair<map<string, string>, map<string, string>>;
    using PA_MAP_SVPS2 = pair<MAP_STR_VPS2, MAP_STR_VPS2>;
    using PA_VEC_PS2 = pair<VEC_PS2, VEC_PS2>;
    using PA_PA_VS2 = pair<PA_VS2, PA_VS2>;
} // namespace Kaco

#endif