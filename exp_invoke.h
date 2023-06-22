#pragma once
#include"include_file.h"
#include "exp_ele.h"
#include "exp_segment.h"
#include "exp_log_debug.h"
#include "exp_cpp_func.h"

//a exp_invoke means it can be called when it correctly matched to a compiled segment
//if a operator has attribute of 'e_invoke', the operator will be linked to a e_invoke element, usually the owner of the segment it belongs to
//the operator's effect will automatically apply to the exp_invoke, too
class exp_invoke :
    public exp_ele
{
public:
    exp_invoke(s_ele_ptr attach_ele);
    //construct a exp_invoke with a auto_iterator<s_ele_ptrs> pointing to the owner of the segment
    exp_invoke(auto_iterator<s_ele_ptrs> vit);
    //
    exp_command_pointer get_commands_ptr();
    //push an element to the stack
    void push(s_ele_ptr ptr);
    //wrap the segment with commands to make it callable
    void wrap_segment();
    //pop an element from the stack
    s_ele_ptr pop();
    //get the top element of the stack
    s_ele_ptr top();
    //get the size of the stack
    int size();

private:
    exp_log_debug _log;
    std::stack<s_ele_ptr> _stack;
    s_ele_ptr invoke_ele;
    s_seg_ptr link_to_segment;
};

typedef std::shared_ptr<exp_invoke> s_i_ptr;