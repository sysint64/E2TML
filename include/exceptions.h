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

	class DoesNotExist : public std::exception {
	private:
		std::string details;

	public:
		DoesNotExist(const std::string &key, const std::string &field) {
			std::stringstream stream;
			stream << field << " \"" << key << "\" does not exist";
			this->details = stream.str();
		}

		virtual const char *what() const noexcept {
			return details.c_str();
		}
	};

	class NodeDoesNotExist : public DoesNotExist {
	public:
		NodeDoesNotExist(const std::string &key) : DoesNotExist(key, "Node") {}
	};

	class ParameterDoesNotExist : public DoesNotExist {
	public:
		ParameterDoesNotExist(const std::string &key) : DoesNotExist(key, "Parameter") {}
	};

	class ValueDoesNotExist : public DoesNotExist {
	public:
		ValueDoesNotExist(const std::string &key) : DoesNotExist(key, "Value") {}
	};

	class BadValueType : public std::exception {
	private:
		std::string details;

	public:
		BadValueType(const std::string &targetType, const std::string &realType) {
			this->details = "Type of value not \"" + targetType + "\". Value is \""+realType+"\"";
		}

		virtual const char *what() const noexcept {
			return details.c_str();
		}
	};
}
