#include "Util.h"
#include <Windows.h>

std::string Util::WideToMultiByte(std::wstring s) {
	auto size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, NULL, NULL);
	std::string r(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &r[0], size, NULL, NULL);

	if (const auto pos = r.find('\0'); pos != std::string::npos) {
		r.erase(pos);
	}

	return r;
}

std::wstring Util::MultiByteToWide(std::string s) {
	auto size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
	std::wstring r(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &r[0], size);
		
	if (const auto pos = r.find('\0'); pos != std::string::npos) {
		r.erase(pos);
	}

	return r;
}
