#include "script_command.h"

bool script_command::is_standard()
{
	return false;
}

void script_command::set_command(s_o_ptr cmd, s_ele_ptr _1, s_ele_ptr _2, s_ele_ptr _3)
{
	command = cmd;
	e1 = _1;
	e2 = _2;
	e3 = _3;
}

s_sc_ptr make_command(s_o_ptr cmd, s_ele_ptr _1, s_ele_ptr _2, s_ele_ptr _3)
{
	auto res_cmd = std::make_shared<script_command>();
	res_cmd->set_command(cmd, _1, _2, _3);
	return res_cmd;
}
