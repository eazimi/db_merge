/*
    creates a message in the following forms:
    
    msg_text [schema::tbl_name]
    [indent] col_names
    caption_text: caption_value
    ...
    caption_text: caption_value


*/

#ifndef LOG_H
#define LOG_H

#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <memory>

using namespace std;

namespace Kaco
{
    class LogBuilder;
    
    class Log
    {
    private:
        friend class LogBuilder;
        int indent = 0;
        string schema = "";
        string tbl_name = "";
        string msg_text = "";
        vector<string> msg_multi{};
        string col_names = "";
        vector<vector<string>> data{};
        vector<string> captions{};
        ostringstream oss{};
        Log() {}

    public:
        ~Log() {}
        Log(Log &&other);
        Log(const Log &other);
        Log &operator=(Log &&other);
        static unique_ptr<LogBuilder> create(int indent = 0);
        string str_records();
        string str_no_pk();
        string str_diff_tbls();
    };
} // namespace Kaco

#endif