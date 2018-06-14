/*
        ##########    Copyright (C) 2017 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

// -------------------------------------------------------------------------------------------------

namespace sl
{
namespace rastr
{
namespace util
{

// -------------------------------------------------------------------------------------------------

template <typename T>
void swap(T& t1, T& t2)
{
  T temp(t1);
  t1 = t2;
  t2 = temp;
}

// -------------------------------------------------------------------------------------------------

template <typename T>
inline T max(T a, T b)
{
  return a > b ? a : b;
}

// -------------------------------------------------------------------------------------------------

template <typename A, typename... Args>
A max(A a, A b, Args... args)
{
  return max(max(a, b), args...);
}

//--------------------------------------------------------------------------------------------------

template <typename T>
inline T min(T a, T b)
{
  return a < b ? a : b;
}

// -------------------------------------------------------------------------------------------------

template <typename A, typename... Args>
A min(A a, A b, Args... args)
{
  return min(min(a, b), args...);
}

// -------------------------------------------------------------------------------------------------

} // namespace util
} // namespace rastr
} // namespace sl
