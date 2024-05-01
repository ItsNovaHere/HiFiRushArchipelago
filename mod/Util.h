#ifndef HIBIKI_UTIL_H
#define HIBIKI_UTIL_H

namespace Util {

	std::string WideToMultiByte(std::wstring s) {
		auto size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, NULL, NULL);
		std::string r(size, 0);
		WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &r[0], size, NULL, NULL);

		if (const auto pos = r.find('\0'); pos != std::string::npos) {
			r.erase(pos);
		}

		return r;
	}

}

#endif //HIBIKI_UTIL_H
