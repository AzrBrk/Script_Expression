#include "exp_ele.h"

exp_ele::exp_ele()
	:
	e_str(""), e_type(ele_type::undefined)
{
}

exp_ele::exp_ele(string str)
	:
	e_str(str), e_type(ele_type::undefined)
{}

exp_ele::exp_ele(string str, ele_type t)
	:e_str(str), e_type(t)
{}

bool exp_ele::operator==(string str)
{
	return e_str == str;
}

bool exp_ele::operator==(ele_type t)
{
	return e_type == t;
}

exp_ele::operator std::string()
{
	return "['" + e_str + "']";
}

std::shared_ptr<exp_ele> exp_ele::cast_to_ele()
{
	return std::static_pointer_cast<exp_ele>(shared_from_this());
}
