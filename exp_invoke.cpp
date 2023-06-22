#include "exp_invoke.h"

exp_invoke::exp_invoke(s_ele_ptr attach_ele)
{
	invoke_ele = attach_ele;
}

exp_invoke::exp_invoke(auto_iterator<s_ele_ptrs> vit)
{
	_log.object("exp_invoke");
	_log.behave(__FUNCTION__);
	invoke_ele = vit.data();
	auto matched_seg = invoke_ele->matched;
	if (matched_seg)
	{
		//auto_iterator move to the begin of the segment
		vit.find([=](s_ele_ptr e)//???
			{
				_log.logout("comparing:", std::to_string((unsigned int)e.get()) + "with" + std::to_string((unsigned int)matched_seg.get()));
				return (unsigned int)e.get() == (unsigned int)matched_seg.get();
			});
		//verify the begin operator
		if (vit.data()->matched)
		{
			_log.logout("begin operator verified");
		}
		else
		{
			_log.logout("Fatal Error:","begin operator not verified");
			_log.logout("begin operator not verified at " + std::to_string(vit.index()) +":", vit.data()->operator std::string());
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
				_log.logout("comparing:", std::to_string((unsigned int)e->matched.get()) + "with" + std::to_string((unsigned int)begin_iter.data().get()));
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
		auto seg_offset = vit.get_last_move_offset()+1;
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
			ele->e2 = invoke_ele;
			if(ele->command->e_str == ",")
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
	invoke_ele->matched = make_taken(link_to_segment->commands.back())->cast_to_ele();
	_log.logout("wrapping segment completed");
	_log.last_behavior();
}
