#pragma once
#include"include_file.h"
#include "exp_ele.h"
#include "exp_taken.h"
//element linked to a cpp function
//use exp_ele::matched to bind to exp_cpp_func object
//provide source language with an interface to C++ function, use exp_invoke to import arguments
class exp_cpp_func :
    public exp_ele
{
public:
    exp_cpp_func();
    using exp_func_type = std::function<s_ele_ptr(s_ele_ptr, s_ele_ptr, s_ele_ptr)>;
    s_ele_ptr operator()(s_ele_ptr _1, s_ele_ptr _2 = nullptr, s_ele_ptr _3 = nullptr);
    s_ele_ptr operator()(s_sc_ptr cmd);
    void bind(exp_func_type _func);
private:
    exp_func_type func;
};

typedef std::shared_ptr<exp_cpp_func> s_cpp_func_ptr;
s_cpp_func_ptr make_cpp_func();