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
#include"exp_type.h"
#include<iostream>
#include<coroutine>

s_ele_ptr e_push(s_ele_ptr e, s_i_ptr e_in)
{
	cout << string(*e) << "pushed to stack";
	e_in->push(e);
	return e;
}



class main_operation
{
public:
	main_operation();
	void run(string cmd_str);
	bool auto_calculate(auto_iterator<s_sc_ptrs>& cmdsi);
	void execute(auto_iterator<s_sc_ptrs>& cmdsi);
	void print_commands(auto_iterator<s_sc_ptrs> scsi);
	void translate_segments(s_seg_ptrs seg_v, auto_iterator<s_sc_ptrs> cmdsi);
	int query_command(s_sc_ptr sc, auto_iterator<s_sc_ptrs> cmds_iter);
	s_sc_ptrs compile_seg_trees(s_seg_ptrs& seg_v);
	s_seg_ptrs seg_current_dir(s_seg_ptr seg, s_seg_ptrs& seg_v);
	s_seg_ptrs seg_tree(s_seg_ptr seg, s_seg_ptrs& seg_v);

	::operators ops;
	exp_log_debug _log;
};


bool apprun()
{
	string cmd_str;
	main_operation mo;
	while (true)
	{
		cout << ">>";
		getline(cin, cmd_str);
		if(cmd_str == "exit") return false;
		mo.run(cmd_str);
	}
}


using namespace script_functions;
int main()
{
	while(apprun());
}

inline main_operation::main_operation()
{
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
	ops.set_delim_operator_type("\"", ::operators::delim_operator::delim_type::con_str);
	_log.object("main_process");
	_log.debug(debug_off);
}

inline void main_operation::run(string cmd_str)
{
	_log.behave("run");
	auto main_s = make_segment();
	auto main_e = make_ele("main", exp_ele::ele_type::invoke);
	main_s->owner = main_e;
	main_s->wrap_needed = false;//main_s is the main segment, so it doesn't need to be wrapped
	_Expression::dev(cmd_str);
	_Expression::convert_operators();
	auto code_v = _Expression::get_devs();
	try
	{
		ops.combine_operators(code_v);
		ops.match_operators(code_v);
		ops.delim_operators_preprocess(code_v);
		auto_iterator<s_ele_ptrs> ai(code_v);
		auto_iterator<s_ele_ptrs> ai_owners(ops.owner_list);
		main_s->import_source(ai, -1);
		s_seg_ptrs sv;
		auto se = main_s;
		auto sg_tr = seg_tree(se, sv);
		sg_tr.push_back(se);
		auto sc_v = compile_seg_trees(sg_tr);
		translate_segments(sg_tr, sc_v);
		print_commands(sc_v);
		auto cmdsi = auto_iterator<s_sc_ptrs>(sc_v);
		execute(cmdsi);
		_log.last_behavior();
	}
	catch (std::exception& e)
	{
		cout << e.what() << endl;
	}
}

inline bool main_operation::auto_calculate(auto_iterator<s_sc_ptrs>& cmdsi)
{
	_log.behave("auto_calculate");
	auto_iterator<s_sc_ptrs> asi(cmdsi);
	move_if_func(cal_process, asi) {//define auto_iterator_move_if_func cal_process
		if (ele->e1 && ele->e1->e_type == exp_ele::ele_type::taken) ele->e1->cast_to<exp_taken>()->fetch_value();
		if (ele->e2 && ele->e2->e_type == exp_ele::ele_type::taken) ele->e2->cast_to<exp_taken>()->fetch_value();
		if (!ele->is_standard())
		{
			p->register_state(FALSE_AUTO_ITERATOR, true);
			return false;
		}
		ele->exp_calculate();
		_log.logout("status: ", "now in command: @", false);
		_log.logout_any(asi.index());
		_log.logout("value=>", ele->value->e_str);
		return true;
	};
	_log.logout("iterating commands");
	asi.move_if(asi.size(), cal_process);
	if (asi.pop_state(FALSE_AUTO_ITERATOR))
	{
		_log.logout("status: not standard command.", "iterator now stopped at: @", false);
		_log.logout_any(asi.index());
		cmdsi.set_at(asi.index());
		_log.last_behavior();
		return false;
	}
	_log.last_behavior();
	return true;
}

void main_operation::execute(auto_iterator<s_sc_ptrs>& cmdsi)
{
	_log.behave("execute");
	auto_iterator<s_sc_ptrs> asi(cmdsi);
	auto e_in = make_invoke(); // stack environment
	while (!auto_calculate(asi))
	{
		// _log.logout("value=>", asi.data()->value->e_str);
		e_in->execute_command(asi.data());
		asi.move(1);
		if(!asi.safe_verify_operate()) break;
	}
	_log.last_behavior();
}

inline void main_operation::print_commands(auto_iterator<s_sc_ptrs> scsi)
{
	scsi.move_with(scsi.size(), move_with_lambda(scsi)
	{
		exp_command_pointer cp = *p;
		cout << cp << endl;
	});
}

inline void main_operation::translate_segments(s_seg_ptrs seg_v, auto_iterator<s_sc_ptrs> cmdsi)
{
	string prefix = "@";
	for (auto& i : seg_v)
	{
		auto cmd = i->share_commands().back();
		auto index = query_command(cmd, cmdsi);
		i->e_str = prefix + to_string(index);
		cmdsi.to_begin();
	}
}

inline int main_operation::query_command(s_sc_ptr sc, auto_iterator<s_sc_ptrs> cmds_iter)
{
	cmds_iter.find_data(sc);
	if (cmds_iter.get_state(FALSE_AUTO_ITERATOR)) return -1;
	return cmds_iter.index();
}

inline s_sc_ptrs main_operation::compile_seg_trees(s_seg_ptrs& seg_v)
{
	s_sc_ptrs sc_v;
	auto_iterator<s_seg_ptrs> asi(seg_v);
	asi.move_with(asi.size(), move_with_lambda(asi)
	{
		ele->compile_source();
		auto s_i = make_invoke(ele);
		s_i->wrap_segment();
		auto cmds = ele->share_commands();
		sc_v.insert(sc_v.end(), cmds.begin(), cmds.end());
	});
	return sc_v;
}

inline s_seg_ptrs main_operation::seg_current_dir(s_seg_ptr seg, s_seg_ptrs& seg_v)
{
	auto i = seg->sub_segment();
	if (i)
	{
		seg_v.push_back(i);
		seg_current_dir(seg, seg_v);
	}
	return seg_v;
}

inline s_seg_ptrs main_operation::seg_tree(s_seg_ptr seg, s_seg_ptrs& seg_v)
{
	s_seg_ptrs tmp_v;
	tmp_v = seg_current_dir(seg, tmp_v);

	if (tmp_v.empty()) return seg_v;
	for (auto i : tmp_v)
	{
		seg_tree(i, tmp_v);
	}
	seg_v.insert(seg_v.end(), tmp_v.begin(), tmp_v.end());
	return seg_v;
}
