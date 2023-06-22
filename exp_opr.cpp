#include "exp_opr.h"

exp_opr::exp_opr()
	:
	exp_ele("", exp_ele::ele_type::opr)
{}

exp_opr::exp_opr(exp_ele e)
	:
	exp_ele(e)
{}

exp_opr::exp_opr(string str)
	:
	exp_ele(str, exp_ele::ele_type::opr)
{}

bool exp_opr::operator==(exp_opr e)
{
	return e_str == e.e_str;
}

exp_opr::opr_order exp_opr::get_order()
{
	return opr_t;
}
