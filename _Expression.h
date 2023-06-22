#pragma once
#include"include_file.h"
#include"auto_iterator.h"
#include"exp_ele.h"
#include"exp_opr.h"
#include"exp_segment.h"

class _Expression
{
public:
    typedef std::shared_ptr<exp_ele> s_ele_ptr;
    typedef std::vector<s_ele_ptr> s_ele_ptrs;
    using string = std::string;
    // 该函数用于从字符串 str 中取出符合 f 函数条件的字符构成新的字符串并返回。
    // 如果指定了 take_num 参数，只会取出 take_num 个符合条件的字符。如果 take_num 小于等于 0，则会取出所有符合条件的字符。
    // str 参数为目标字符串，f 参数为函数类型，其参数为 char 类型，返回值类型为 bool 类型。
    template<typename func_type>
    static  string TAKE_WHEN(string& str, func_type f, int take_num = -1);
#define take_when(x,func) _Expression::TAKE_WHEN<decltype(func)>(x,func)
#define take_when_times(x,func, times)  _Expression::TAKE_WHEN<decltype(func)>(x, func, times)

    static bool is_not_number(char c);
    static bool is_alpha(char c);
    static bool is_down_slice(char c);
    static bool is_number(char c);
    static bool is_blank_space(char c);
    static bool is_not_number_or_alpha(char c);
    static bool is_owned(auto_iterator<s_ele_ptrs> v);
    struct is_available_var_name
    {
        bool b_first_char = true;
        bool operator()(char c);
    };

    static bool is_(s_ele_ptr& e, exp_ele::ele_type);
    static s_ele_ptrs devs;
    static void dev(std::string str);
    static void convert_operators();
    static s_ele_ptrs get_devs();

    //在元素容器中转化ele元素类型
    template<ELE_BASE et>
    static std::shared_ptr<et> ele_type_convert(auto_iterator<s_ele_ptrs>& _it);

    
private:
    _Expression() = default;
};

template<typename func_type>
inline std::string _Expression::TAKE_WHEN(std::string& str, func_type f, int take_num)
{
    bool b_num_cnt = false; // b_num_cnt 用于判断是否需要限制取出的字符数量
    string res; // res 用于记录符合条件的字符组成的字符串
    if (take_num >= 0)
        b_num_cnt = true;
    while (f(str.at(0))) // 循环取出符合条件的字符直到第一个不符合条件的字符
    {
        if (b_num_cnt && --take_num < 0) // 如果需要限制取出数量，并且已经取出了 take_num 个字符，则退出循环
            break;
        res += str.at(0); // 将符合条件的字符添加到结果字符串中
        str.erase(0, 1); // 删除已经取出的字符
        if (str.empty()) // 如果已经取出了所有字符，则退出循环
            break;
    }
    return res; // 返回取出的字符构成的字符串
}
template<ELE_BASE et>
inline std::shared_ptr<et> _Expression::ele_type_convert(auto_iterator<s_ele_ptrs>& _it)
{
    auto new_ptr = std::make_shared<et>(unref(_it.data()));
    _it.erase();
    _it.insert(new_ptr->cast_to_ele());
    return new_ptr;
}

