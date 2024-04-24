//
// Created by Tyler on 2024-04-23.
//

#include "log.h"

void Log::Info(const char* fmt, ...) {
	std::va_list args1;
	va_start(args1, fmt);
	std::va_list args2;
	va_copy(args2, args1);
	std::vector<char> buf(1 + std::vsnprintf(nullptr, 0, fmt, args1));
	va_end(args1);
	std::vsnprintf(buf.data(), buf.size(), fmt, args2);

	std::printf("[HIBIKI] %s\n", buf.data());
}