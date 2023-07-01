#include "script_command.h"

bool script_command::is_standard()
{
	if (!(*this == "+" || *this == "-" || *this == "*" || *this == "/")) return false;
	if (!(*e1 == exp_ele::ele_type::num && *e2 == exp_ele::ele_type::num)) return false;
	return true;
}

void script_command::set_command(s_o_ptr cmd, s_ele_ptr _1, s_ele_ptr _2, s_ele_ptr _3)
{
	command = cmd;
	e1 = _1;
	e2 = _2;
	e3 = _3;
}

void script_command::exp_calculate()
{
	if (!is_standard()) return;
	try
	{
		auto c_1 = exp_type<int>::make_type(e1)->strong_cast();
		auto c_2 = exp_type<int>::make_type(e2)->strong_cast();
		if (*this == "+") value = exp_type<int>::make_type_from(c_1 + c_2);
		else if (*this == "-") value = exp_type<int>::make_type_from(c_1 - c_2);
		else if (*this == "*") value = exp_type<int>::make_type_from(c_1 * c_2);
		else if (*this == "/") value = exp_type<int>::make_type_from(c_1 / c_2);
		else throw std::exception("script_command::exp_calculate() error");
		value->e_type = exp_ele::ele_type::num;
	}
	catch (const std::exception& e)
	{
		throw e;
	}
}


bool script_command::operator==(std::string cmp_opr)
{
	return command->e_str == cmp_opr;
}

s_sc_ptr make_command(s_o_ptr cmd, s_ele_ptr _1, s_ele_ptr _2, s_ele_ptr _3)
{
	auto res_cmd = std::make_shared<script_command>();
	res_cmd->set_command(cmd, _1, _2, _3);
	return res_cmd;
}
