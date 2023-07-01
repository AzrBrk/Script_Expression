#pragma once
#include"include_file.h"
#include"statement_register.h"
//restrict the type of exp_ele derived class
template<class T>
concept ELE_BASE = requires(T t)
{
	t.e_type;
	t.e_str;
};
//primitive element class, all classes derived from this class will be stored in a command in primitive element type
//except for command in script_command
class exp_ele : public std::enable_shared_from_this<exp_ele>, public statement_register
{
public:
	using string = std::string;
	//element type should not be modified after initialization, unless undefined
	enum class ele_type
	{
		undefined,
		var,
		num,
		opr,
		taken,
		segment,
		invoke,
		cpp_func,
	};
	exp_ele();
	exp_ele(string str);
	exp_ele(string str, ele_type t);
	exp_ele(const exp_ele& e) = default;
	virtual ~exp_ele() = default;
	bool operator ==(string str);
	bool operator ==(ele_type t);//quickly compare the type of element
	operator std::string();
	ele_type e_type;
	string  e_str;
	template<ELE_BASE e_t>
	static std::shared_ptr<e_t> ele_cast();
	template<ELE_BASE e_t>
	std::shared_ptr<e_t> cast_to();
	std::shared_ptr<exp_ele> cast_to_ele();//directly cast to exp_ele, but unsafe
	//match to another ele_base_ptr, could be useful
	std::shared_ptr<exp_ele> matched{ nullptr };
	std::shared_ptr<exp_ele> owner{ nullptr };//indicate the owner of this element, could be useful

	BOOST_DESCRIBE_CLASS(exp_ele, (),(),(),())
};


typedef std::shared_ptr<exp_ele> s_ele_ptr;
typedef std::vector<s_ele_ptr> s_ele_ptrs;
template<typename... Args>
s_ele_ptr make_ele(Args ...args);

template<ELE_BASE e_t>
inline std::shared_ptr<e_t> exp_ele::ele_cast()
{
	return std::shared_ptr<e_t>(shared_from_this());
}

template<ELE_BASE e_t>
inline std::shared_ptr<e_t> exp_ele::cast_to()
{
	auto ptr = shared_from_this();
	return std::dynamic_pointer_cast<e_t>(ptr);
}

template<typename ...Args>
inline s_ele_ptr make_ele(Args ...args)
{
	return std::make_shared<exp_ele>(std::forward<Args>(args)...);
}
