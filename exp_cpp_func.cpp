#include "exp_cpp_func.h"

exp_cpp_func::exp_cpp_func()
{
    e_type = ele_type::cpp_func;
}

s_ele_ptr exp_cpp_func::operator()(s_ele_ptr _1, s_ele_ptr _2, s_ele_ptr _3)
{
    return func(_1, _2, _3);
}

s_ele_ptr exp_cpp_func::operator()(s_sc_ptr cmd)
{
    return func(cmd->e1, cmd->e2, cmd->e3);
}

void exp_cpp_func::bind(exp_func_type _func)
{
    func = _func;
}

s_cpp_func_ptr make_cpp_func()
{
    return std::make_shared<exp_cpp_func>();
}
