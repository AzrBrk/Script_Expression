#pragma once
#include"include_file.h"
#include "exp_ele.h"
#include "operators.h"
#include "auto_iterator.h"
#include "script_command.h"
#include "_Expression.h"
#include "exp_log_debug.h"
class exp_taken :
    public exp_ele, public script_command
{
public:
	exp_taken();
	exp_taken(s_sc_ptr link);//link to a command unit, link_to_command will be set
	s_sc_ptr to_command();//convert this element to a command unit
	s_ele_ptr e(unsigned int index);//quick access to a linked element, to use this function, link_to_command must be set
	s_sc_ptr link_to_command{nullptr};//link to a command unit
	auto_iterator<s_ele_ptrs> take_opr(auto_iterator<s_ele_ptrs> _vit);//extract a operator from sources, according to the operator's order
private:
	exp_log_debug _log;
};
typedef std::shared_ptr<exp_taken> s_t_ptr;
s_t_ptr make_taken(s_sc_ptr link = nullptr);

//A taken element only work in a commands list, a commands list that contains the taken elements must be provided.
std::string query_taken(s_t_ptr tk_ptr, s_sc_ptrs at_cmds_list);

//Get string from sources, optionally provide a elements vector
std::string ele_ptrs_to_string(s_ele_ptrs v_eles, s_sc_ptrs at_cmds_list);

//A pointer type, it must be initialized with a command list
class exp_command_pointer :public exp_ele, public auto_iterator<s_sc_ptrs>
{
public:
	exp_command_pointer(s_sc_ptrs&);
	exp_command_pointer(auto_iterator<s_sc_ptrs>);
	exp_command_pointer& operator=(const exp_command_pointer);
	friend std::ostream& operator<<(std::ostream& os, exp_command_pointer& ptr)
	{
		os << "@" << ptr._address << ":";
		os << (*ptr)->command->e_str << " ";
		//if an element is taken, it will be replaced by its address
		if ((*ptr)->e1)
		{
			os << ((*ptr)->e1->e_type == ele_type::taken ? "@" + query_taken((*ptr)->e1->cast_to<exp_taken>(), ptr.vec()) : (*ptr)->e1->e_str) << " ";
		}
		if ((*ptr)->e2)
		{
			os << ",";
			os << ((*ptr)->e2->e_type == ele_type::taken ? "@" + query_taken((*ptr)->e2->cast_to<exp_taken>(), ptr.vec()) : (*ptr)->e2->e_str) << " ";
		}
		if ((*ptr)->e3)
		{
			os << ",";
			os << ((*ptr)->e3->e_type == ele_type::taken ? "@" + query_taken((*ptr)->e3->cast_to<exp_taken>(), ptr.vec()) : (*ptr)->e3->e_str) << " ";
		}
		return os;
	}//output address and command
	bool operator==(const exp_command_pointer&);//compare address
	s_sc_ptr& operator*();//return reference to command
	s_sc_ptr operator ->();//return pointer to command
	operator unsigned int&();//return reference to address
	unsigned int address() const;//Get the address of the pointer
	void link_to_executale(s_ele_ptr e_func);//link to a executable element
	template<typename EXEC_TYPE>
	void exec();//execute the command, both exp_invoke and exp_cpp_func
private:
	unsigned int _address{0};
	exp_log_debug __log;
};

template<typename EXEC_TYPE>
inline void exp_command_pointer::exec()
{
	__log.behave("exec");
	auto cmd = **this;
	if (!cmd->exec_matched)
	{
		__log.logout("undefined executable element");
		throw __log;
	}
	auto func = cmd->exec_matched->cast_to<EXEC_TYPE>();
	if (!func)
	{
		__log.logout("executable element is not a function");
		throw __log;
	}
	cmd->value = func(cmd->e1, cmd->e2, cmd->e3);
	__log.last_behavior();
}
