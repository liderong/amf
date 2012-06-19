//////////////////////////////////////////////////////////////////////
// UTF-8编码转换
// hui.wang.chn@gmail.com
//////////////////////////////////////////////////////////////////////


#ifndef CODING_UTF8_H__
#define CODING_UTF8_H__

#include <string>
#include <clocale>
#include <vector>
using namespace std;

std::ostream& operator<<(std::ostream& f, std::wstring& s);
std::istream& operator>>(std::istream& f, std::wstring& s);
void utf8_to_wstring(std::wstring& dest, const std::string& src);
void wstring_to_utf8(std::string& dest, const std::wstring& src);
std::string wstring_to_utf8(const std::wstring& str);
std::wstring utf8_to_wstring(const std::string& str);
std::string ws2s(const std::wstring& ws);
std::wstring s2ws(const std::string& s);

static std::string s2utfs(const std::string& strSrc){return wstring_to_utf8(s2ws(strSrc));}
static std::string utfs2s(const std::string& strutf){return ws2s(utf8_to_wstring(strutf)); }

void split(std::vector<string>& theStringVector,const std::string& theString,const std::string& theDelimiter);

#endif // header guard


