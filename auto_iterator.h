#pragma once
#include"include_file.h"
#include"statement_register.h"
template<class T>
concept VEC_TYPE = requires(T t)
{
	t.begin();
	t.end();
	t.size();
	t.empty();
};
template<VEC_TYPE vec_t>
class auto_iterator : public statement_register
{
public:
	auto_iterator(vec_t& v);//the auto_iterator must be initialized with a vec_t
	vec_t& vec();
	size_t size();
	bool safe_verify_operate(bool direction = true);//if failed, ++ or -- will not be executed
	auto_iterator<vec_t> operator++(int);
	auto_iterator<vec_t> operator--(int);
	auto_iterator<vec_t> at(unsigned int index);//return an auto_iterator which points at index
	void to_begin();
	void to_end();//move to the last element of the vec but not the end()
	bool at_begin();
	bool first_time_at_begin();//needed because there is no element before the first element
	bool at_end();
	bool set_at(unsigned int offset);//set the iterator at offset, last_move_offset will be  not be set with this function
	void move(int offset);
	template<class func_type>
	void move_with(int offset, func_type f);
	template<class func_type>
	void move_with_ref(int offset, func_type& f);
	template<class bf>
	void move_if_ref(int offset, bf& f);
	template<class bf>
	void move_if(int offset, bf f);
	size_t index();
	auto& data();
	int get_last_move_offset();//get and reset counts of steps moved
	int go_back();//move to the last position based on last_move_offset
	void erase();
	void erase(int offset);
	void insert(auto ele);
	void replace(auto ele);
	vec_t sub_vec(int offset);//create a new vec_t with elements from offset to the curent position
	template<class func_type>
	bool find(func_type f);
	bool find_data(auto ele);
	bool find_unref_data(auto ele);//use this if the data is a pointer to get the unref data
	template<class func_type>
	void remove_all_at(func_type f, int offset);
private:
	struct start_counter
	{
		operator bool();
		short count{0};
		void reset();
	};
	start_counter _sc{};
	int last_move_offset{ 0 };
	bool at_edge();
	vec_t::iterator current_iter;
	vec_t& ref_v;
};
#define move_with_func(name, auto_iter) auto name = [&](decltype(auto_iter.data()) ele, decltype(auto_iter)* p)
#define move_if_func(name,  auto_iter) auto name = [&](decltype(auto_iter.data()) ele, decltype(auto_iter)* p)
#define move_if_lambda(auto_iter) [&](decltype(auto_iter.data()) ele, decltype(auto_iter)* p)
#define move_with_lambda(auto_iter) [&](decltype(auto_iter.data()) ele, decltype(auto_iter)* p)
#define delete_if(exp)\
			if(exp){\
				p->erase();\
				(*p)--;\
					}
#define r_delete_if(exp)\
			if(exp){\
				p->erase();\
					}
#define negative_num(x) 0 - x
template<VEC_TYPE vec_t>
inline auto_iterator<vec_t>::auto_iterator(vec_t& v)
	:
	ref_v(v), current_iter(v.begin())
{
}

template<VEC_TYPE vec_t>
inline vec_t& auto_iterator<vec_t>::vec()
{
	return ref_v;
}

template<VEC_TYPE vec_t>
inline size_t auto_iterator<vec_t>::size()
{
	return ref_v.size();
}

template<VEC_TYPE vec_t>
inline bool auto_iterator<vec_t>::safe_verify_operate(bool direction)
{
	//此函数用于验证是否可以执行++ 或 --， 所以 begin(), end()时无效
	//direction， true 验证++ 操作， false 验证-- 操作
	if (direction) _return_if(at_end(), false);
	if (!direction) _return_if(at_begin(), false);
	return true;
}

template<VEC_TYPE vec_t>
inline void auto_iterator<vec_t>::to_begin()
{
	current_iter = ref_v.begin();
}

template<VEC_TYPE vec_t>
inline void auto_iterator<vec_t>::to_end()
{
	current_iter = ref_v.end() - 1;
}

template<VEC_TYPE vec_t>
inline bool auto_iterator<vec_t>::at_begin()
{
	return current_iter == ref_v.begin();
}

template<VEC_TYPE vec_t>
inline bool auto_iterator<vec_t>::first_time_at_begin()
{
	return _sc;
}

template<VEC_TYPE vec_t>
inline bool auto_iterator<vec_t>::at_end()
{
	return current_iter == ref_v.end();
}

template<VEC_TYPE vec_t>
inline bool auto_iterator<vec_t>::set_at(unsigned int pos)
{
	_return_if(ref_v.empty(), false);
	_return_if(pos > ref_v.size() - 1, false);
	current_iter = ref_v.begin() + pos;
	return true;
}

template<VEC_TYPE vec_t>
inline void auto_iterator<vec_t>::move(int offset)
{
	//auto_iterator 重载的++，--已经判定执行时是否会越界，因此不会越界。
	
	if (offset > 0)
		while (offset--) (*this)++;
	else
		while (offset++) (*this)--;
}

template<VEC_TYPE vec_t>
template<class func_type>
inline void auto_iterator<vec_t>::move_with_ref(int offset, func_type& f)
{
	_sc.reset();
	if (offset > 0)
		while (offset-- && !at_edge()) { f(data(), this); move(1); }
	else
		while (offset++ && !at_edge()) { f(data(), this); move(-1); }

}


template<VEC_TYPE vec_t>
template<class bf>
inline void auto_iterator<vec_t>::move_if(int offset, bf f)
{
	if (offset > 0)
		while (offset-- && f(data(), this))
			move(1);

	else
		while (offset++ && f(data(), this))
			move(-1);
}

template<VEC_TYPE vec_t>
inline size_t auto_iterator<vec_t>::index()
{
	return current_iter - ref_v.begin();
}


template<VEC_TYPE vec_t>
inline auto_iterator<vec_t> auto_iterator<vec_t>::operator++(int)
{
	auto tmp{ *this };
	if (safe_verify_operate()) ++current_iter, ++last_move_offset;
	return tmp;
}

template<VEC_TYPE vec_t>
inline auto_iterator<vec_t> auto_iterator<vec_t>::operator--(int)
{
	auto tmp{ *this };
	if (safe_verify_operate(false)) --current_iter, --last_move_offset;
	return tmp;
}

template<VEC_TYPE vec_t>
inline auto_iterator<vec_t> auto_iterator<vec_t>::at(unsigned int index)
{
	auto_iterator<vec_t> res(ref_v);
	if (index >= ref_v.size())
	{
		res.register_state(FALSE_AUTO_ITERATOR, true);
		return res;
	}
	res.set_at(index);
	return res;
}

template<VEC_TYPE vec_t>
inline auto& auto_iterator<vec_t>::data()
{
	if (ref_v.empty()) throw std::exception((std::string(__func__) + "::error: empty vector").c_str());//我写了那么多限制你居然给我在空容器里面找data()?!!!
	_return_if(at_end(), *(current_iter - 1));
	return *current_iter;
}

template<VEC_TYPE vec_t>
inline int auto_iterator<vec_t>::get_last_move_offset()
{
	auto offset = last_move_offset;
	last_move_offset = 0;
	return offset;
}

template<VEC_TYPE vec_t>
inline int auto_iterator<vec_t>::go_back()
{
	int offset = get_last_move_offset();
	current_iter = offset < 0 ? current_iter + std::abs(offset) : current_iter - std::abs(offset);
	return offset;
}

template<VEC_TYPE vec_t>
inline void auto_iterator<vec_t>::erase()
{
	current_iter = ref_v.erase(current_iter);
}

template<VEC_TYPE vec_t>
inline void auto_iterator<vec_t>::erase(int offset)
{
	current_iter = (offset >= 0 ? ref_v.erase(current_iter, current_iter + offset) : ref_v.erase(current_iter - std::abs(offset), current_iter));
}

template<VEC_TYPE vec_t>
inline void auto_iterator<vec_t>::insert(auto ele)
{
	current_iter = ref_v.insert(current_iter, ele);
}

template<VEC_TYPE vec_t>
inline void auto_iterator<vec_t>::replace(auto ele)
{
	erase();
	insert(ele);
	//move(-1);
}

template<VEC_TYPE vec_t>
inline vec_t auto_iterator<vec_t>::sub_vec(int offset)
{
	auto res_vec = vec_t(current_iter, current_iter + offset);
	return res_vec;
}

template<VEC_TYPE vec_t>
inline bool auto_iterator<vec_t>::find_data(auto ele)
{
	return find([=](decltype(ele) v_ele)
	{
		return v_ele == ele;
	});
}

template<VEC_TYPE vec_t>
inline bool auto_iterator<vec_t>::find_unref_data(auto ele)
{
	return find([=](decltype(data()) v_ele)
	{
		return unref(v_ele) == ele;
	});
}

template<VEC_TYPE vec_t>
inline bool auto_iterator<vec_t>::at_edge()
{
	_return_if(at_begin() && !_sc, true);
	_return_if(at_end(), true);
	return false;
}

template<VEC_TYPE vec_t>
template<class func_type>
inline void auto_iterator<vec_t>::move_with(int offset, func_type f)
{
	//auto_iterator 重载的++，--已经判定执行时是否会越界，因此不会越界。
	_sc.reset();
	if (offset > 0)
		while (offset-- && !at_edge()) { f(data(), this); move(1); }
	else
		while (offset++ && !at_edge()) { f(data(), this); move(-1); }
}

template<VEC_TYPE vec_t>
template<class bf>
inline void auto_iterator<vec_t>::move_if_ref(int offset, bf& f)
{
	if (offset > 0)
		while (offset-- && f(data(), this)) 
			move(1);
		
	else
		while (offset++ && f(data(), this))
			move(-1);
}

template<VEC_TYPE vec_t>
template<class func_type>
inline bool auto_iterator<vec_t>::find(func_type f)
{
	register_state(FALSE_AUTO_ITERATOR, true);
	auto f_iter = std::find_if(current_iter, ref_v.end(), f);
	_return_if(f_iter == ref_v.end(), false);
	pop_state(FALSE_AUTO_ITERATOR);
	last_move_offset += f_iter - current_iter;
	current_iter = f_iter;
	return true;
}

template<VEC_TYPE vec_t>
template<class func_type>
inline void auto_iterator<vec_t>::remove_all_at(func_type f, int offset)
{
	ref_v.erase(std::remove_if(current_iter, current_iter + offset, f));
}

template<VEC_TYPE vec_t>
inline auto_iterator<vec_t>::start_counter::operator bool()
{
	if (count > 0)
	{
		count = 0;
		return false;
	}
	++count;
	return true;
}

template<VEC_TYPE vec_t>
inline void auto_iterator<vec_t>::start_counter::reset()
{
	count = 0;
}
