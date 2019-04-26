#pragma once

# include <ostream>

# include <elle/print/object.hh>

namespace std
{
  template <typename... Args>
  ostream&
  operator <<(ostream& out, vector<Args...> const& s)
  {
    out << '[';
    auto* sep = "";
    for (auto const& e: s)
    {
      elle::_details::print(out, sep);
      elle::_details::print(out, e);
      sep = ", ";
    }
    out << ']';
    return out;
  }
}
