#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<sstream>
#include<map>
#include<unordered_map>
#include<memory>
#include<iostream>




class h_attrib
{
public:
	h_attrib(std::string name, std::string value);
	h_attrib();
	int size();
	
	std::unordered_map<std::string, std::string>::iterator get_attribs_iter();
	std::unordered_map<std::string, std::string>::iterator get_attribs_end_iter();

	std::string& operator[](std::string);
	std::string operator-(std::string);
	bool find_attrib(std::string attrib_name);

	virtual std::string attribs_list();
protected:
	int attrib_cnt;
	std::unordered_map<std::string, std::string> attribs;
};

class css_element : public h_attrib
{
public:
	
	std::vector<std::string> css_line();
	virtual std::string attribs_list()override;
private:
	std::map<std::string, std::string>::iterator attr_iter;
};
class html_element
{
private:
	std::vector<std::shared_ptr<html_element>> elements;
	h_attrib attribs;
	css_element c_attribs;

	//ptrs
	std::weak_ptr<html_element> self_ptr;
	std::weak_ptr<html_element> father_ptr;

	std::string m_name;
	std::string push;
	std::string inner_text;

	
	bool m_single = false;
	int m_level;
	bool is_css_element = false;
	bool m_invisible = false;

	//temporatory date
	unsigned int this_index = 0;
	
public:
	void set_this_index(unsigned int index);
	unsigned int get_this_index();
	explicit html_element(std::string name, int level);
	~html_element();
	std::shared_ptr<html_element>& add_element(std::string name);
	std::string list_all_element(std::weak_ptr<html_element> ele);


	std::string& operator[](std::string name);
	std::shared_ptr<html_element>& operator[](int index);
	std::string operator-(std::string name);
	void operator-(int index);
	std::unordered_map<std::string, std::string>::iterator get_attribs_iter();
	std::unordered_map<std::string, std::string>::iterator get_attribs_end_iter();
	void set_father(std::shared_ptr<html_element> father);
	std::weak_ptr<html_element> father();

	std::string begin();
	std::string close();
	std::string str();
	std::string get_inner_text();
	//std::string final_str();


	int level();
	int size();

	void text(std::string text);
	void reset_text(std::string text);
	void write_line(std::string text);
	std::shared_ptr<html_element> share_self();

	void set_push(int level);
	void set_single();
	void set_to_css();
	void store_self(std::shared_ptr<html_element> self);
	

	bool name_equal(std::string name);
	bool is_single();
	bool is_a_css_element();
	bool has_attrib(std::string attrib_name);

	std::string name();
};