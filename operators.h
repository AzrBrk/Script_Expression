#pragma once
#include"exp_opr.h"
#include"auto_iterator.h"
#include"statement_register.h"
#include"exp_log_debug.h"
class operators
{
public:
	typedef std::vector<exp_opr> v_oprs;
	v_oprs oprts;
	exp_log_debug _log;
	struct delim_operator
	{
		enum class delim_type
		{
			con_str,
			func_arg,
			arr_idx,
			seg_body,
			weight_on
		};
		delim_operator(exp_opr& a, exp_opr& b, delim_type d = delim_type::seg_body);

		delim_type d_type;
		exp_opr begin;
		exp_opr end;
	};
	std::vector<delim_operator> delim_oprts;
	//从输入的操作符列表中构建分隔符表
	bool mark_owner(auto_iterator<s_ele_ptrs> target_v);
	//store index of owner in owner_list
	s_ele_ptrs owner_list;
	void combine_operators(s_ele_ptrs& v);
	void initialize_delim_operators_list(v_oprs::iterator iter);
	void add_operator(std::string e_str, exp_opr::opr_order order, unsigned int level);
	void auto_add_operator(std::string e_str, exp_opr::opr_order order, bool base_inc = true);
	bool match_operator(exp_opr& oop);
	void match_operators(s_ele_ptrs& v);
	void delim_operators_preprocess(s_ele_ptrs& v);
	void string_delim_operators_preprocess(s_ele_ptrs& v);
	void weight_on(s_ele_ptrs& v);
	void match_delim_operator(delim_operator _do, auto_iterator<s_ele_ptrs> v);
	void set_delim_operator_type(std::string e_str, delim_operator::delim_type dt);

	operators() { _log.object(typeid(*this).name()); _log.debug(debug_on); };
private:
	string get_ai_index_str(auto_iterator<s_ele_ptrs>& ai) { return "[" + std::to_string(ai.index()) + "]"; };
	unsigned int opr_inc_base{ 0 };
};

