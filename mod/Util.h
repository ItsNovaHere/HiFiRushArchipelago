#pragma once

#ifndef HIBIKI_UTIL_H
#define HIBIKI_UTIL_H

#include <string>


namespace Util {

	std::string WideToMultiByte(std::wstring s);

	std::wstring MultiByteToWide(std::string s);

}

#endif //HIBIKI_UTIL_H
