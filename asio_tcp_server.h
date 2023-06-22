#pragma once
#include<boost/asio/ip/tcp.hpp>
#include<boost/asio.hpp>
#include<iostream>
#include<string>
#include<sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;
using namespace boost;

using address_type = asio::ip::address;
using socket_type = asio::ip::tcp::socket;
using acceptor_type = asio::ip::tcp::acceptor;
using endpoint_type = asio::ip::tcp::endpoint;

class my_asio_server
{
public:
	my_asio_server(short port)
		:
		m_sock(m_io),
		port(port)
	{}
	void accept_new_sock()
	{
		acceptor_type acceptor(m_io, endpoint_type(asio::ip::tcp::v4(), port));
		acceptor.accept(m_sock);

	}
	void close_sock()
	{
		m_sock.close();
	}
	string server_receive()
	{
		ostringstream oss;
		vector<char> str_vec(m_sock.available() + 1);
		m_sock.receive(asio::buffer(str_vec));
		oss << &str_vec[0];
		return oss.str();
	}
	int send_by_server(string str)
	{
		auto bf = asio::buffer(str);
		return m_sock.send(asio::buffer(str));
	}
private:
	unsigned short port;
	boost::asio::io_service m_io;
	socket_type m_sock;
};

void string_replacer(string& str, string ori, string rep)
{
	int pos = -1;
	while ((pos = str.find(ori)) != string::npos)
	{
		str.replace(pos, ori.length(), rep);
	}
}

string post_request_context_reader(string post_req_line)
{
	int pos = post_req_line.find("\r\n\r\n");
	if (pos != string::npos)
	{
		return post_req_line.substr(pos + 4);
	}
	else
		return "";
}

//该函数将字符串html_str中的url_code转化成ascii码
void html_doc_processor(string& html_str)
{
	int len = html_str.length();

	for (int i = 0; i < len; i++) {
		if (html_str[i] == '%' && i + 2 < len) {  //找到url_code
			char c1 = html_str[i + 1];
			char c2 = html_str[i + 2];
			if (isxdigit(c1) && isxdigit(c2)) {  //确保c1和c2为十六进制字符
				int ascii_code = stoi(string(1, c1) + string(1, c2), nullptr, 16);  //转换成ascii码
				html_str.replace(i, 3, string(1, (char)ascii_code));  //替换原字符串中的url_code
				len -= 2;  //修改长度，因为url_code被替换成了一个字符
			}
		}
	}
}

vector<string> post_request_divider(string& req_context)
{
	vector<string> res;
	int pos = -1;
	while ((pos=req_context.find('&'))!=string::npos)
	{
		res.push_back(req_context.substr(0, pos));
		req_context = req_context.substr(pos + 1);
	}
	return res;
}