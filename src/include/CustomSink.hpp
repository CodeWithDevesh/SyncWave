#pragma once
#include <iostream>
#include <g3log/logmessage.hpp>
#include <string>


struct CustomSink {
	enum FG_Color { YELLOW = 33, RED = 31, GREEN = 32, WHITE = 97 };

	FG_Color GetColor(const LEVELS level) const {
		if (level.value == WARNING.value) { return YELLOW; }
		if (level.value == DEBUG.value) { return GREEN; }
		if (g3::internal::wasFatal(level)) { return RED; }

		return WHITE;
	}


public:


	void ReceiveLogMessage(g3::LogMessageMover logEntry) {
		auto level = logEntry.get()._level;
		auto color = GetColor(level);

		g3::LogMessage msg = logEntry.get();

#ifdef DEBUG
		if(level.value == FATAL.value) {
			std::cout << "\033[" << color << "m"
				 << msg.message() << "\033[m" << std::endl;
		}
		return;
#else
		if(level.value == WARNING.value + 1) { 
			std::cout << "\033[" << color << "m"
				<< msg.message() << "\033[m" << std::endl;
			std::terminate();

		}
#endif // DEBUG

		std::cout << "\033[" << color << "m"
			 << msg.message() << "\033[m" << std::endl;
	}
};