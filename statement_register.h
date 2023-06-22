#pragma once
#include"include_file.h"

//chat gpt ����
using std::map;
class statement_register
{
public:
    void register_state(int state_name, bool value)
    {
        states[state_name] = value;
    }

    bool pop_state(int state_name)
    {
        auto iter = states.find(state_name);
        if (iter != states.end())
        {
            bool result = iter->second;
            states.erase(iter);
            return result;
        }
        else
            return false; // ����Ҳ�����Ӧ state������ false
    }

    bool get_state(int state_name)
    {
        auto iter = states.find(state_name);
        if (iter != states.end())
            return iter->second;
        else
            return false; // ����Ҳ�����Ӧ state������ false
    }

    bool has_state(int state_name)
    {
        return (states.count(state_name) > 0);
    }

    void modify_state(int state_name, bool new_value)
    {
        auto iter = states.find(state_name);
        if (iter != states.end())
            iter->second = new_value;
        else
            states[state_name] = new_value;
    }

private:
    std::map<int, bool> states;
};

