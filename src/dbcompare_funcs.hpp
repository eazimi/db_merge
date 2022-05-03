#ifndef DBCOMPARE_FUNC_HPP
#define DBCOMPARE_FUNC_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <sstream>
#include <stack>
#include <memory>
#include <tuple>
#include <algorithm>
#include "IDbReader.hpp"
#include "dbcompare_defines.hpp"

using namespace std;
using TUPLE_ALL_STR = tuple<string, string, string>;
namespace Kaco
{
    template <typename T>
    static void print(string message, T data)
    {
        cout << endl
             << "\"" << message << "\"" << endl;
        for (const auto &str : data)
            cout << str << endl;
    }

    static void print(string main_msg, string aux_msg1, string aux_msg2, vector<TUPLE_ALL_STR> data)
    {
        cout << endl
             << "\"" << main_msg << "\"" << endl;
        for (const auto &str : data)
            cout << "'[" << get<0>(str) << "]'" << endl
                 << "'# " << aux_msg1 << "'" << endl 
                 << get<1>(str) << endl
                 << "'# " << aux_msg2 << "'" << endl
                 << get<2>(str) << endl;
    }

    static void print(string main_msg, string aux_msg, map<string, string> data)
    {
        cout << endl
             << "\"" << main_msg << "\"" << endl;
        for (const auto &str : data)
            cout << "'# " << aux_msg << " [" << get<0>(str) << "]'" << endl
                 << get<1>(str) << endl;
    }

    static void print(string message, map<string, vector<pair<string, string>>> data)
    {
        cout << endl
             << "\"" << message << "\"" << endl;
        for (auto const &rec : data)
        {
            auto tbl_name = rec.first;
            auto vec_triggers = rec.second;
            for (auto const &trigger : vec_triggers)
            {
                auto trigger_name = trigger.first;
                auto trigger_sql = trigger.second;
                cout << endl << "'[" << tbl_name 
                     << "::" << trigger_name << "]'" << endl
                     << trigger_sql << endl;
            }
        }
    };

    static void print(string message, vector<pair<string, string>> data)
    {
        cout << endl
             << "\"" << message << "\"" << endl;
        for (auto const &vec : data)
        {
            auto trigger_name = vec.first;
            auto trigger_sql = vec.second;
            cout << endl
                 << "'[" << trigger_name << "]'" << endl
                 << trigger_sql << endl;
        }
    };

    static auto mprint = [](string message, map<string, string> data)
    {
        cout << endl
             << "\"" << message << "\"" << endl;
        for (const auto &str : data)
            cout << str.first << ": " << str.second << endl;
    };

    static auto updateRefTable = [](const vector<string> &cols, string schema)
    {
        vector<string> updated_cols = {};
        for (auto rec : cols)
        {
            size_t pos = rec.find(CT_REF);
            int ctref_size = ((string)CT_REF).length();
            while (pos != string::npos)
            {
                int index = pos + ctref_size + 1; // right on the beginning of the table name
                pos = rec.find_first_of(" ", index);
                if (pos != string::npos)
                {
                    int tblname_len = pos - index;
                    auto tblName = rec.substr(index, tblname_len);
                    stringstream ss;
                    ss << "\"" << schema << "\""
                       << "." << tblName;
                    auto newtbl_name = ss.str();
                    int newtblname_size = newtbl_name.size();
                    rec.replace(index, tblname_len, newtbl_name);
                    pos = rec.find(CT_REF, index + newtblname_size);
                }
                else
                    break;
            }
            updated_cols.push_back(std::move(rec));
        }
        return updated_cols;
    };

    static auto updateColNames = [](vector<string> cols, string schema, string tblName)
    {
        vector<string> updatedCols = {};
        stringstream ss;
        for (auto col : cols)
        {
            ss << "\"" << schema << "\""
               << "." << tblName << "." << col;
            updatedCols.push_back(std::move(ss.str()));
            ss.str("");
        }
        return updatedCols;
    };

    static auto updateColNameInConstraints = [](vector<string> constraints, vector<string> cols, string schema, string tblName)
    {
        vector<string> updated_constraints = {};
        for (auto constraint : constraints)
        {
            for (auto col : cols)
            {
                int index = 0;
                do
                {
                    size_t pos = constraint.find(col, index);
                    if (pos != string::npos)
                    {
                        int col_length = col.length();
                        auto next_pos = pos + col_length;
                        char ch = constraint[next_pos];
                        if ((ch == ' ') || (ch == ',') || (ch == ')'))
                        {
                            stringstream ss;
                            ss << "\"" << schema << "\""
                               << "." << tblName << "." << col;
                            auto ss_str = ss.str();
                            constraint.replace(pos, col_length, ss_str);
                            ss.str("");
                            index += (pos + ss_str.length());
                        }
                        else
                            index = next_pos;
                    }
                    else
                        break;
                } while (true);
            }
            updated_constraints.push_back(std::move(constraint));
        }
        return updated_constraints;
    };

    static auto getColNamesDetails = [](unordered_map<string, string> cols)
    {
        vector<string> col_name = {};
        vector<string> col_detail = {};
        for (const auto &str : cols)
        {
            col_name.push_back(str.first);
            col_detail.push_back(str.second);
        }
        return make_pair(col_name, col_detail);
    };

    static auto mergeColsAndConstraint = [](vector<string> cols, vector<string> constraints)
    {
        int col_size = cols.size();
        vector<string> column_constrain(col_size + constraints.size());
        column_constrain.assign(cols.begin(), cols.end());
        column_constrain.insert(column_constrain.begin() + col_size, constraints.begin(), constraints.end());
        return column_constrain;
    };

    static auto getColsAndConstraints = [](vector<string> colsCons)
    {
        unordered_map<string, string> cols = {};
        vector<string> constraints = {};
        for (auto str : colsCons)
        {
            int pos = str.find_first_of(" ");
            auto col = str.substr(0, pos);
            bool isTblConstraint = CHECK_IS_TBL_CONSTRAINT(col);
            if (!isTblConstraint)
                cols.insert({std::move(col), std::move(str)});
            else
                constraints.push_back(std::move(str));
        }
        return make_pair(cols, constraints);
    };

    static auto checkForMatch = [](char *stream)
    {
        stack<char> stack;
        for (auto i = 0; stream[i] != '\0'; i++)
        {
            if ((stream[i] == '(') || (stream[i] == '['))
                stack.push(stream[i]);
            else if ((stream[i] == ')') || (stream[i] == ']')) // I assume that [ and ( are followed by closings immediately
                stack.pop();
        }
        if (stack.empty())
            return true;
        return false;
    };

    // it assumes that there is no disparity between the number of '(' and ')'
    static auto handleWord = [](string word, stack<char> &stack)
    {
        bool stackIsEmpty = false;
        bool virgool = false;
        for (auto ch : word)
        {
            if (ch == '(')
                stack.push('(');
            else if (ch == ')')
                stack.pop();
            else if (ch == ',')
            {
                virgool = true;
                stackIsEmpty = stack.empty();
            }
        }
        return (stackIsEmpty && virgool);
    };

    static vector<string>
    splitCreateTblCmd(string cmd)
    {
        vector<string> cols = {};
        int firstPos = cmd.find_first_of("(");
        int lastPos = cmd.find_last_of(")");

        auto data = cmd.substr(firstPos + 1, lastPos - firstPos - 1);
        int spacePos = 0;
        stack<char> stack;
        stringstream ss;
        spacePos = data.find_first_of(" ");

#ifdef DEBUG
        cout << endl
             << "splitCreateTblCmd(): " << endl;
        cout << "input cmd is: " << endl
             << cmd << endl;
        cout << endl
             << "data is: " << endl
             << data << endl;
#endif

        while (spacePos != string::npos)
        {
            auto word = data.substr(0, spacePos);
            bool flush = handleWord(word, stack);
            if (flush)
            {
                ss << word.substr(0, word.length() - 1);
                cols.push_back(std::move(ss.str()));
                ss.str("");
            }
            else
                ss << word << " ";
            data = data.substr(spacePos + 1);
            spacePos = data.find_first_of(" ");
        }
        ss << data;
        cols.push_back(ss.str());
        return cols;
    }

    static pair<vector<string>, vector<string>> getColsConsDiff(vector<string> targetCols, vector<string> refCols)
    {
        vector<string> targetDiff = {};
        vector<string> refDiff = {};
        sort(targetCols.begin(), targetCols.end());
        sort(refCols.begin(), refCols.end());
        set_difference(targetCols.begin(), targetCols.end(), refCols.begin(), refCols.end(), back_inserter(targetDiff));
        set_difference(refCols.begin(), refCols.end(), targetCols.begin(), targetCols.end(), back_inserter(refDiff));
        return {targetDiff, refDiff};
    }

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

    static vector<string> getColsConsIntersect(vector<string> targetCols, vector<string> refCols)
    {
        vector<string> intersect = {};
        sort(targetCols.begin(), targetCols.end());
        sort(refCols.begin(), refCols.end());
        set_intersection(targetCols.begin(), targetCols.end(), refCols.begin(), refCols.end(),
                         std::inserter(intersect, intersect.begin()));
        return intersect;
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

    static map<string, string> initTablesSchema(const shared_ptr<IDbReader> &db, const vector<string> &tables)
    {
        map<string, string> result{};

        for (auto table : tables)
        {
            auto tableSchema = db->getTableSchema(table);
            stringstream ss;
            for (auto i = 0; i < tableSchema.size(); i++)
            {
                if (i)
                    ss << STR_SEPERATOR;
                ss << tableSchema[i];
            }
            result.insert({table, ss.str()});
        }

        return result;
    }

    static map<string, string> initTableIndices(const shared_ptr<IDbReader> &db, const vector<string> &tables)
    {
        map<string, string> result{};

        for (auto table : tables)
        {
            auto tableIndex = db->getIndices(table);
            stringstream ss;
            for (auto i = 0; i < tableIndex.size(); i++)
            {
                if (i)
                    ss << STR_SEPERATOR;
                ss << tableIndex[i];
            }
            result.insert({table, ss.str()});
        }

        return result;
    }

    // returns pair<tbl_name, trigger_name|trigger_sql_command>
    static map<string, string> initTableTriggers(const shared_ptr<IDbReader> &db, const vector<string> &tables)
    {
        map<string, string> result{};

        for (auto table : tables)
        {
            auto tableTriggers = db->getTriggers(table);
            stringstream ss;
            for (auto i = 0; i < tableTriggers.size(); i++)
            {
                if (i)
                    ss << STR_SEPERATOR;
                ss << tableTriggers[i].first << VAL_SEPERATOR << tableTriggers[i].second;
            }
            result.insert({table, ss.str()});
        }

        return result;
    }

    static map<string, vector<pair<string, string>>> initTblTriggers(const shared_ptr<IDbReader> &db, const vector<string> &tables)
    {
        map<string, vector<pair<string, string>>> tbl_triggers = {};
        for (auto tbl : tables)
        {
            auto triggers = db->getTriggers(tbl);
            auto pair_tbl_triggers = make_pair(tbl, triggers);
            tbl_triggers.insert(std::move(pair_tbl_triggers));
        }
        return tbl_triggers;
    }

} // namespace Kaco

#endif