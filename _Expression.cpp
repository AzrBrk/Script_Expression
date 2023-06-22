#include "_Expression.h"
_Expression::s_ele_ptrs _Expression::devs{};
bool _Expression::is_not_number(char c)
{
    return !(c >= '0' && c <= '9');
}

inline bool _Expression::is_alpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline bool _Expression::is_down_slice(char c)
{
	return c == '_';
}

inline bool _Expression::is_number(char c)
{
	return c >= '0' && c <= '9';
}

inline bool _Expression::is_blank_space(char c)
{
	return c == ' ';
}

inline bool _Expression::is_not_number_or_alpha(char c)
{
	return !(is_alpha(c) || is_number(c) || is_down_slice(c) || is_blank_space(c));
}

bool _Expression::is_owned(auto_iterator<s_ele_ptrs> v)
{
	v--;
	return is_(v.data(), exp_ele::ele_type::var);
}

void _Expression::dev(std::string str)
{
	if (str.length() <= 0)
	{
		convert_operators();
		return;
	}
		
	if (is_available_var_name()(str.at(0)))
		devs.push_back(make_shared<exp_ele>(take_when(str, is_available_var_name()), exp_ele::ele_type::var));
	else if (is_number(str.at(0)))
		devs.push_back(make_shared<exp_ele>(take_when(str, is_number), exp_ele::ele_type::num));
	else if (is_not_number_or_alpha(str.at(0)))
		devs.push_back(make_shared<exp_ele>(take_when_times(str, is_not_number_or_alpha, 1), exp_ele::ele_type::opr));
	else if (is_blank_space(str.at(0)))
		devs.push_back(make_shared<exp_ele>(take_when_times(str, is_blank_space, 1), exp_ele::ele_type::var));
	dev(str);
}

bool _Expression::is_(s_ele_ptr& e, exp_ele::ele_type t)
{
	return e->e_type == t;
}

void _Expression::convert_operators()
{
	auto_iterator<s_ele_ptrs> eai{devs};
	eai.move_with(devs.size(), move_if_lambda(eai)
	{
		if (is_(ele, exp_ele::ele_type::opr))
			ele_type_convert<exp_opr>(*p);
	});
}


_Expression::s_ele_ptrs _Expression::get_devs()
{
	auto tmp{ devs };
	devs.clear();
	return tmp;
}

inline bool _Expression::is_available_var_name::operator()(char c)
{
	if (b_first_char)
	{
		b_first_char = false;
		return (is_alpha(c) || is_down_slice(c));
	}
	else
	{
		return is_number(c) || is_alpha(c) || is_down_slice(c);
	}
}
