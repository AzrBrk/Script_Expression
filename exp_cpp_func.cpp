#include "exp_cpp_func.h"

exp_cpp_func::exp_cpp_func()
{
    e_type = ele_type::cpp_func;
    _log.object("exp_cpp_func");
    _log.behave("exp_cpp_func::exp_cpp_func");
}

exp_cpp_func::exp_cpp_func(s_ele_ptr matched_e)
    :
    exp_cpp_func()
{
	matched = matched_e;
    //matched_e->matched = shared_from_this();
}

void exp_cpp_func::reflexed_by(string name)
{
    e_str = name;
}

void exp_cpp_func::reflex_redirect(s_ele_ptr _from, s_ele_ptr _to)
{
    _from->matched = shared_from_this();
    matched = _to;
}

void exp_cpp_func::exp_bind(exp_func_type _func)
{
    func = _func;
}

void exp_cpp_func::exp_bind_ele(s_ele_ptr e_func)
{
    ele_func= e_func;
}

s_cpp_func_ptr make_cpp_func(s_ele_ptr e)
{
    _return_if(e == nullptr, std::make_shared<exp_cpp_func>());
    auto res =  std::make_shared<exp_cpp_func>(e);
    return res;
}
