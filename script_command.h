#pragma once
#include"exp_opr.h"
#include"exp_type.h"
//a standard command unit
class script_command
{
public:
	s_o_ptr     command;//operator
	s_ele_ptr        e1;
	s_ele_ptr        e2;
	s_ele_ptr        e3;//maximum 3 elements supported for a command unit
	s_ele_ptr     value{nullptr};//the value of this command unit, use prefix '@' plus address to access the value
	s_ele_ptr     exec_matched{nullptr};//the element matched to this command unit
	virtual bool is_standard();//quick calculating access,if a command is standard, it will not rely on any other script class
	void set_command(s_o_ptr cmd, s_ele_ptr _1, s_ele_ptr _2 = nullptr, s_ele_ptr _3 = nullptr);
	void exp_calculate();//quick calculating access,if a command is standard, it will not rely on any other script class
	bool operator ==(std::string cmp_opr);
};
typedef std::shared_ptr<script_command> s_sc_ptr;
typedef std::vector<s_sc_ptr> s_sc_ptrs;
s_sc_ptr make_command(s_o_ptr cmd, s_ele_ptr _1, s_ele_ptr _2 = nullptr, s_ele_ptr _3 = nullptr);

