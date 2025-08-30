#pragma once

/* https://datatracker.ietf.org/doc/html/rfc6838 */

#include "ascii.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace common_good
{
	/// @brief Media Type as defined by RFC 6838.
	class media_type
	{
		/// @brief Test if string only contain alphanumeric characters and characters in list.
		/// @param string String to test.
		/// @param list List of allowed characters.
		/// @return True if string only contain alphanumeric characters and characters in list.
		[[nodiscard]] static constexpr auto allowed_characters(const std::string_view string,
															   const std::initializer_list<char> list) noexcept
		{
			auto lambda = [&list](const char character) noexcept
			{ return ascii::is_alphanumeric(character) or std::ranges::contains(list, character); };
			return std::ranges::all_of(string, lambda);
		}

	  public:
		/// @brief Media Type parsing error.
		class parsing_error : public std::invalid_argument
		{
		  public:
			/// @brief Media Type parsing error.
			/// @param message Error message.
			[[nodiscard]] explicit parsing_error(const std::string& message) noexcept : std::invalid_argument {message} { };

			/// @brief Media Type parsing error.
			/// @param message Error message.
			[[nodiscard]] explicit parsing_error(const char* const message) noexcept : std::invalid_argument {message} { };
		};

		/// @brief Media Type top-level type.
		class media_top_type
		{
			std::string value;

		  public:
			/// @brief Media Type top-level type.
			/// @param input Top-level type in format 'type'.
			/// @exception common_good::media_type::parsing_error If string is empty.
			/// @exception common_good::media_type::parsing_error If string is not less than 128 characters.
			/// @exception common_good::media_type::parsing_error If first character is not alphanumeric.
			/// @exception common_good::media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
			/// '^', '_', '.', '+'.
			[[nodiscard]] constexpr media_top_type(std::string input) : value {std::move(input)}
			{
				if (value.empty() or value.size() > 127)
				{
					throw parsing_error {"media type: top-level type: lenght required to be [1..127] characters"};
				}

				if (not ascii::is_alphanumeric(value.front()))
				{
					throw parsing_error {"media type: top-level type: first character required to be alphanumeric"};
				}

				if (not allowed_characters(value, {'!', '#', '$', '&', '-', '^', '_', '.', '+'}))
				{
					throw parsing_error {"media type: top-level type: containing non-valid characters"};
				}

				std::ranges::transform(value, value.begin(), ascii::to_lowercase);
			}

			[[nodiscard]] constexpr auto operator==(const media_top_type&) const noexcept -> bool = default;

			/// @brief Get string representing top-level type.
			/// @return Const reference to string.
			[[nodiscard]] constexpr auto& string() const& noexcept { return value; }

			/// @brief Get string representing top-level type.
			[[nodiscard]] constexpr auto string() && noexcept { return std::move(value); }
		};

		/// @brief Media Type registration tree.
		class media_tree
		{
			std::string value;

		  public:
			/// @brief Media Type registration tree.
			/// @param input Registration tree in format '' (empty, standards tree) or 'tree.'
			/// @exception common_good::media_type::parsing_error If string is not less than 128 characters.
			/// @exception common_good::media_type::parsing_error If first character is not alphanumeric.
			/// @exception common_good::media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
			/// '^', '_'.
			/// @exception common_good::media_type::parsing_error If last character is not '.'.
			[[nodiscard]] constexpr media_tree(std::string input) : value {std::move(input)}
			{
				if (value.empty())
				{
					return;
				}

				if (value.size() < 2 or value.size() > 127)
				{
					throw parsing_error {"media type: tree: lenght required to be [2..127] characters"};
				}

				if (not ascii::is_alphanumeric(value.front()))
				{
					throw parsing_error {"media type: tree: first character required to be alphanumeric"};
				}

				if (value.back() != '.')
				{
					throw parsing_error {"media type: tree: last character required to be '.'"};
				}

				if (not allowed_characters({value.begin(), std::prev(value.end())}, {'!', '#', '$', '&', '-', '^', '_'}))
				{
					throw parsing_error {"media type: tree: containing non-valid characters"};
				}

				std::ranges::transform(value, value.begin(), ascii::to_lowercase);
			}

			[[nodiscard]] constexpr auto operator==(const media_tree&) const noexcept -> bool = default;

			/// @brief Get string representing registration tree. String is empty if tree is standard tree.
			/// @return Const reference to string.
			[[nodiscard]] constexpr auto& string() const& noexcept { return value; }

			/// @brief Get string representing registration tree. String is empty if tree is standard tree.
			[[nodiscard]] constexpr auto string() && noexcept { return std::move(value); }

			/// @brief Test if registration tree is standard tree.
			/// @return Return true if registration tree is standard tree.
			[[nodiscard]] constexpr auto standard() const noexcept -> bool { return value.empty(); }
		};

		/// @brief Media Type subtype.
		class media_subtype
		{
			std::string value;

		  public:
			/// @brief Media Type subtype.
			/// @param input Subtype in format 'subtype'
			/// @exception common_good::media_type::parsing_error If string is empty.
			/// @exception common_good::media_type::parsing_error If string is not less than 128 characters.
			/// @exception common_good::media_type::parsing_error If first character is not alphanumeric.
			/// @exception common_good::media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
			/// '^', '_',
			/// '.', '+'.
			[[nodiscard]] constexpr media_subtype(std::string input) : value {std::move(input)}
			{
				if (value.empty() or value.size() > 127)
				{
					throw parsing_error {"media type: subtype: lenght required to be [1..127] characters"};
				}

				if (not ascii::is_alphanumeric(value.front()))
				{
					throw parsing_error {"media type: subtype: first character required to be alphanumeric"};
				}

				if (not allowed_characters(value, {'!', '#', '$', '&', '-', '^', '_', '.', '+'}))
				{
					throw parsing_error {"media type: subtype: containing non-valid characters"};
				}

				std::ranges::transform(value, value.begin(), ascii::to_lowercase);
			}

			[[nodiscard]] constexpr auto operator==(const media_subtype&) const noexcept -> bool = default;

			/// @brief Get string representing subtype.
			/// @return Const reference to string.
			[[nodiscard]] constexpr auto& string() const& noexcept { return value; }

			/// @brief Get string representing subtype.
			[[nodiscard]] constexpr auto string() && noexcept { return std::move(value); }
		};

		/// @brief Media Type suffix.
		class media_suffix
		{
			std::string value;

		  public:
			/// @brief Media Type suffix.
			/// @param input Suffix in format '+suffix'.
			/// @exception common_good::media_type::parsing_error If string is less than 2 characters.
			/// @exception common_good::media_type::parsing_error If string is not less than 128 characters.
			/// @exception common_good::media_type::parsing_error If string does not begin with '+'.
			/// @exception common_good::media_type::parsing_error If second character is not alphanumeric.
			/// @exception common_good::media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
			/// '^', '_'.
			[[nodiscard]] constexpr media_suffix(std::string input) : value {std::move(input)}
			{
				if (value.size() < 2 or value.size() > 127)
				{
					throw parsing_error {"media type: suffix: lenght required to be [2..127] characters"};
				}

				if (value.front() != '+')
				{
					throw parsing_error {"media type: suffix: first character required to be '+'"};
				}

				if (not ascii::is_alphanumeric(value[1]))
				{
					throw parsing_error {"media type: suffix: second character required to be alphanumeric"};
				}

				if (not allowed_characters({std::next(value.begin()), value.end()}, {'!', '#', '$', '&', '-', '^', '_'}))
				{
					throw parsing_error {"media type: suffix: containing non-valid characters"};
				}

				std::ranges::transform(value, value.begin(), ascii::to_lowercase);
			}

			[[nodiscard]] constexpr auto operator==(const media_suffix&) const noexcept -> bool = default;

			/// @brief Get string representing suffix.
			/// @return Const reference to string.
			[[nodiscard]] constexpr auto& string() const& noexcept { return value; }

			/// @brief Get string representing suffix.
			[[nodiscard]] constexpr auto string() && noexcept { return std::move(value); }
		};

	  private:
		/// @brief Parse media type as defined by RFC 6838. NOTE: Currently no support for parameters.
		/// @param input Media type in format 'type/tree.subtype+suffix'
		/// @return Constructed media_type
		/// @exception common_good::media_type::parsing_error If string fail to parse.
		[[nodiscard]] constexpr static auto parser(std::string_view input)
		{
			using std::ranges::find;
			using std::ranges::find_last;

			/* As this class not currently supports parameters, the following line ignores potentional parameters. */
			input = std::string_view {input.begin(), find(input, ';')};

			auto type = [&input]
			{
				if (const std::string_view match {input.begin(), find(input, '/')}; match.end() == input.end())
				{
					throw parsing_error {"media type: parsing: missing delimiter '/' after type"};
				}
				else
				{
					input.remove_prefix(match.size() + 1);
					return media_top_type {std::string {match}};
				}
			}();

			auto tree = [&input]
			{
				if (const auto match = find(input, '.'); match == input.begin())
				{
					throw parsing_error {"media type: parsing: missing tree between '/' and '.'"};
				}
				else if (match == input.end())
				{
					return media_tree {""};
				}
				else
				{
					const std::string_view result {input.begin(), std::next(match)};
					input.remove_prefix(result.size());
					return media_tree {std::string {result}};
				}
			}();

			auto subtype = [&input]
			{
				const std::string_view match {input.begin(), find_last(input, '+').begin()};
				input.remove_prefix(match.size());
				return media_subtype {std::string {match}};
			}();

			auto suffix = input.empty() ? std::nullopt : std::optional<media_suffix> {std::string {input}};

			return media_type {std::move(type), std::move(tree), std::move(subtype), std::move(suffix)};
		}

	  public:
		/// @brief Media Type as defined by RFC 6838. NOTE: Currently no support for parameters.
		/// @param input Media type in format 'type/tree.subtype+suffix'
		/// @exception common_good::media_type::parsing_error If string fail to parse.
		constexpr media_type(std::string_view input) : media_type {parser(input)} { };

		/// @brief Media Type as defined by RFC 6838. NOTE: Currently no support for parameters.
		/// @param type Top-level type.
		/// @param tree Registration tree.
		/// @param subtype Subtype.
		/// @param suffix Optional suffix.
		constexpr media_type(media_top_type type, media_tree tree, media_subtype subtype, std::optional<media_suffix> suffix = {}) noexcept
			:
			type {std::move(type)}, tree {std::move(tree)}, subtype {std::move(subtype)}, suffix {std::move(suffix)} { };

		/// @brief Top-level type.
		media_top_type type;

		/// @brief Registration tree.
		media_tree tree;

		/// @brief Subtype.
		media_subtype subtype;

		/// @brief Optional structured type name suffix.
		std::optional<media_suffix> suffix;

		[[nodiscard]] constexpr auto operator==(const media_type&) const noexcept -> bool = default;

		/// @brief Get media type as string in format 'type/tree.subtype+suffix'
		[[nodiscard]] constexpr operator std::string() const;

		/// @brief Get media type as string in format 'type/tree.subtype+suffix'
		[[nodiscard]] constexpr auto string() const { return operator std::string(); }
	};
}

constexpr auto operator""_media_type(const char* string, const std::size_t lenght)
{
	return common_good::media_type {std::string_view {string, lenght}};
}

constexpr auto operator""_top_type(const char* string, const std::size_t lenght)
{
	return common_good::media_type::media_top_type {std::string {string, lenght}};
}

constexpr auto operator""_tree(const char* string, const std::size_t lenght)
{
	return common_good::media_type::media_tree {std::string {string, lenght}};
}

constexpr auto operator""_subtype(const char* string, const std::size_t lenght)
{
	return common_good::media_type::media_subtype {std::string {string, lenght}};
}

constexpr auto operator""_suffix(const char* string, const std::size_t lenght)
{
	return common_good::media_type::media_suffix {std::string {string, lenght}};
}

template<>
struct std::formatter<common_good::media_type::media_top_type> : std::formatter<std::string>
{
	constexpr auto format(const common_good::media_type::media_top_type& type, std::format_context& context) const
	{
		return std::formatter<std::string>::format(type.string(), context);
	}
};

template<>
struct std::formatter<common_good::media_type::media_tree> : std::formatter<std::string>
{
	constexpr auto format(const common_good::media_type::media_tree& tree, std::format_context& context) const
	{
		return std::formatter<std::string>::format(tree.string(), context);
	}
};

template<>
struct std::formatter<common_good::media_type::media_subtype> : std::formatter<std::string>
{
	constexpr auto format(const common_good::media_type::media_subtype& subtype, std::format_context& context) const
	{
		return std::formatter<std::string>::format(subtype.string(), context);
	}
};

template<>
struct std::formatter<common_good::media_type::media_suffix> : std::formatter<std::string>
{
	constexpr auto format(const common_good::media_type::media_suffix& suffix, std::format_context& context) const
	{
		return std::formatter<std::string>::format(suffix.string(), context);
	}
};

template<>
struct std::formatter<std::optional<common_good::media_type::media_suffix>> : std::formatter<std::string>
{
	constexpr auto format(const std::optional<common_good::media_type::media_suffix>& optional_suffix, std::format_context& context) const
	{
		return std::formatter<std::string>::format(optional_suffix ? optional_suffix->string() : "", context);
	}
};

template<>
struct std::formatter<common_good::media_type> : std::formatter<std::string>
{
	constexpr auto format(const common_good::media_type& media_type, std::format_context& context) const
	{
		return std::formatter<std::string>::format(media_type.string(), context);
	}
};

namespace common_good
{
	[[nodiscard]] constexpr media_type::operator std::string() const { return std::format("{}/{}{}{}", type, tree, subtype, suffix); }
}