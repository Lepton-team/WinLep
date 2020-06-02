#pragma once
#include <string>
#include <iostream>

namespace wleputils {
	class ExceptionUtil {

	public:
		template<typename EX>
		static void throwAndPrintException(std::string &msg) {
			std::cerr << createErrorMsg(msg);
			throw EX(msg.c_str());
		}

		template<typename EX>
		static void throwAndPrintException(const char *msg) {
			std::string str_msg = std::string(msg);
			printErrorMsg(msg);
			throw EX(msg);
		}

		template<typename EX>
		static void throwAndPrintException(const char *msg, const char *desc) {
			std::string str_msg = std::string(msg);
			std::string str_desc = std::string(desc);
			std::cerr << createErrorMsg(str_msg) << createErrorMsg(str_desc);
			std::string ex_msg = str_msg + '\n' + str_desc;
			throw EX(ex_msg.c_str());
		}

		template<typename EX>
		static void throwAndPrintException(const char *msg, const char *desc, unsigned long error_code) {
			std::string str_desc = std::string(desc);
			str_desc.append(" (")
				.append(std::to_string(error_code))
				.append(")");
			ExceptionUtil::throwAndPrintException<EX>(msg, str_desc.c_str());
		}

		template<typename EX>
		static void throwAndPrintException(const char *msg, unsigned long error_code) {
			std::string str_desc = std::string(msg);
			str_desc.append(" (")
				.append(std::to_string(error_code))
				.append(")");
			ExceptionUtil::throwAndPrintException<EX>(msg);
		}

		template<typename EX>
		static void throwAndPrintException(std::string &msg, std::string &desc) {
			std::cerr << createErrorMsg(msg) << createErrorMsg(desc);
			std::string ex_msg = msg + '\n' + desc;

			throw EX(ex_msg.c_str());
		}

		static inline std::string createErrorMsg(const char *msg, unsigned long error_code) {
			std::string str = std::string(msg);
			return "[ERROR]" + str + " (" + std::to_string(error_code) + ")\n";
		}

		static inline void printErrorMsg(const char *msg, unsigned long error_code) {
			std::cerr << createErrorMsg(msg, error_code);
		}

		static inline std::string createErrorMsg(std::string &msg) {
			return "[ERROR] " + msg + '\n';
		}

		static inline void printErrorMsg(std::string &msg) {
			std::cerr << createErrorMsg(msg);
		}

		static inline void printErrorMsg(const char *msg) {
			std::string str_msg = std::string(msg);
			std::cerr << createErrorMsg(str_msg);
		}
	};
}