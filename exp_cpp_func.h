#pragma once
#include"include_file.h"
#include"exp_ele.h"
#include"script_command.h"
#include "exp_log_debug.h"
//element linked to a cpp function
//use exp_ele::matched to bind to exp_cpp_func object
//provide source language with an interface to C++ function, use exp_invoke to import arguments

struct co_func
{
    struct promise_type
    {
        s_ele_ptr _out;
        co_func get_return_object()
        {
			return co_func(std::coroutine_handle<promise_type>::from_promise(*this));
		}
		std::suspend_never initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void return_void() {}
		void unhandled_exception() { std::terminate(); }
	};

    std::coroutine_handle<promise_type> _h;
};

struct co_input
{
    s_ele_ptr e;
	bool await_ready() { return false; }
	void await_suspend(std::coroutine_handle<co_func::promise_type> h) {
        e = h.promise()._out;
    }
	s_ele_ptr await_resume() { return e; }
};

struct exp_executable : exp_ele
{
    template<typename func_t, typename ...Args>
    co_func exp_exec(func_t f, Args ...args)
    {
        auto e_func = std::bind(f, (std::forward<Args>(args))...);
        s_ele_ptr e = co_await co_input{};
        matched = e_func();
    }

    template<typename func_t, typename ...Args>
    void exp_bind(func_t f, Args ...args)
    {
        auto ret = exp_exec(f, (std::forward<Args>(args))...);
        _h_func = ret._h;
    }
    std::coroutine_handle<co_func::promise_type> _h_func;//
};

//element linked to a cpp function
//use exp_ele::matched to bind to exp_cpp_func object
//provide source language with an interface to C++ function, use exp_invoke to import arguments
class exp_cpp_func :
    public exp_ele
{
public:
    exp_cpp_func();
    exp_cpp_func(s_ele_ptr matched_e);//for invoke_ele

    void reflexed_by(string name);//reflexed by a name;

    void reflex_redirect(s_ele_ptr _from, s_ele_ptr _to);//
   
    //standard command handling function, if an exp_entrance is provide, function can be overloaded
    using exp_func_type = std::function<s_ele_ptr(s_ele_ptr, s_ele_ptr, s_ele_ptr)>;
   
    //customized cpp function, this type of function cannot be overloaded
    using exp_execute_func_type = std::function<s_ele_ptr()>;

    //immediately execute the 0 function, regardless of overload functions
    s_ele_ptr operator()(s_ele_ptr _1_, s_ele_ptr _2_ = nullptr, s_ele_ptr _3_ = nullptr)
    {
       overloaded_funcs.push_back( std::bind(func, _1_, _2_, _3_));
       auto ret =  overloaded_funcs[0]();
       return ret;
    }
    s_ele_ptr operator()(s_sc_ptr cmd)
    {
        return this->operator()(cmd->e1, cmd->e2, cmd->e3);
    }
    void exp_bind(exp_func_type _func);
    void exp_bind_ele(s_ele_ptr e_func);

    template<typename func_t, typename ...Args>
    co_func exp_exec(func_t f, Args ...args)
    {
		auto e_func = std::bind(f, (std::forward<Args>(args))...);
        s_ele_ptr e = co_await co_input{};
        matched = e_func();
    }

    template<typename func_t, typename ...Args>
    void exp_bind(func_t f, Args ...args)
    {
    	auto ret = exp_exec(f, (std::forward<Args>(args))...);
        _h_func = ret._h;
	}
    
    void exp_execute()
    {
       _h_func.resume();
    }
    s_ele_ptr get_func()
    {
		return ele_func;
	}
    template<typename ...Ts>
    void exp_bind_tuple(std::tuple<Ts...> t)
    {
		std::apply([&](auto&&... args) { exp_bind(args...); }, t);
	}
private:
    exp_func_type func;
    exp_execute_func_type exec_func;
    s_ele_ptr ele_func;
    std::vector<exp_execute_func_type> overloaded_funcs;//overload list
    exp_log_debug _log;
    std::coroutine_handle<co_func::promise_type> _h_func;//
};

typedef std::shared_ptr<exp_cpp_func> s_cpp_func_ptr;
s_cpp_func_ptr make_cpp_func(s_ele_ptr = nullptr);