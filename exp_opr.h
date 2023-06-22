#pragma once
#include "exp_ele.h"
//define the operator element
class exp_opr :
    public exp_ele
{
public:
	//define the order type of operator, e stands for an operand, op stands for an operator
	//_ means the operator will take only an operand on a specified side, e__ operator means the operator will take all operands until it meets an __e operator 
	//e_invoke operator means the operator will take an exp_invoke object as its operand
	enum class opr_order
	{
		e_op_e,
		e_op,
		op_e,
		e__,
		__e,
		e_invoke
	};
	exp_opr();
	exp_opr(exp_ele e);
	exp_opr(string str);
	bool operator ==(exp_opr e);//compare with a primitive operator in a operator table
	unsigned int level = 0;
	opr_order opr_t;
	opr_order get_order();
};
typedef std::shared_ptr<exp_opr> s_o_ptr;
