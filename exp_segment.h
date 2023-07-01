#pragma once
#include"exp_taken.h"
#include"auto_iterator.h"
#include"operators.h"
#include"exp_log_debug.h"


class exp_segment :
	public exp_ele
{
public:
	exp_segment();
	exp_segment(s_ele_ptr e);
	friend class exp_invoke;
	int compile_source();
	s_sc_ptrs share_commands();
	std::shared_ptr<exp_segment> sub_segment();
	std::shared_ptr<exp_segment> sub_segment(auto_iterator<s_ele_ptrs>& v);
	std::string taken_str(s_ele_ptr ptr);
	auto_iterator<s_ele_ptrs> highest_level_operator();
	void import_source(auto_iterator<s_ele_ptrs> vit, int offset = -1);
	::operators::delim_operator::delim_type dt = ::operators::delim_operator::delim_type::seg_body;
	unsigned int																    length = 0;
	s_ele_ptrs																   link_to_segment;
	s_sc_ptr																   link_to_command;
	bool                                                                    wrap_needed = true;

private:
	s_ele_ptrs sources;
	s_sc_ptrs commands;
	exp_log_debug _log;
	s_o_ptr push;
	s_o_ptr pop;
};

typedef std::shared_ptr<exp_segment> s_seg_ptr;
typedef std::vector<std::shared_ptr<exp_segment>> s_seg_ptrs;
s_seg_ptr make_segment();

