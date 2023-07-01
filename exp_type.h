#pragma once
#include"exp_ele.h"
#include"exp_log_debug.h"
template<typename T>
class exp_type :
	public exp_ele
{
public:
	using ex_type = T;
	static std::shared_ptr<exp_type<T>> make_type(s_ele_ptr e)
	{
		return std::make_shared<exp_type<T>>(*e);
	}
	static std::shared_ptr<exp_type<T>> make_type_from(ex_type const& _value)
	{
		return std::make_shared<exp_type<T>>(_value);
	}
	exp_type() = default;
	exp_type(exp_ele ele) :exp_ele(ele) {}
	template<typename U>
	exp_type(const U& e) {
		_log.object("exp_type");
		_log.behave("copy_constructor");
		ostringstream oss;
		oss << e;
		e_str = oss.str();
		_log.last_behavior();
	}
	ex_type strong_cast()
	{
		_log.object("exp_type");
		_log.behave("strong_cast");
		ex_type value{};
		std::istringstream iss{ e_str };
		iss >> value;
		if (iss.fail())
			_log.logout("warning:",e_str + " is not a " + typeid(ex_type).name());
		_log.last_behavior();
		return value;
	}
private:
	exp_log_debug _log;
};


