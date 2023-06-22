#include "html_doc.h"

//默认从html根目录创建
std::shared_ptr<html_element> html_doc::create_dir(std::string dir, std::shared_ptr<html_element> h_ptr)
{
	_return_if(!dir.length(), nullptr);
	std::shared_ptr<html_element> res_ptr = h_ptr == nullptr ? html : h_ptr;
	_Expression::dev(dir);
	auto devs = _Expression::get_devs();
	auto_iterator<_Expression::s_ele_ptrs> eles(devs);
	move_with_func(del_blank_and_dot, eles)
	{
		delete_if(ele->e_str == " " || ele->e_str == ".")
	};
	move_with_func(c_dir, eles)
	{
		html_element_selector selector(ele->e_str, res_ptr);
		if (!selector.size()) res_ptr = res_ptr->add_element(ele->e_str);
		else res_ptr = selector[0];
	};
	eles.move_with(devs.size(), del_blank_and_dot);
	eles.to_begin();
	eles.move_with(devs.size(), c_dir);
	return res_ptr;
}

void html_doc::end_line()
{
	shift_line(html);
}

void html_doc::title(std::string title_str)
{
	html_element_selector selector("title", head);
	v_return_if(!selector.size());
	unref(selector[0]).reset_text(title_str);
}

void html_doc::line(std::string str)
{
	html->write_line(str);
}

std::string html_doc::html_str()
{
	auto doc_type = make_html_ptr("!DOCTYPE html", 0);
	doc_type->set_single();
	return get_html_str(doc_type) + get_html_str(html);
}

size_t html_doc::write_doc(std::string filepath)
{
	return write_html_doc(html_str(), filepath);
}

std::shared_ptr<html_element> html_doc::phtml()
{
	return html;
}

std::shared_ptr<html_element> html_doc::phead()
{
	return head;
}

std::shared_ptr<html_element> html_doc::pbody()
{
	return body;
}

std::string html_doc::get_html_str(std::shared_ptr<html_element> html_ptr)
{
	return html_ptr->list_all_element(html_ptr);
}

size_t html_doc::write_html_doc(std::string html_text, std::string filepath)
{
	std::ofstream outf(filepath);
	_return_if(!outf, 0);
	outf.write(html_text.c_str(), html_text.length());
	return html_text.length();
}

std::shared_ptr<html_element> html_doc::make_html_ptr(std::string name, int level)
{
	return std::make_shared<html_element>(name, level);
}

void html_doc::shift_line(std::shared_ptr<html_element> html_ptr)
{
	auto br = html_ptr->add_element("br");
	br->set_single();
}

html_doc::html_element_selector html_doc::collect(std::string name, std::shared_ptr<html_element> where)
{
	return html_element_selector(name, where);
}

html_doc::html_doc()
	:
	html(std::make_shared<html_element>(std::string("html"),0))
{
	head = html->add_element("head");
	(head->add_element("title"))->text("html_doc");
	auto meta = head->add_element("meta");
	meta->set_single();
	(*meta)["charset"] = "utf-8";
	body = html->add_element("body");
}

html_doc::html_element_selector::html_element_selector(std::string name, std::shared_ptr<html_element> htmlele)
{
	reset(name, htmlele);
}

std::shared_ptr<html_element>& html_doc::html_element_selector::operator[](int index)
{
	if ((index >= element_pos.size()) || (index < 0))
	{
		throw "html_element_selector error : index_out_of_range";
	}
	return (*m_ele.lock())[element_pos[index]];
}

void html_doc::html_element_selector::operator-(int index)
{
	if ((index >= element_pos.size()) || (index < 0))
	{
		throw "html_element_selector error : index_out_of_range";
	}
	(*m_ele.lock()) - element_pos[index];
}

bool html_doc::html_element_selector::operator&(std::shared_ptr<html_element> comparation)
{
	for (size_t i = 0; i < size(); i++)
	{
		if (comparation == (*this)[i])
		{
			return true;
		}
	}
	return false;
}

void html_doc::html_element_selector::reset(std::string name, std::shared_ptr<html_element> htmlele)
{
	if (name.front() == '"' && name.back() == '"' && name.length() != 1)
	{
		name = name.substr(1);
		name.pop_back();
	}
	element_pos.clear();
	m_name = name;
	m_ele = htmlele;
	for (int i = 0; i < htmlele->size(); i++)
	{
		if ((*htmlele)[i]->name_equal(name))
		{
			element_pos.push_back(i);
		}
	}
}

void html_doc::html_element_selector::update()
{
	reset(m_name, m_ele.lock());
}

int html_doc::html_element_selector::size()
{
	return element_pos.size();
}

int html_doc::html_element_selector::index(std::shared_ptr<html_element> element_ptr)
{
	if (!((*this)& element_ptr))
	{
		return -1;
	}
	for (size_t i = 0; i < size(); i++)
	{
		if ((*this)[i] == element_ptr)
		{
			return i;
		}
	}
	return -1;
}

int html_doc::html_element_selector::index_in_html_tree(int ele_index)
{
	return element_pos[ele_index];
}
