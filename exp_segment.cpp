#include "exp_segment.h"
#include"include_file.h"

exp_segment::exp_segment()
	:
exp_ele()
{
	e_type = exp_ele::ele_type::segment;
	_log.object("exp_segment");
	_log.debug(debug_on);
	_log.behave(__FUNCTION__);
	_log.logout("LOG initialization completed!", "LOG running");
	push = std::make_shared<exp_opr>("PUSH");
	push->opr_t = exp_opr::opr_order::e_invoke;
}

int exp_segment::compile_source()
{
	_log.behave(__FUNCTION__);
	_log.debug(debug_on);// if debug_off, until debug_on, logout will be turned off
	_log.logout("Compiling segment source:", "start!");
	_log.logout("Current sources view:", ele_ptrs_to_string(sources, commands));
	while (sources.size() > 1)
	{
		auto h_est_op = highest_level_operator();
		_log.logout("Highest level operator:", *(h_est_op.data()));
		auto command_taker_ccc = make_taken();
		auto cmd_iter = command_taker_ccc->take_opr(h_est_op);

		if (cmd_iter.get_state(FALSE_AUTO_ITERATOR) && sources.size() >= 2)
		{
			cmd_iter.remove_all_at([&](s_ele_ptr e)
				{
					return e->e_str == " ";
				}, cmd_iter.vec().size());
			if(cmd_iter.vec().size()>1)
			{
				_log.logout("Error occurred while extracting operands!!!", "auto_iterator has a states of 'FALSE_AUTO_ITERATOR', this could be occurred by an undefined operator");
				_log.logout("Multiple tokens found in final result,", "can not resolve to one certain commands tree!");
				return -1;
			}
		}
		_log.logout("Taken elements now deleted!");
		if (!cmd_iter.vec().empty()) _log.logout("The auto_iterator now synced at sources.at(" + std::to_string(cmd_iter.index()),")");
		auto ele_ptr = make_ele();
		if (command_taker_ccc->command && command_taker_ccc->command->opr_t == exp_opr::opr_order::e_invoke)
		{
			_log.logout("Push to "+owner->e_str+" with ", _Expression::is_(command_taker_ccc->e1,
				ele_type::taken) ? taken_str(command_taker_ccc->e1) : unref(command_taker_ccc->e1).operator std::string());
		}
		else if (command_taker_ccc->command)
		{
			ele_ptr = command_taker_ccc->cast_to_ele();
			cmd_iter.insert(ele_ptr);
		}
		if (command_taker_ccc->command)
		{
			commands.push_back(command_taker_ccc->to_command());
			_log.logout("A command has been taken", _Expression::is_(ele_ptr,
				ele_type::taken) ?
				taken_str(ele_ptr) : unref(ele_ptr).operator std::string());
		}
		_log.logout("Current sources view:", ele_ptrs_to_string(sources, commands));
		
	}
	commands.push_back(make_command(push, sources.back()));
	_log.logout("Last element in segment pushed to stack, commands count =", std::to_string(commands.size()));
	_log.last_behavior();
	return 0;
}

s_sc_ptrs exp_segment::share_commands()
{
	return commands;
}

std::string exp_segment::taken_str(s_ele_ptr ptr)
{
	return "@" + query_taken(ptr->cast_to<exp_taken>(), commands);
}

auto_iterator<s_ele_ptrs> exp_segment::highest_level_operator()
{
	_log.behave(__FUNCTION__);
	//_log.debug(debug_off);
	auto_iterator<s_ele_ptrs> vit(sources);
	int level = -1;
	_log.logout("searching for highest level of operators:", "processing");
	while(vit.find([&](s_ele_ptr e)
		{
			_log.logout("current_level:", std::to_string(level));
			if (_Expression::is_(e, ele_type::opr))
			{
				auto pe = e->cast_to<exp_opr>();
				if (!pe) { _log.logout(e->e_str, "illegal format of operator:"); return false; }
				if ((int)(pe->level) > level) { 
					_log.logout("highest priority operator:", *pe);
					level = pe->level; 
					return true;
				}
			}
			return false;
		}));
	//_log.debug(debug_on);
	_log.last_behavior();
	return vit;
}

void exp_segment::import_source(auto_iterator<s_ele_ptrs> vit, int offset)
{
	_log.behave(__FUNCTION__);
	_log.logout("Importing segment");
	auto e_p = this->cast_to_ele();
	if (!e_p) { _log.logout("Error", "Failed to convert to primitive ele type"); throw _log; }
	if (offset != -1) 
	{ 
		auto _vec = vit.sub_vec(offset);
		sources = _vec;
		vit.erase(offset);
		vit.insert(e_p);
		_log.logout("Segment has taken some elements, count = ", std::to_string(offset));
	}
	else
	{
		sources = vit.vec();
		vit.vec().clear();
		vit.vec().push_back(e_p);
		vit.to_begin();
		_log.logout("Segment has taken all elements! count = ", std::to_string(sources.size()));
	}
	_log.logout("Finished!");
	auto_iterator<s_ele_ptrs> vit_del_seg_opr(sources);
	vit_del_seg_opr.to_begin();
	vit_del_seg_opr.find([&](s_ele_ptr e)
		{
			if (_Expression::is_(e, ele_type::opr))
			{
				auto pe = e->cast_to<exp_opr>();
				if (!pe) { _log.logout(e->e_str, "illegal format of e__ operator:"); return false; }
				if (pe->opr_t == exp_opr::opr_order::e__) { _log.logout("Deleting segment operator:", *pe); return true; }
			}
			return false;
		});
	vit_del_seg_opr.erase();
	//move to end to delete the last element
	vit_del_seg_opr.to_end();
	vit_del_seg_opr.find([&](s_ele_ptr e)
		{
			if (_Expression::is_(e, ele_type::opr))
			{
				auto pe = e->cast_to<exp_opr>();
				if (!pe) { _log.logout(e->e_str, "illegal format of __e operator:"); return false; }
				if (pe->opr_t == exp_opr::opr_order::__e) { _log.logout("Deleting segment operator:", *pe); return true; }
			}
			return false;
		});
	vit_del_seg_opr.erase();
	_log.last_behavior();
}


s_seg_ptr make_segment()
{
	return std::make_shared<exp_segment>();
}
