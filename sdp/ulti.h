#include <string>
#include <regex>
#include <iostream>


std::vector<std::string> StrSplit(const std::string& str, const std::string& s) {

  std::vector<std::string> ret;

  std::regex pat(s);
  std::sregex_token_iterator iter(str.begin(),
    str.end(),
    pat,
    -1);
  std::sregex_token_iterator end;
  for (; iter != end; ++iter)
    ret.push_back(*iter);
  //std::cout << *iter << '\n';
  return ret;
}