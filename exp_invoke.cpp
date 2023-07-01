#include "exp_invoke.h"

exp_invoke::exp_invoke(s_ele_ptr attach_ele)
	:
	exp_invoke()
{
	invoke_ele = attach_ele;
}

exp_invoke::exp_invoke(s_seg_ptr seg_ptr)
	:
	exp_invoke()
{
	link_to_segment = seg_ptr;
	invoke_ele = seg_ptr->owner;
}

exp_invoke::exp_invoke(auto_iterator<s_ele_ptrs> vit)
	:
	exp_invoke()
{

	invoke_ele = vit.data();
	auto matched_seg = invoke_ele->matched;
	if (matched_seg)
	{
		//auto_iterator move to the begin of the segment
		vit.find([=](s_ele_ptr e)//???
			{
				//_log.logout("comparing:", std::to_string((unsigned int)e.get()) + "with" + std::to_string((unsigned int)matched_seg.get()));
				return (unsigned int)e.get() == (unsigned int)matched_seg.get();
			});
		//verify the begin operator
		if (vit.data()->matched)
		{
			_log.logout("begin operator verified");
		}
		else
		{
			_log.logout("Fatal Error:", "begin operator not verified");
			_log.logout("begin operator not verified at " + std::to_string(vit.index()) + ":", vit.data()->operator std::string());
			throw _log;
		}
		//save a copy of the begin iterator
		auto begin_iter = vit;//???
		//reset offset recorder
		//vit.to_begin();
		vit.get_last_move_offset();
		//move to the end of the segment
		vit.pop_state(FALSE_AUTO_ITERATOR);
		vit.find([&](s_ele_ptr e)
			{
				//_log.logout("comparing:", std::to_string((unsigned int)e->matched.get()) + "with" + std::to_string((unsigned int)begin_iter.data().get()));
				return e->matched.get() == begin_iter.data().get();
			});
		//verify the end operator
		if (vit.get_state(FALSE_AUTO_ITERATOR))
		{
			_log.logout("Fatal Error:", "end operator not found");
			throw _log;

		}
		else
		{
			_log.logout("end operator verified");
		}
		auto seg_offset = vit.get_last_move_offset() + 1;
		_log.logout("segment offset:", std::to_string(seg_offset));
		_log.logout("importing segment...");
		link_to_segment = make_segment();
		link_to_segment->owner = invoke_ele;
		link_to_segment->import_source(begin_iter, seg_offset);
		link_to_segment->compile_source();
	}
	else
	{
		_log.logout("Fatal Error:", "matched segment not found");
		throw _log;
	}
}

exp_command_pointer exp_invoke::get_commands_ptr()
{
	return exp_command_pointer(link_to_segment->commands);
}

void exp_invoke::push(s_ele_ptr ptr)
{
	_stack.push(ptr);
}

void exp_invoke::wrap_segment()
{
	_log.behave(__FUNCTION__);
	if (!link_to_segment)
	{
		_log.logout("Fatal Error:", "segment not found");
		throw _log;
	}
	if (!link_to_segment->wrap_needed)
	{
		_log.logout(link_to_segment->operator std::string(), "segment already wrapped");
		return;
	}
	_log.logout("wrapping segment...");
	_log.logout("creating call operator...");
	auto call_opr = std::make_shared<exp_opr>("CALL");
	auto call_cmd = make_command(call_opr, invoke_ele);
	auto pop_opr = std::make_shared<exp_opr>("POP");//return value
	auto pop_cmd = make_command(pop_opr, invoke_ele);
	auto_iterator<s_sc_ptrs> cmd_iter = link_to_segment->commands;
	_log.logout("searching for invoke operators...");
	cmd_iter.move_with(cmd_iter.vec().size(), move_with_lambda(cmd_iter)
	{
		if (ele->command->opr_t == exp_opr::opr_order::e_invoke)
		{
			exp_command_pointer c = *p;
			cout << c << "=>";
			if (ele->command->e_str == ",")
				ele->command->e_str = "PUSH";
			cout << c << endl;
		}

	});
	_log.logout("inserting call operator...");
	link_to_segment->commands.push_back(call_cmd);
	_log.logout("inserting pop operator...");
	link_to_segment->commands.push_back(pop_cmd);
	//wrap return value with a taken element to link to invoke element
	_log.logout("wrapping return value...");
	link_to_segment->link_to_command = pop_cmd;
	_log.logout("wrapping segment completed");
	_log.last_behavior();
}

s_ele_ptr exp_invoke::pop()
{
	auto p_e = _stack.top();
	_stack.pop();
	return p_e;
}

s_ele_ptr exp_invoke::top()
{
	return _stack.top();
}

int exp_invoke::size()
{
	return _stack.size();
}

void exp_invoke::execute_command(s_sc_ptr cmd)
{
	_log.behave(__FUNCTION__);
	if (!cmd)
	{
		_log.logout("Fatal Error:", "command not found");
		throw _log;
	}
	string cmd_str = cmd->command->e_str;
	if (cmd_str == "PUSH")
	{
		auto push_ele = cmd->e1;
		if (!push_ele)
		{
			_log.logout("Fatal Error:", "push element not found");
			throw _log;
		}
		if (*push_ele == ele_type::taken)
		{
			_log.logout("ele_type =", " taken, calculating...");
			push_ele->cast_to<exp_taken>()->fetch_value();
			_log.logout("value = ", push_ele->operator std::string());
		}
		else if (*push_ele == ele_type::segment)
		{
			auto seg = push_ele->cast_to<exp_segment>();
			if(!seg)
			{
				_log.logout("Fatal Error:", "segment not found");
				throw _log;
			}
			if(!seg->link_to_command->value)
			{
				_log.logout("Fatal Error:", "segment not linked");
				throw _log;
			}
			push_ele = seg->link_to_command->value;
		}
		_log.logout("pushing element:", push_ele->operator std::string());
		push(push_ele);
	}
	else if (cmd_str == "CALL")
	{
		auto call_ele = cmd->e1;
		if (!call_ele)
		{
			_log.logout("Fatal Error:", "call element not found");
			throw _log;
		}
		auto func_name = call_ele->e_str;
		script_functions::funcs fl;
		try
		{
			call_func_by_str(fl, func_name);
		}
		catch (const std::exception& e)
		{
			_log.logout("Fatal Error:", " error while calling:" + func_name);
			throw _log;
		}

	}
	else if (cmd_str == "POP")
	{
		if (!_return)
		{
			_log.logout("Fatal Error:", "return element not found");
			throw _log;
		}
		else if (*_return == "empty")
		{
			_log.logout("warning:", "try to return from 'void'");
			_log.logout("debug:", "debug is on, trying to return 0 to host");
			_return->e_str = "0";
			_return->e_type = ele_type::num;
		}
		cmd->value = _return;
	}
}

void exp_invoke::link_func(s_ele_ptr e)
{
	_return = invoke_ele->matched;
	invoke_ele->matched = e;;
}

