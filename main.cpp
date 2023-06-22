#include"auto_iterator.h"
#include"_Expression.h"
#include"operators.h"
#include"if_container.h"
#include"asio_tcp_server.h"
#include"html_doc.h"
#include"exp_taken.h"
#include"exp_segment.h"
#include"exp_invoke.h"
#include"exp_cpp_func.h"
#include<iostream>


int main()
{
	::operators ops;
	ops.auto_add_operator("=", exp_opr::opr_order::e_op_e);
	ops.auto_add_operator(",", exp_opr::opr_order::e_invoke);
	ops.auto_add_operator("+", exp_opr::opr_order::e_op_e);
	ops.auto_add_operator("-", exp_opr::opr_order::e_op_e, 0);
	ops.auto_add_operator("++", exp_opr::opr_order::e_op);
	ops.auto_add_operator("*", exp_opr::opr_order::e_op_e);
	ops.auto_add_operator("/", exp_opr::opr_order::e_op_e, 0);
	ops.auto_add_operator("(", exp_opr::opr_order::e__);
	ops.auto_add_operator(")", exp_opr::opr_order::__e, 0);
	ops.auto_add_operator("\"", exp_opr::opr_order::e__);
	ops.auto_add_operator("\"", exp_opr::opr_order::__e, 0);
	ops.initialize_delim_operators_list(ops.oprts.begin());
	ops.set_delim_operator_type("\"", ::operators::delim_operator::delim_type::con_str);//ÉèÖÃË«ÒýºÅÎª×Ö·û´®¶Î
	auto esg = make_segment();

	string cmd_str = "print(3*(90+4),5*(1+2),(20+14))";//
	_Expression::dev(cmd_str);
	_Expression::convert_operators();
	auto code_v = _Expression::get_devs();
	ops.combine_operators(code_v);
	ops.match_operators(code_v);
	ops.delim_operators_preprocess(code_v);
	auto cpp_func = std::make_shared<exp_cpp_func>();;
	//cpp_func->bind(e_plus);
	try
	{
		auto_iterator<s_ele_ptrs> ai(code_v);
		auto_iterator<s_ele_ptrs> ai_owners(ops.owner_list);
		while (ai.find_data(ai_owners.data()) && !ai_owners.at_end())
		{
			auto e_in = std::make_shared<exp_invoke>(ai);
			e_in->wrap_segment();
			auto c_ptr = e_in->get_commands_ptr();
			while (!c_ptr.auto_iterator<s_sc_ptrs>::get_state(FALSE_AUTO_ITERATOR))
			{
				cout << c_ptr << endl;
				c_ptr += 1;
			}
			ai_owners++;
		}
	}
	catch (std::exception & e)
	{
		cout << e.what() << endl;
	}
}