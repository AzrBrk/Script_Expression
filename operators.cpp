#include "operators.h"

operators::delim_operator::delim_operator(exp_opr& a, exp_opr& b, delim_type d)
	:
	begin(a),
	end(b),
	d_type(d)
{}

bool operators::mark_owner(auto_iterator<s_ele_ptrs> target_v)
{
	_return_if(!target_v.size(), false);
	_return_if(target_v.at_begin(), false);
	
	auto e_ptr = target_v.at(target_v.index() - 1).data();
	if (*e_ptr == exp_ele::ele_type::var && *e_ptr != " ") 
	{
		target_v.data()->owner = e_ptr;
		e_ptr->matched = target_v.data();
		e_ptr->e_type = exp_ele::ele_type::invoke;
		owner_list.push_back(target_v.data()->owner);
		return true;
	}
	return false;
}

void operators::combine_operators(s_ele_ptrs& v)
{
	for (auto i = v.begin(); i != v.end() - 1; ++i)
	{
		if (i == v.end()) break;
		if ((**i).e_type != exp_ele::ele_type::opr)
			continue;
		auto i_str = (**i).e_str + (**(i + 1)).e_str;
		if (std::find(oprts.begin(), oprts.end(), i_str) != oprts.end())
		{
			(**i).e_str = i_str;
			v.erase(i + 1);
			
		}
	}
}

void operators::initialize_delim_operators_list(v_oprs::iterator iter)
{
	decltype(iter) first_iter = oprts.end(), second_iter = oprts.end();

	for (; iter != oprts.end(); ++iter)
	{
		if (iter->opr_t == exp_opr::opr_order::e__)//定义段左操作符

		{
			first_iter = iter;
			break;
		}
	}
	for (; iter != oprts.end(); ++iter)
	{
		if (iter->opr_t == exp_opr::opr_order::__e)//定义左操作符后面的另一个段操作符即被定义为段的终止符
		{
			second_iter = iter;
			break;
		}
	}

	if (first_iter == oprts.end())
		return;
	if (second_iter == oprts.end())
		throw std::exception("error：lack of an end for a segment operator");
	delim_oprts.push_back(delim_operator(*first_iter, *second_iter));
	return initialize_delim_operators_list(iter);
}

void operators::add_operator(std::string e_str, exp_opr::opr_order order, unsigned int level)
{
	exp_opr e(e_str);
	e.opr_t = order;
	e.level = level;

	oprts.push_back(e);
}

void operators::auto_add_operator(std::string e_str, exp_opr::opr_order order, bool base_inc)
{
	add_operator(e_str, order, base_inc ? ++opr_inc_base : opr_inc_base);
}

bool operators::match_operator(exp_opr& oop)
{
	auto i = std::find(oprts.begin(), oprts.end(), oop);
	if (i == oprts.end()) return false;
	oop.opr_t = i->opr_t;
	oop.level = i->level;
	return true;
}

void operators::match_operators(s_ele_ptrs& v)
{
	auto_iterator<s_ele_ptrs> eai(v);
	eai.move_with(v.size(), move_with_lambda(eai)
	{
		bool b_find = p->find([](s_ele_ptr ep)
			{ return *ep == exp_ele::ele_type::opr; });
		if (b_find)
		{
			auto op = p->data()->cast_to<exp_opr>();
			match_operator(*op);
		}
	});
}

void operators::delim_operators_preprocess(s_ele_ptrs& v)
{
	//sort delim_oprts to make sure the highest level delimiters are firstly processed 
	std::sort(delim_oprts.begin(), delim_oprts.end(), [=](operators::delim_operator& a, operators::delim_operator& b)
		{
			return a.begin.level > b.begin.level;
		}
	);
	_log.behave(__FUNCTION__);
	_log.logout("starting delim operators preprocessing");
	auto_iterator<s_ele_ptrs> v_iter(v);
	for (auto i : delim_oprts)
	{
		match_delim_operator(i, v_iter);
	}
	v_iter.find([](s_ele_ptr e)
		{
			if (!(e->e_type == exp_ele::ele_type::opr)) return false;
			auto op = e->cast_to<exp_opr>();
			if (op && (op->opr_t == exp_opr::opr_order::__e) && (!(e->matched)))
				return true;
			return false;
		});
	if(!v_iter.has_state(FALSE_AUTO_ITERATOR)) {
		_log.logout("Error:", "lack of begin operator for a end delim operator");
		throw _log;
	}
	
	weight_on(v);
	_log.logout("sorting owners list...");
	_log.last_behavior();
}

void operators::string_delim_operators_preprocess(s_ele_ptrs& v)
{
	_log.behave(__FUNCTION__);
	_log.logout("starting string delim operators preprocessing");
	auto_iterator<s_ele_ptrs> v_iter(v);
	while (v_iter.find([&](s_ele_ptr e)
		{
			if(!(*e == exp_ele::ele_type::opr))
				return false;
			
			return false;
		}));
	_log.last_behavior();
}

//if a segment is not owned by any element
//the segment is regarded as a weight on segment
//all operators inside the segment will be upgrade to have a higher priority to be execute
void operators::weight_on(s_ele_ptrs& v)
{
	_log.behave(__FUNCTION__);
	_log.logout("running weight on operators...");
	auto_iterator<s_ele_ptrs> ai(v);
	ai.find([=](s_ele_ptr e)
		{
			//check if e is opr
			if (!(*e == exp_ele::ele_type::opr)) return false;
			auto o_ptr = e->cast_to<exp_opr>();
			if (!o_ptr)
			{
				_log.logout("invalid pointer!");
				throw _log;
			}
			//check if e is delim_opr
			if (!(o_ptr->opr_t == exp_opr::opr_order::e__)) return false;
			//check if e owned by an element
			if (e->owner) return false;
			return true;
		});
	if (ai.pop_state(FALSE_AUTO_ITERATOR)) 
	{
		_log.logout("there is no further more delim operators"); 
		_log.last_behavior(); 
		return;
	}
	auto begin_matched = ai.data()->matched;
	//after finishing its job weight on operators should be deleted
	_log.logout("weight on () at [", std::to_string(ai.index())+"]");
	//ai.erase();
	ai.get_last_move_offset();
	while (ai.find([=](s_ele_ptr e) {
		//_log.logout("comparing:", *e);
		return (*e == exp_ele::ele_type::opr);
		}))
	{
		
		//_log.logout("comparing ", std::to_string((unsigned int)(current_matched.get())) + " with " + std::to_string((unsigned int)(begin_matched.get())));
		if (ai.data() == begin_matched)
		{
			_log.logout("found matched! at [", std::to_string(ai.index()) + "]");
			break;
		}
		_log.logout(*(ai.data()), "at [" + std::to_string(ai.index()) + "] level upgrade:" + std::to_string(ai.data()->cast_to<exp_opr>()->level));
		ai.data()->cast_to<exp_opr>()->level += begin_matched->cast_to<exp_opr>()->level;
		_log.logout("=>", std::to_string(ai.data()->cast_to<exp_opr>()->level));
		ai++;
	}
	_log.logout("beta feature:", "auto_iterator is trying to go back to the begin pos...");
	ai.go_back();
	_log.logout("auto_iterator now synced at", get_ai_index_str(ai) + ":" + (ai.data()->operator std::string()));
	if (ai.data()->owner)
	{
		_log.logout("This segment is owned by ", *(ai.data()->owner));
		ai++;
	}
	else
	{
		ai.erase();
		_log.logout("operator erased, searching for matched end operator");
		ai.to_begin();
		ai.find_data(begin_matched);
		ai.erase();
		_log.logout("current delim operator deleted at", std::to_string(ai.index()));
	}
	
	weight_on(ai.vec());
}

//if a begin operator has not been found, do not match the end operator
void operators::match_delim_operator(delim_operator _do, auto_iterator<s_ele_ptrs> v)
{
	_log.behave(__FUNCTION__);
	s_ele_ptr begin_ptr{ nullptr };
	string begin_pos = "";
	_log.logout("matching delim operator: ", _do.begin.e_str + _do.end.e_str + " at [" + std::to_string(v.index()) + "]");
	_log.logout("checking for sub delim...");
	v++;
	if (v.find([=](s_ele_ptr e)
		{
			//_log.logout("comparing:", *e);
			return *e == _do.begin.e_str && !e->matched;
		}))
	{
		_log.logout("positive!");
		if (mark_owner(v))
		{
			_log.logout("owned by:", *(v.data()->owner));
		}
		match_delim_operator(_do, v);
	}
	else 
	{
		_log.logout("negative!"); 
		return;
	}
	begin_ptr = v.data();
	begin_pos = "[" + std::to_string(v.index()) + "]";
	_log.logout("looking for end operator...");
	v++;
	if (v.find([=](s_ele_ptr e)
		{
			return *e == _do.end.e_str && !e->matched;
		}))
	{
		_log.logout(begin_pos + " matched at [", std::to_string(v.index()) + "]");
		v.data()->matched = begin_ptr;
		begin_ptr->matched = v.data();
	}
	else
	{
		_log.logout("negative!");
		_log.logout("Error:", "lack of an end operator for a begin delim operator");
		throw _log;
	}
	_log.last_behavior();
}

void operators::set_delim_operator_type(std::string e_str, delim_operator::delim_type dt)
{
	auto iter = std::find_if(delim_oprts.begin(), delim_oprts.end(), [=](delim_operator d)
		{
			return d.begin.e_str == e_str;
		});

	if (iter == delim_oprts.end())
	{
		throw std::exception("error: there is no such segment operator");
	}

	iter->d_type = dt;
}
