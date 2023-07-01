#pragma once
#include<iostream>
#include"include_file.h"
#include"exp_ele.h"
#include"exp_type.h"


namespace script_functions
{

	struct func_e_plus :exp_ele
	{
		using arg_type = std::tuple<int, int>;
		int _2;
		int _1;

		arg_type construct_arguments()const
		{
			auto res = std::make_tuple(_2, _1);
			return res;
		}
		s_ele_ptr execute()const
		{
			auto args = construct_arguments();
			auto res = exp_type<int>::make_type_from(std::get<0>(args) + std::get<1>(args));
			return res;
		}
		s_ele_ptr operator()()const
		{
			return execute();
		}
		string f_execute{ "e_plus" };
	};
	BOOST_DESCRIBE_STRUCT(func_e_plus, (), (f_execute, _2, _1))

		struct func_print :exp_ele
	{
		int _4{};
		int _3{};
		int _2{};
		int _1{};
		s_ele_ptr execute()const
		{
			_1 ? cout << _1 << " " : cout << "";
			_2 ? cout << _2 << " " : cout << "";
			_3 ? cout << _3 << " " : cout << "";
			_4 ? cout << _4 << " " : cout << "";
			cout << endl;
			return make_ele("empty", exp_ele::ele_type::var);
		}
		s_ele_ptr operator()()const
		{
			return execute();
		}
		string f_execute{ "print" };
	};
	BOOST_DESCRIBE_STRUCT(func_print, (), (f_execute, _4, _3, _2, _1))
		struct funcs
	{
		func_e_plus e_plus() { return func_e_plus(); }
		func_print print() { return func_print(); }
	};
	BOOST_DESCRIBE_STRUCT(funcs, (), (e_plus, print))
}