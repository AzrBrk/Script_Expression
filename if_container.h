#pragma once
#include"include_file.h"
//模板 conditional container从外部引入一个 functype 类型的函数，该函数返回bool值，假如为true，则放入vectype类型的容器
template<class func_type, class vec_type>
class conditional_container
{
public:
	conditional_container(func_type f);
	//由于不知道函数f的参数，所以 operator()必须接受可变参数列表
	template<typename DATA_T, typename... Args>
	bool operator()(DATA_T t, Args... args);
	vec_type& data();
	struct placeholder {};
private:
	func_type func;
	vec_type vec{};
};
#define auto_if_container(func, typedata) conditional_container<decltype(func), decltype(typedata)>(func) 
template<typename func_type, typename vec_type>
conditional_container<func_type, vec_type> make_conditional_container(func_type f, vec_type v);

template<class func_type, class vec_type>
inline conditional_container<func_type, vec_type>::conditional_container(func_type f)
	:
	func(f)
{
}

template<class func_type, class vec_type>
inline vec_type& conditional_container<func_type, vec_type>::data()
{
	return vec;
}

template<class func_type, class vec_type>
template<typename DATA_T, typename... Args>
inline bool conditional_container<func_type, vec_type>::operator()(DATA_T t, Args... args)
{
	if (func(t, std::forward<Args>(args)...))
	{
		vec.push_back(t);
		return true;
	}
	else return false;
}

template<typename func_type, typename vec_type>
inline conditional_container<func_type, vec_type> make_conditional_container(func_type f, vec_type v)
{
	return conditional_container<func_type, vec_type>(f);
}
