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
    // �ú������ڴ��ַ��� str ��ȡ������ f �����������ַ������µ��ַ��������ء�
    // ���ָ���� take_num ������ֻ��ȡ�� take_num �������������ַ������ take_num С�ڵ��� 0�����ȡ�����з����������ַ���
    // str ����ΪĿ���ַ�����f ����Ϊ�������ͣ������Ϊ char ���ͣ�����ֵ����Ϊ bool ���͡�
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

    //��Ԫ��������ת��eleԪ������
    template<ELE_BASE et>
    static std::shared_ptr<et> ele_type_convert(auto_iterator<s_ele_ptrs>& _it);

    
private:
    _Expression() = default;
};

template<typename func_type>
inline std::string _Expression::TAKE_WHEN(std::string& str, func_type f, int take_num)
{
    bool b_num_cnt = false; // b_num_cnt �����ж��Ƿ���Ҫ����ȡ�����ַ�����
    string res; // res ���ڼ�¼�����������ַ���ɵ��ַ���
    if (take_num >= 0)
        b_num_cnt = true;
    while (f(str.at(0))) // ѭ��ȡ�������������ַ�ֱ����һ���������������ַ�
    {
        if (b_num_cnt && --take_num < 0) // �����Ҫ����ȡ�������������Ѿ�ȡ���� take_num ���ַ������˳�ѭ��
            break;
        res += str.at(0); // �������������ַ���ӵ�����ַ�����
        str.erase(0, 1); // ɾ���Ѿ�ȡ�����ַ�
        if (str.empty()) // ����Ѿ�ȡ���������ַ������˳�ѭ��
            break;
    }
    return res; // ����ȡ�����ַ����ɵ��ַ���
}
template<ELE_BASE et>
inline std::shared_ptr<et> _Expression::ele_type_convert(auto_iterator<s_ele_ptrs>& _it)
{
    auto new_ptr = std::make_shared<et>(unref(_it.data()));
    _it.erase();
    _it.insert(new_ptr->cast_to_ele());
    return new_ptr;
}

