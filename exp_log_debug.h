#pragma once
#include<exception>
#include<string>
#include<iostream>
#include<ctime>
#include<sstream>

using std::cout;
using std::endl;
using std::string;
using std::ostringstream;

constexpr bool debug_on = true;
constexpr bool debug_off = false;
class exp_log_debug:public std::exception
{
public:
	const char* what() const { return err_info.c_str(); };
	void logout(string err, string add = "");
	void debug(bool b_d);
	void object(string obj, string beh = "");
	void behave(string beh);
	void last_behavior();
private:
	string time_text();
	string _object{"main"};
	string _behavior{"main()"};
	string _last_behavior;
	string _additional{};
	string err_info{};
	string all_info{};
	void make_all_info();
	bool debug_status{debug_on};
};
