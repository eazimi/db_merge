#ifndef GLOBAL_FUNCS_HPP
#define GLOBAL_FUNCS_HPP

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

namespace Kaco
{
    static pair<vector<string>, vector<string>> getDiff(vector<string> main, vector<string> ref)
    {
        vector<string> main_diff = {};
        vector<string> ref_diff = {};
        sort(main.begin(), main.end());
        sort(ref.begin(), ref.end());
        set_difference(main.begin(), main.end(), ref.begin(), ref.end(), back_inserter(main_diff));
        set_difference(ref.begin(), ref.end(), main.begin(), main.end(), back_inserter(ref_diff));
        return {main_diff, ref_diff};
    }

    static vector<string> getIntersect(vector<string> main, vector<string> ref)
    {
        vector<string> intersect = {};
        sort(main.begin(), main.end());
        sort(ref.begin(), ref.end());
        set_intersection(main.begin(), main.end(), ref.begin(), ref.end(),
                         std::back_inserter(intersect));
        sort(intersect.begin(), intersect.end());
        return intersect;
    }

} // namespace Kaco

#endif