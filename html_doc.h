#pragma once
#include"htmlElement.h"
#include"include_file.h"
#include"_Expression.h"
#include"auto_iterator.h"
#include"if_container.h"

class html_doc
{
public:
	class html_element_selector {
	public:
		explicit html_element_selector(std::string name, std::shared_ptr<html_element> htmlele);
		std::shared_ptr<html_element>& operator[](int index);
		void operator-(int index);
		bool operator&(std::shared_ptr<html_element> comparation);
		void reset(std::string name, std::shared_ptr<html_element> htmlele);
		void update();
		int size();
		int index(std::shared_ptr<html_element> element_ptr);
		int index_in_html_tree(int ele_index);
		
	private:
		std::string m_name;
		std::vector<int> element_pos;
		std::weak_ptr<html_element> m_ele;
	};
	std::shared_ptr<html_element> create_dir(std::string dir, std::shared_ptr<html_element> h_ptr = nullptr);
	void end_line();
	void title(std::string title_str);
	void line(std::string str);
	std::string html_str();
	size_t write_doc(std:: string filepath);
	std::shared_ptr<html_element> phtml();
	std::shared_ptr<html_element> phead();
	std::shared_ptr<html_element> pbody();
	static std::string get_html_str(std::shared_ptr<html_element> html_ptr);
	static size_t write_html_doc(std::string html_text, std::string filepath);
	static std::shared_ptr<html_element> make_html_ptr(std::string name, int level);
	static void shift_line(std::shared_ptr<html_element> html_ptr);

	html_element_selector collect(std::string name, std::shared_ptr<html_element> where);
public:
	html_doc();
	html_doc(const html_doc&) = delete;
	html_doc& operator=(const html_doc&) = delete;
private:
	std::shared_ptr<html_element> html, head, body;
};

