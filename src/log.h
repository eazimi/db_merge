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
        string col_names = "";
        vector<pair<string, string>> captions; // vector<<caption_text, caption_value>>
        ostringstream oss;
        Log() {}

    public:
        ~Log() {}
        Log(Log &&other);
        Log(const Log &other);
        Log &operator=(Log &&other);
        static LogBuilder create(int indent = 0);
        string str();
    };
} // namespace Kaco

#endif