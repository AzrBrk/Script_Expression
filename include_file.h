#pragma once
#include<concepts>
#include<memory>
#include<vector>
#include<algorithm>
#include<list>
#include<optional>
#include<string>
#include<utility>
#include<fstream>
#include<optional>
#include<cmath>
#include<map>
#include<stack>
#include<functional>
#include<coroutine>
#include<tuple>
#include<sstream>
#include<boost/describe.hpp>
#include<boost/mp11.hpp>
#include<boost/json.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/string_view.hpp>
#define _return_if(ex, y) if(ex) return y
#define v_return_if(ex) if(ex) return
#define unref(x) (*(x))
#define FALSE_STRING "false"
#define FALSE_AUTO_ITERATOR 0
#define INVOKE_CHAIN 1