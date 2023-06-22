#include "exp_log_debug.h"

void exp_log_debug::logout(string err, string add)
{
    err_info = err;
    _additional = add;
    make_all_info();
    if(debug_status) cout << all_info << endl;
}

void exp_log_debug::debug(bool b_d)
{
    debug_status = b_d;
    //logout(b_d ? "debug on" : "debug off");
}

void exp_log_debug::object(string obj, string beh)
{
    if (size_t pos = obj.find("class ") != string::npos)
    {
        pos = obj.find(" ") + 1;
        obj = obj.substr(pos);
    }
    _object = obj;
    _behavior = beh;
}

void exp_log_debug::behave(string beh)
{
    _last_behavior = _behavior;
    _behavior = beh;
}

void exp_log_debug::last_behavior()
{
    _behavior = _last_behavior;
}

string exp_log_debug::time_text()
{
    ostringstream res{};
    res << '[';
    time_t t_now = time(0);
    tm tlm;
    localtime_s(&tlm, &t_now);
    res << 1900 + tlm.tm_year << "/" << 1+ tlm.tm_mon << "/" << tlm.tm_mday << " ";
    res << tlm.tm_hour << ":" << tlm.tm_min << ":" << tlm.tm_sec;
    res << ']';
    return res.str();
}

void exp_log_debug::make_all_info()
{
    ostringstream oss;
    if (_behavior.find(_object + "::") != std::string::npos)  oss << time_text() << "[" << _behavior << "]:" << err_info<< _additional;
    else oss << time_text() << ":[" << _object << "]::[" << _behavior << "]:" << err_info << _additional;
    all_info = oss.str();
}
