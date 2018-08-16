#pragma once
// precompiled header
// stdafx -- standart pch name for msvc compiler
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <iostream>
#include <list>
#include <vector>
#include <functional>

using boost::asio::ip::tcp;

template<class T>
using list_ptr = std::list<std::shared_ptr<T>>;

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;

using i32 = int32_t;

#define INVALID_ID -1