#pragma once

#include <exception>
#include <string>
#include <sstream>


namespace e2ml {
	class LexerError : public std::exception {
	private:
		std::string details;

	public:
		LexerError(const std::string &details) {
			this->details = details;
		}

		virtual const char *what() const noexcept {
			return details.c_str();
		}
	};

	class ParseError : public std::exception {
	private:
		std::string details;

	public:
		ParseError(const std::string &details, const int line, const int pos) {
			std::stringstream stream;
			stream << "line " << line << ", pos " << pos << ": " << details;
			this->details = stream.str();
		}

		virtual const char *what() const noexcept {
			return details.c_str();
		}
	};
}
