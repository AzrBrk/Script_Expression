#include "exp_taken.h"

exp_taken::exp_taken()
	:
	exp_ele("", exp_ele::ele_type::taken)
{
	_log.object(typeid(*this).name());
}

exp_taken::exp_taken(s_sc_ptr link)
	:
	exp_ele("", exp_ele::ele_type::taken),
	script_command(*link)
{}

//Convert a taken element to a command, the exp_taken element will not keep the original elements anymore!
s_sc_ptr exp_taken::to_command()
{
	auto sc = make_command(command, e1, e2, e3);
	link_to_command = sc;
	e1 = e2 = e3 = nullptr;
	return sc;
}

s_ele_ptr exp_taken::e(unsigned int index = 0)
{
	_log.behave(__FUNCTION__);
	if (!link_to_command)
	{
		_log.logout("Error: Invalid script_command smart pointer!:", "This taken pointer has been converted to a command yet!"); 
		throw _log;
	}
	switch (index)
	{
	case 0:
		if (!link_to_command->value) _log.logout("Warning: Invalid value!:", "This command unit has not be correctly calculated!!!");
		return link_to_command->value;
	case 1:
		_log.last_behavior();  return link_to_command->e1;
	case 2:
		_log.last_behavior();  return link_to_command->e2;
	case 3:
		_log.last_behavior(); return link_to_command->e3;
	default:
		break;
	}
	_log.last_behavior(); return nullptr;
}


auto_iterator<s_ele_ptrs> exp_taken::take_opr(auto_iterator<s_ele_ptrs> _vit)
{
	_vit.register_state(FALSE_AUTO_ITERATOR, true);
	_return_if(_vit.vec().empty(), _vit);
	_return_if(!_Expression::is_(_vit.data(), exp_ele::ele_type::opr), _vit);
	move_if_func(ignore_blank, _vit)
	{
		return ele->e_str == " ";
	};
	auto take_side = [&](bool direction, auto_iterator<s_ele_ptrs>& _vit)
	{
		_vit.get_last_move_offset();//重置迭代器计数器
		int move_offset = direction ? _vit++.vec().size() : negative_num(_vit--.vec().size());
		_vit.move_if(move_offset, ignore_blank);//迭代器会在非空格的元素停止
		auto res = _vit.data();
		_vit.erase(_vit.go_back());//go back函数会返回 erase 函数所需要的偏移量，并且回到上一次重置计数之后的位置
		return res;
	};
	auto opr_p = _vit.data()->cast_to<exp_opr>();
	_return_if(!opr_p, _vit);//防止转换出错
	switch (opr_p->opr_t)
	{
	case exp_opr::opr_order::e_op_e://二元操作符
	{
		e1 = take_side(false, _vit);//取左边的第一个值
		e2 = take_side(true, _vit);//取右边第一个值
		
		break;
	}
	case exp_opr::opr_order::op_e://一元后置操作符
	{
		e1 = take_side(true, _vit);//取右边第一个值
		
		break;
	}
	case exp_opr::opr_order::e_invoke://函数调用
	{
		e1 = take_side(false, _vit);//取左边第一个值
		
		break;
	}
	case exp_opr::opr_order::e_op://一元前置操作符
	{
		e1 = take_side(false, _vit);//取左边的第一个值
		
		break;
	}
	default:
		break;
	}
	command = opr_p;
	_vit.erase();
	_vit.modify_state(FALSE_AUTO_ITERATOR, false);
	return _vit;
}


s_t_ptr make_taken(s_sc_ptr link)
{
	auto res = std::make_shared<exp_taken>();
	_return_if(link == nullptr, res);
	res->set_command(link->command, link->e1, link->e2, link->e3);
	return res;
}

std::string query_taken(s_t_ptr tk_ptr, s_sc_ptrs at_cmds_list)
{
	auto_iterator<s_sc_ptrs> list_iter(at_cmds_list);
	if (list_iter.find([=](s_sc_ptr _e)
		{
		return tk_ptr->link_to_command == _e;
		}))
		return std::to_string(list_iter.index());
	else return FALSE_STRING;
}

std::string ele_ptrs_to_string(s_ele_ptrs v_eles, s_sc_ptrs at_cmds_list)
{
	std::string res_str{};
	auto_iterator<s_ele_ptrs> v_iter(v_eles);
	v_iter.move_with(v_eles.size(), move_with_lambda(v_iter)
	{
		if (_Expression::is_(ele, exp_ele::ele_type::taken))
		{
			auto t_ptr = ele->cast_to<exp_taken>();
			res_str += "@" + query_taken(t_ptr, at_cmds_list);
		}
		else
		{
			res_str += *ele;
		}
		
	});
	return res_str;
}

exp_command_pointer::exp_command_pointer(s_sc_ptrs& cmd_list)
	:
	auto_iterator<s_sc_ptrs>(cmd_list)
{
	__log.object(typeid(*this).name());
}

exp_command_pointer::exp_command_pointer(auto_iterator<s_sc_ptrs> iter)
	:
	auto_iterator<s_sc_ptrs>(iter),
	_address(iter.index())
{}

exp_command_pointer& exp_command_pointer::operator=(const exp_command_pointer source)
{
	_address = source._address;
	return *this;
}

bool exp_command_pointer::operator==(const exp_command_pointer& another)
{
	if (_address == another._address)
		return true;
	return false;
}

s_sc_ptr& exp_command_pointer::operator*()
{
	set_at(_address);
	return data();
}

s_sc_ptr exp_command_pointer::operator->()
{
	set_at(_address);
	return data();
}

exp_command_pointer::operator unsigned int& ()
{
	if (!set_at(_address))
		auto_iterator<s_sc_ptrs>::register_state(FALSE_AUTO_ITERATOR, true);
	return _address;
}

unsigned int exp_command_pointer::address() const
{
	return _address;
}

void exp_command_pointer::link_to_executale(s_ele_ptr e_func)//?
{
	auto cmd = *(*this);
	cmd->exec_matched = e_func;
}

