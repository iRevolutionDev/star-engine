#pragma once

#include <star/export.hpp>

namespace star::utils {
    struct STAR_EXPORT string_utils final {
        [[nodiscard]] static std::string to_lower(std::string_view sv) noexcept;

        [[nodiscard]] static std::string to_upper(std::string_view sv) noexcept;

        [[nodiscard]] static bool starts_with(std::string_view sv, std::string_view start) noexcept;

        [[nodiscard]] static bool ends_with(std::string_view sv, std::string_view end) noexcept;

        [[nodiscard]] static bool contains(std::string_view sv, std::string_view part) noexcept;

        [[nodiscard]] static bool contains(std::string_view sv, std::string_view::value_type part) noexcept;

        [[nodiscard]] static std::optional<int> get_int_suffix(std::string_view name, std::string_view prefix) noexcept;

        static std::string bin_to_hex(void *ptr, size_t size) noexcept;

        template<typename T>
        static std::string bin_to_hex(T &&v) noexcept {
            return string_utils::sprintf("%02X", v);
        }

        template<const void *>
        static std::string bin_to_hex(const void *v) noexcept {
            return string_utils::sprintf("%p", v);
        }

        [[nodiscard]] static std::vector<std::string> split_words(std::string_view sv) noexcept;

        [[nodiscard]] static std::vector<std::string> split(std::string_view sv, char sep) noexcept;

        [[nodiscard]] static std::vector<std::string> split(std::string_view sv, std::string_view sep) noexcept;

        template<typename Iter, typename C>
        [[nodiscard]] static std::string join(std::string_view sep, Iter begin, Iter end, C callback) noexcept {
            std::vector<std::string> strs;
            for (auto itr = begin; itr != end; ++itr) {
                strs.emplace_back(callback(*itr));
            }
            return do_join(sep, strs);
        }

        template<typename T, typename C>
        [[nodiscard]] static std::string join(std::string_view sep, const std::vector<T> &elements,
                                              C callback) noexcept {
            return join(sep, elements.begin(), elements.end(), callback);
        }

        template<typename T>
        [[nodiscard]] static std::string join(std::string_view sep, const std::vector<T> &elements) noexcept {
            return join(sep, elements.begin(), elements.end());
        }

        template<typename Iter>
        [[nodiscard]] static std::string join(std::string_view sep, Iter begin, Iter end) noexcept {
            return join_impl<Iter, typename std::iterator_traits<Iter>::value_type>::run(sep, begin, end);
        }

        [[nodiscard]] static uint8_t hex_to_bin(char chr);

        [[nodiscard]] static uint8_t hex_to_bin(std::string_view sv);

        [[nodiscard]] static std::string get_file_stem(std::string_view filename, bool lower = true) noexcept;

        [[nodiscard]] static std::string get_file_ext(std::string_view filename, bool lower = true) noexcept;

        [[nodiscard]] static std::string escape_argument(std::string_view arg) noexcept;

        [[nodiscard]] static bool contains_glob_pattern(std::string_view glob) noexcept;

        [[nodiscard]] static std::string glob_to_regex(std::string_view glob) noexcept;

        static size_t replace(std::string &str, std::string_view src, std::string_view dst) noexcept;

        [[nodiscard]] static std::optional<std::string> get_env(const std::string &name) noexcept;

        [[nodiscard]] static std::u8string utf8_cast(const std::string &str) noexcept;

        static void ltrim(std::string &str) noexcept;

        static void rtrim(std::string &str) noexcept;

        static void trim(std::string &str) noexcept;

        static std::string vsprintf(const char *fmt, va_list args);

        static std::string sprintf(const char *fmt, ...);

        static std::string get_time_suffix() noexcept;

        template<typename Callback>
        static bool regex_replace(std::string &str, const std::regex &pattern, Callback &&callback) {
            auto strItr = str.cbegin();
            auto changed = false;
            std::smatch match;
            while (std::regex_search(strItr, str.cend(), match, pattern)) {
                std::string repl;
                if (callback(match, repl)) {
                    auto pos = match.position(0);
                    str.replace(pos, match.length(0), repl);
                    strItr = str.cbegin() + pos + repl.length();
                    changed = true;
                }
            }
            return changed;
        }

        template<size_t S>
        static const std::string &get_enum_name(size_t idx, const std::array<std::string, S> &names) noexcept {
            if (idx >= names.size()) {
                static const std::string empty;
                return empty;
            }
            return names[idx];
        }

        template<typename T, size_t S>
        static std::optional<T> read_enum(std::string_view name, const std::array<std::string, S> &names,
                                          const std::string prefix = "") noexcept {
            auto lower_name = string_utils::to_lower(name);
            auto lower_prefix = string_utils::to_lower(prefix);
            if (!lower_prefix.empty() && string_utils::starts_with(lower_name, lower_prefix)) {
                lower_name = lower_name.substr(lower_prefix.size());
            }
            for (size_t i = 0; i < names.size(); ++i) {
                auto key_name = string_utils::to_lower(names[i]);
                if (key_name == lower_name) {
                    return static_cast<T>(i);
                }
            }
            return std::nullopt;
        }

    private:
        [[nodiscard]] static std::string do_join(std::string_view sep, const std::vector<std::string> &strs) noexcept;

        template<typename Iter, typename V>
        struct join_impl final {
            static std::string run(std::string_view sep, Iter begin, Iter end) {
                return join(sep, begin, end, [](auto &elm) { return std::to_string(elm); });
            }
        };

        template<typename Iter>
        struct join_impl<Iter, std::string> final {
            static std::string run(std::string_view sep, Iter begin, Iter end) {
                return join(sep, begin, end, [](auto &elm) { return elm; });
            }
        };

        template<typename Iter>
        struct join_impl<Iter, std::string_view> final {
            static std::string run(std::string_view sep, Iter begin, Iter end) {
                return join(sep, begin, end, [](auto &elm) { return elm; });
            }
        };
    };
}
