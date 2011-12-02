//
//  StringUtil.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/StringUtil.h"

namespace ukn {
    
    StringTokenlizer::StringTokenlizer() {
        
    }
    
    StringTokenlizer::StringTokenlizer(const ukn_string& str) {
        parse(str, ukn_string());
    }
    
    StringTokenlizer::StringTokenlizer(const ukn_string& str, const ukn_string& deli) {
        parse(str, deli);
    }
    
    ukn_string StringTokenlizer::first() {
        if(mTokens.size() != 0)
            return mTokens.front();
        return ukn_string();
    }
    
    ukn_string StringTokenlizer::last() {
        if(mTokens.size() != 0)
            return mTokens.back();
        return ukn_string();
    }
    
    ukn_string StringTokenlizer::front() {
        if(mTokens.size() != 0)
            return mTokens.front();
        return ukn_string();
    }
    
    ukn_string StringTokenlizer::back() {
        if(mTokens.size() != 0)
            return mTokens.back();
        return ukn_string();
        
    }
    
    StringTokenlizer::iterator StringTokenlizer::begin() {
        return mTokens.begin();
    }
    
    StringTokenlizer::iterator StringTokenlizer::end() {
        return mTokens.end();
    }
    
    const StringTokenlizer::TokenContainer& StringTokenlizer::getTokens() const {
        return mTokens;
    }
    
    bool StringTokenlizer::isSpace(char c) {
        return (c == '\n' || c == ' ' || c == 'r' || c == '\t');
    }
    
    bool StringTokenlizer::isAlpha(char c) {
        return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
    }
    
    void StringTokenlizer::parse(const ukn_string& str) {
        parse(str, ukn_string());
    }
    
    ukn_string StringTokenlizer::operator[](size_t index) {
        ukn_assert(index < mTokens.size());
        
        return mTokens[index];
    }
    
    void StringTokenlizer::parse(const ukn_string& str, const ukn_string& deli) {
        size_t start = 0, end = 0;
        if(deli.size() != 0) {
            while(end < str.size()) {
                while((deli.find(str[end]) == ukn_string::npos) && end < str.size()) { ++end; }
                if(end == str.size()) {
                    while(deli.find(str[end]) != ukn_string::npos)
                        --end;
                    mTokens.push_back( str.substr(start, end-start) );
                    break;
                }
                if(end != start)
                    mTokens.push_back( str.substr(start, end-start) );
                
                ++end; 
                while((deli.find(str[end]) != ukn_string::npos || isSpace(str[end])) && end < str.size()) ++end;
                
                start = end;
            }
        } else {
            while(end < str.size()) {
                while(isAlpha(str[end]) && end < str.size()) { ++end; }
                if(end == str.size()) {
                    mTokens.push_back( str.substr(start) );
                    break;
                }
                mTokens.push_back( str.substr(start, end-start) );
                
                ++end; 
                while(!isAlpha(str[end]) && end < str.size()) {
                    if(str[end] == '"') {
                        ++end;
                        start = end;
                        while(str[end] != '"' && end < str.size())
                            ++end;
                        mTokens.push_back( str.substr(start, end-start) );
                    }
                    ++end;
                    
                }
                start = end;
            }
        }
    }
    
    void StringTokenlizer::operator=(const ukn_string& str) {
        parse(str, ukn_string());
    }
    
    void StringTokenlizer::operator=(const char* str) {
        parse(str, ukn_string());
    }
    
    size_t StringTokenlizer::size() const {
        return mTokens.size();
    }
    
} // namespace ukn

#ifdef UKN_OS_WINDOWS
#include <windows.h>
#elif defined(UKN_OS_IOS) || defined(UKN_OS_OSX)
#include "AppleStuff.h"
#endif

#include <cstring>
#include <locale.h>

namespace ukn {
    
#ifdef UKN_OS_WINDOWS
	static ukn_string ukn_win_wstring_to_string(const ukn_wstring& pwszSrc) {
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc.c_str(), -1, NULL, 0, NULL, NULL);
		if (nLen<= 0) return std::string("");
		char* pszDst = new char[nLen];
		if (NULL == pszDst) return std::string("");
		WideCharToMultiByte(CP_ACP, 0, pwszSrc.c_str(), -1, pszDst, nLen, NULL, NULL);
		pszDst[nLen -1] = 0;
		std::string strTemp(pszDst);
		delete [] pszDst;
		return strTemp;
	}
    
	static ukn_wstring ukn_win_string_to_wstring(const ukn_string& pszSrc) {
		if(pszSrc.size() == 0)
			return L"\0";
		int nSize = MultiByteToWideChar(CP_ACP, 0, pszSrc.c_str(), pszSrc.size(), 0, 0);
		if(nSize <= 0) return NULL;
		WCHAR *pwszDst = new WCHAR[nSize+1];
		if( NULL == pwszDst) return NULL;
		MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc.c_str(), pszSrc.size(), pwszDst, nSize);
		pwszDst[nSize] = 0;
		if( pwszDst[0] == 0xFEFF)                    // skip Oxfeff
			for(int i = 0; i < nSize; i ++)
				pwszDst[i] = pwszDst[i+1];
		std::wstring wcharString(pwszDst);
		delete pwszDst;
		return wcharString;
	}
#endif
    
    static ukn_string ukn_normal_wstring_to_string(const ukn_wstring& ws) {
        std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
        setlocale(LC_ALL, "chs");
        const wchar_t* _Source = ws.c_str();
        size_t _Dsize = 2 * ws.size() + 1;
        char *_Dest = new char[_Dsize];
        memset(_Dest,0,_Dsize);
        wcstombs(_Dest,_Source,_Dsize);
        std::string result = _Dest;
        delete []_Dest;
        setlocale(LC_ALL, curLocale.c_str());
        return result;
    }
    
    static ukn_wstring ukn_normal_string_to_wstring(const ukn_string& s) {
        setlocale(LC_ALL, "chs"); 
        const char* _Source = s.c_str();
        size_t _Dsize = s.size() + 1;
        wchar_t *_Dest = new wchar_t[_Dsize];
        wmemset(_Dest, 0, _Dsize);
        mbstowcs(_Dest,_Source,_Dsize);
        std::wstring result = _Dest;
        delete []_Dest;
        setlocale(LC_ALL, "C");
        return result;
    }
    
    
    UKN_API ukn_string wstring_to_string(const ukn_wstring& str) {
#if defined(UKN_OS_WINDOWS)
        return ukn_win_wstring_to_string(str);
#elif defined(UKN_OS_FAMILY_APPLE)
        return ukn_apple_wstring_to_string(str);
#else
        return ukn_normal_wstring_to_string(str);
#endif
    }
    
    UKN_API ukn_wstring string_to_wstring(const ukn_string& str) {
#if defined(UKN_OS_WINDOWS)
        return ukn_win_string_to_wstring(str);
#elif defined(UKN_OS_FAMILY_APPLE)
        return ukn_apple_string_to_wstring(str);
#else
        return ukn_normal_string_to_wstring(str);
#endif  
    }
    
    UKN_API ukn_string wstring_to_string_fast(const ukn_wstring& str) {
        ukn_string buffer(str.length(), ' ');
		std::copy(str.begin(), str.end(), buffer.begin());
		return buffer;
    }
    
    UKN_API ukn_wstring string_to_wstring_fast(const ukn_string& str) {
        ukn_wstring buffer(str.length(), L' ');
		std::copy(str.begin(), str.end(), buffer.begin());
		return buffer; 
    }
    
    UKN_API ukn_string get_file_name(const ukn_string& str) {
        ukn_string::const_iterator it = str.end();
        it--;
        
        while(it != str.begin() && *it != '/' && *it != '\\') {
            --it;
        }
        return ukn_string(it, str.end());
    }
    
    UKN_API ukn_wstring get_file_name(const ukn_wstring& str) {
        ukn_wstring::const_iterator it = str.end();
        it--;
        
        while(it != str.begin() && *it != L'/' && *it != L'\\') {
            --it;
        }
        return ukn_wstring(it, str.end());
    }
    
    
} // namespace ukn
