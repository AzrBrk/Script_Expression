#pragma once
#include"include_file.h"
#include"exp_type.h"
#include "exp_segment.h"
#include "exp_log_debug.h"
#include "exp_cpp_func.h"
#include "functions_list.h"
using namespace boost::describe;

//a exp_invoke means it can be called when it correctly matched to a compiled segment
//if a operator has attribute of 'e_invoke', the operator will be linked to a e_invoke element, usually the owner of the segment it belongs to
//the operator's effect will automatically apply to the exp_invoke, too
class exp_invoke :
	public exp_ele
{
public:
	exp_invoke() :exp_ele() {
		_log.object("exp_invoke");
		_log.behave(__FUNCTION__); 
		e_type = ele_type::invoke;
	};
	exp_invoke(s_ele_ptr attach_ele);
	exp_invoke(s_seg_ptr seg_ptr);
	//construct a exp_invoke with a auto_iterator<s_ele_ptrs> pointing to the owner of the segment
	exp_invoke(auto_iterator<s_ele_ptrs> vit);
	//
	exp_command_pointer get_commands_ptr();
	//push an element to the stack
	void push(s_ele_ptr ptr);
	//wrap the segment with commands to make it callable
	void wrap_segment();
	//pop an element from the stack
	s_ele_ptr pop();
	//get the top element of the stack
	s_ele_ptr top();
	//get the size of the stack
	int size();
	void execute_command(s_sc_ptr cmd);
	void link_func(s_ele_ptr e);
	template<class C1, class C2, class R>
	R _create_func_(C1& c1, R(C2::* f)())
	{
		R r = (c1.*f)();
		return r;
	}
	template<class func_list>
	void call_func_by_str(func_list& fl,string func_str)
	{
		using Fd = boost::describe::describe_members<func_list,
			boost::describe::mod_public | boost::describe::mod_function>;
		boost::json::value result;
		bool found = false;
		boost::mp11::mp_for_each<Fd>([&](auto m)
			{
				if (!found && m.name == func_str)
				{
					found = true;
					_call(_create_func_(fl, m.pointer), func_str);
					//auto r = call(result);
				}
			});
	}
	template<class T>
	void assign(T& _1, const s_ele_ptr& _2) { 
		_log.behave(__FUNCTION__);
		try
		{
			boost::json::value const& _value = boost::json::parse(_2->e_str);
			_1 = boost::json::value_to<T>(_value);
			_log.logout("=>", _2->e_str);
		}
		catch (const std::exception&)
		{
			_log.logout("warning:", "assign failed!:"+_2->operator std::string());
			_log.last_behavior();
		}
		_log.last_behavior();
	} 
	template <class T, class T2>
	void assign_str(T &t, const T2& _2) { t = _2; }
	template<class T1, class T2>
		bool compare(T1& _1, const T2& _2)
	{
			return _1 == _2;
	}

	template<class T1, class T2>
	bool compare_str(T1& _1, const T2& _2)
	{
		if (!std::is_same<T1, T2>::value)
		{
			_log.logout("warning: type mismatch!");
			return false;
		}
		else return compare(_1, _2);
	}

	template<class T,
		class Bd = boost::describe::describe_bases<T, mod_any_access>,
		class Md = describe_members<T, mod_any_access>,
		class En = std::enable_if_t<!std::is_union<T>::value>>
		void _call(T  t, std::string f_name)
	{
		string func_name;
		string arg_name;
		bool found = false;
		_log.behave(__FUNCTION__);
		boost::mp11::mp_for_each<Md>([&](auto m)
			{
				arg_name = m.name;
				if (found && !_stack.empty())
				{
					_log.logout("assigning ", m.name);
					assign(t.*m.pointer, pop());
				}
				
				else if (arg_name == "f_execute")
				{
					assign_str(func_name, t.*m.pointer);
					found = compare_str(func_name, f_name);
				}
				
				else if(_stack.empty())
				{
					_log.logout("warning:stack is empty!");
				}
			});
		if (found) { 
			_log.logout("calling ", func_name);
			_return = t(); 
			//exp_return();
			found = false; 
		}
		else _log.logout("no execute() found");

		_log.last_behavior();
	}
	s_ele_ptr exp_return() { link_to_segment->link_to_command->value = _return;  return _return; }

private:
	exp_log_debug _log;
	std::stack<s_ele_ptr> _stack;
	s_ele_ptr invoke_ele;
	s_ele_ptr _return{ nullptr };
	s_seg_ptr link_to_segment;
};

typedef std::shared_ptr<exp_invoke> s_i_ptr;
template<class ...Args>
s_i_ptr make_invoke(Args&& ...args)
{
	return std::make_shared<exp_invoke>(std::forward<Args>(args)...);
}