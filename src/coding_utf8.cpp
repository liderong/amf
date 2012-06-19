#include "coding_utf8.h"

void utf8_to_wstring( std::wstring& dest, const std::string& src){
	dest.clear();
	wchar_t w = 0;
	int bytes = 0;
	wchar_t err = L' ';
	for (size_t i = 0; i < src.size(); i++){
		unsigned char c = (unsigned char)src[i];
		if (c <= 0x7f){//first byte
			if (bytes){
				dest.push_back(err);
				bytes = 0;
			}
			dest.push_back((wchar_t)c);
		}
		else if (c <= 0xbf){//second/third/etc byte
			if (bytes){
				w = ((w << 6)|(c & 0x3f));
				bytes--;
				if (bytes == 0)
					dest.push_back(w);
			}
			else
				dest.push_back(err);
		}
		else if (c <= 0xdf){//2byte sequence start
			bytes = 1;
			w = c & 0x1f;
		}
		else if (c <= 0xef){//3byte sequence start
			bytes = 2;
			w = c & 0x0f;
		}
		else if (c <= 0xf7){//3byte sequence start
			bytes = 3;
			w = c & 0x07;
		}
		else{
			dest.push_back(err);
			bytes = 0;
		}
	}
	if (bytes)
		dest.push_back(err);
}

void wstring_to_utf8(std::string& dest,  const std::wstring& src){
	dest.clear();
	for (size_t i = 0; i < src.size(); i++){
		wchar_t w = src[i];
		if (w <= 0x7f)
			dest.push_back((char)w);
		else if (w <= 0x7ff){
			dest.push_back(0xc0 | ((w >> 6)& 0x1f));
			dest.push_back(0x80| (w & 0x3f));
		}
		else if (w <= 0xffff){
			dest.push_back(0xe0 | ((w >> 12)& 0x0f));
			dest.push_back(0x80| ((w >> 6) & 0x3f));
			dest.push_back(0x80| (w & 0x3f));
		}
		else if (w <= 0x10ffff){
			dest.push_back(0xf0 | ((w >> 18)& 0x07));
			dest.push_back(0x80| ((w >> 12) & 0x3f));
			dest.push_back(0x80| ((w >> 6) & 0x3f));
			dest.push_back(0x80| (w & 0x3f));
		}
		else
			dest.push_back('?');
	}
}

std::string wstring_to_utf8(const std::wstring& str){
	std::string result;
	wstring_to_utf8(result, str);
	return result;
}

std::wstring utf8_to_wstring(const std::string& str){
	std::wstring result;
	utf8_to_wstring(result, str);
	return result;
}

std::ostream& operator<<(std::ostream& f,std::wstring& s){
	std::string s1;
	wstring_to_utf8(s1, s);
	f << s1;
	return f;
}

std::istream& operator>>(std::istream& f, std::wstring& s){
	std::string s1;
	f >> s1;
	utf8_to_wstring(s, s1);
	return f;
}

std::string ws2s(const std::wstring& ws)  
{  
	std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";   
	setlocale(LC_ALL, "chs");  
	const wchar_t* _Source = ws.c_str();  
	size_t _Dsize = 2 * ws.size() + 1;  
	char *_Dest = new char[_Dsize];  
	memset(_Dest, 0, _Dsize);  
	wcstombs(_Dest, _Source, _Dsize);  
	std::string result = _Dest;  
	delete [] _Dest;  
	setlocale(LC_ALL, curLocale.c_str());  
	return result;  
}  

std::wstring s2ws(const std::string& s)   
{  
	setlocale(LC_ALL, "chs");  
	const char* _Source = s.c_str();  
	size_t _Dsize = s.size() + 1;  
	wchar_t* _Dest = new wchar_t[_Dsize];  
	wmemset(_Dest, 0, _Dsize);  
	int nret = mbstowcs(_Dest, _Source, _Dsize);  
	std::wstring result = _Dest;  
	delete [] _Dest;  
	setlocale(LC_ALL, "C");  

	return result;  
}  

void split( std::vector<string>& theStringVector,const std::string& theString,const std::string& theDelimiter )
{
	size_t  start = 0, end = 0;
	while ( end != string::npos)
	{
		end = theString.find( theDelimiter, start);
		theStringVector.push_back( theString.substr( start,
			(end == string::npos) ? string::npos : end - start));
		start = (( end >(string::npos - theDelimiter.size()) )
			? string::npos:end + theDelimiter.size());
	}
}
