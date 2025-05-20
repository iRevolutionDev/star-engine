#include "star/core/common.hpp"
#include <star/utils/string_utils.hpp>

namespace star::utils {
	std::string string_utils::to_lower(std::string_view sv) noexcept {
		std::string s(sv);
		std::ranges::transform(s, s.begin(),
		                       [](const unsigned char c) { return std::tolower(c); }
		);
		return s;
	}

	std::string string_utils::to_upper(std::string_view sv) noexcept {
		std::string s(sv);
		std::ranges::transform(s, s.begin(),
		                       [](const unsigned char c) { return std::toupper(c); }
		);
		return s;
	}

	bool string_utils::starts_with(std::string_view sv, std::string_view start) noexcept {
		return sv.find(start) == 0;
	}

	bool string_utils::ends_with(std::string_view sv, std::string_view end) noexcept {
		return sv.rfind(end) == sv.size() - end.size();
	}

	bool string_utils::contains(std::string_view sv, std::string_view part) noexcept {
		return sv.find(part) != std::string::npos;
	}

	bool string_utils::contains(std::string_view sv, std::string_view::value_type part) noexcept {
		return sv.find(part) != std::string::npos;
	}

	std::optional<int> string_utils::get_int_suffix(std::string_view name, std::string_view prefix) noexcept {
		if (!starts_with(name, prefix)) {
			return std::nullopt;
		}
		int v;
		auto r = std::from_chars(name.data() + prefix.size(), name.data() + name.size(), v);
		if (r.ptr == nullptr) {
			return std::nullopt;
		}
		return v;
	}

	std::string string_utils::bin_to_hex(void *ptr, size_t size) noexcept {
		std::stringstream ss;
		for (size_t i = 0; i < size; i++) {
			auto &v = ((uint8_t *) ptr)[i];
			ss << bin_to_hex(v);
		}
		return ss.str();
	}

	std::vector<std::string> string_utils::split_words(std::string_view sv) noexcept {
		std::vector<std::string> words;
		std::stringstream ss;
		ss << sv;
		std::string word;
		while (ss >> word) {
			words.push_back(word);
		}
		return words;
	}

	std::vector<std::string> string_utils::split(std::string_view sv, char sep) noexcept {
		std::vector<std::string> parts;
		size_t start = 0, end = 0;
		while ((end = sv.find(sep, start)) != std::string::npos) {
			parts.emplace_back(sv.substr(start, end - start));
			start = end + 1;
		}
		if (start < sv.size() - 1) {
			parts.emplace_back(sv.substr(start));
		}
		return parts;
	}

	std::vector<std::string> string_utils::split(std::string_view sv, std::string_view sep) noexcept {
		std::vector<std::string> parts;
		size_t start = 0, end = 0;
		while ((end = sv.find(sep, start)) != std::string::npos) {
			parts.emplace_back(sv.substr(start, end - start));
			start = end + sep.length();
		}
		if (start < sv.size() - sep.length()) {
			parts.emplace_back(sv.substr(start));
		}
		return parts;
	}

	std::string string_utils::do_join(std::string_view sep, const std::vector<std::string> &strs) noexcept {
		std::ostringstream ss;
		for (size_t i = 0; i < strs.size(); i++) {
			if (i > 0) {
				ss << sep;
			}
			ss << strs[i];
		}
		return ss.str();
	}

	uint8_t string_utils::hex_to_bin(char chr) {
		if (chr >= '0' && chr <= '9') {
			return chr - '0';
		} else if (chr >= 'A' && chr <= 'F') {
			return chr - 'A' + 10;
		} else if (chr >= 'a' && chr <= 'f') {
			return chr - 'a' + 10;
		}
		throw std::invalid_argument("Invalid hex character");
	}

	uint8_t string_utils::hex_to_bin(std::string_view sv) {
		if (sv.length() != 2) {
			throw std::invalid_argument("Hex string must be exactly 2 characters long");
		}

		uint8_t high = hex_to_bin(sv[0]);
		uint8_t low = hex_to_bin(sv[1]);
		return (high << 4) | low;
	}

	std::string string_utils::get_file_stem(std::string_view filename, bool lower) noexcept {
		auto filenameStr = lower ? to_lower(filename) : std::string(filename);
		auto pos = filenameStr.find('.');
		if (pos == std::string::npos) {
			return filenameStr;
		}
		return filenameStr.substr(0, pos);
	}

	std::string string_utils::get_file_ext(std::string_view filename, bool lower) noexcept {
		auto filenameStr = lower ? to_lower(filename) : std::string(filename);
		auto pos = filename.find('.');
		if (pos == std::string::npos) {
			return "";
		}
		return filenameStr.substr(pos, filename.size());
	}

	void string_utils::ltrim(std::string &str) noexcept {
		auto itr = std::find_if(str.begin(), str.end(), [](auto ch) {
			return !std::isspace(ch);
		});
		str.erase(str.begin(), itr);
	}

	void string_utils::rtrim(std::string &str) noexcept {
		auto itr = std::find_if(str.rbegin(), str.rend(), [](auto ch) {
			return !std::isspace(ch);
		});
		str.erase(itr.base(), str.end());
	}

	void string_utils::trim(std::string &str) noexcept {
		ltrim(str);
		rtrim(str);
	}

	std::string string_utils::vsprintf(const char *fmt, va_list args) {
		std::string result;
#ifdef _MSC_VER
		auto len = _vscprintf(fmt, args);
		result.resize(len);
		vsprintf_s(result.data(), len + 1, fmt, args);
#else
		auto len = vsnprintf(nullptr, 0, fmt, args);
		result.resize(len);
		vsnprintf(result.data(), len + 1, fmt, args);
#endif
		return result;
	}

	std::string string_utils::sprintf(const char *fmt, ...) {
		va_list args;
		va_start(args, fmt);
		auto str = vsprintf(fmt, args);
		va_end(args);
		return str;
	}

	std::string string_utils::get_time_suffix() noexcept {
		time_t val;
		time(&val);
		return std::to_string(val);
	}

	std::string string_utils::escape_argument(std::string_view arg) noexcept {
		std::ostringstream oss;

		bool needsQuotes = false;
		for (char c: arg) {
			if (c == ' ' || c == '\t' || c == '"' || c == '\\') {
				needsQuotes = true;
				break;
			}
		}

		if (needsQuotes) {
			oss << '"';
			for (char c: arg) {
				if (c == '"' || c == '\\') {
					oss << '\\';
				}
				oss << c;
			}
			oss << '"';
		} else {
			oss << arg;
		}

		return oss.str();
	}

	bool string_utils::contains_glob_pattern(std::string_view sv) noexcept {
		return contains(sv, '*') || contains(sv, '?');
	}

	std::string string_utils::glob_to_regex(std::string_view glob) noexcept {
		std::ostringstream regex("^");
		for (char c: glob) {
			switch (c) {
				case '*': regex << ".*";
					break;
				case '?': regex << '.';
					break;
				case '.': regex << "\\.";
					break;
				default: regex << c;
					break;
			}
		}
		regex << '$';
		return regex.str();
	}

	size_t string_utils::replace(std::string &str, std::string_view src, std::string_view dst) noexcept {
		if (src.empty()) {
			return 0;
		}
		size_t startPos = 0;
		size_t count = 0;
		while ((startPos = str.find(src, startPos)) != std::string::npos) {
			str.replace(startPos, src.length(), dst);
			startPos += dst.length();
			++count;
		}
		return count;
	}

	std::optional<std::string> string_utils::get_env(const std::string &name) noexcept {
		char *val = getenv(name.c_str());
		if (val == NULL) {
			return std::nullopt;
		}
		return std::string(val);
	}

	std::u8string string_utils::utf8_cast(const std::string &str) noexcept {
		return std::u8string(reinterpret_cast<const char8_t *>(str.data()), str.size());
	}
}
