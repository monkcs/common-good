#pragma once

/*
	https://datatracker.ietf.org/doc/html/rfc6838
	https://datatracker.ietf.org/doc/html/rfc2045#section-5.1 (ABNF)
	https://datatracker.ietf.org/doc/html/rfc7231#section-3.1.1.1
*/

#include "ascii.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <initializer_list>
#include <iterator>
#include <map>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace common_good
{
	/// @brief Media type as defined by RFC 6838.
	class media_type
	{
		struct character_offset
		{
			unsigned char after_slash {};
			unsigned char after_possible_first_dot {};
			unsigned char possible_last_plus {};
		};

		std::string value;
		character_offset offset {};

		/// @brief Test if string only contain alphanumeric lowercase characters or '!', '#', '$', '&', '-', '^', '_', '.', '+'.
		/// @param string String to test.
		/// @return True if string only contain allowed characters.
		[[nodiscard]] static constexpr auto is_restricted_name(const std::string_view string) noexcept
		{
			auto predicate = [](const char character) noexcept
			{
				static constexpr auto list = {'!', '#', '$', '&', '-', '^', '_', '.', '+'};
				return ascii::is_alphanumeric_lowercase(character) or std::ranges::contains(list, character);
			};

			return std::ranges::all_of(string, predicate);
		}

		/// @brief Test if string only contain alphanumeric lowercase characters or '!', '#', '$', '&', '-', '^', '_'.
		/// @brief Same as `is_restricted_name(string_view)` but not allowing '.' or '+'.
		/// @param string String to test.
		/// @return True if string only contain allowed characters.
		[[nodiscard]] static constexpr auto is_modified_restricted_name(const std::string_view string) noexcept
		{
			auto predicate = [](const char character) noexcept
			{
				static constexpr auto list = {'!', '#', '$', '&', '-', '^', '_'};
				return ascii::is_alphanumeric_lowercase(character) or std::ranges::contains(list, character);
			};

			return std::ranges::all_of(string, predicate);
		}

		/// @brief Validate top-level type.
		/// @param input Top-level type in format 'type'.
		/// @exception media_type::parsing_error If string is empty.
		/// @exception media_type::parsing_error If string is not less than 128 characters.
		/// @exception media_type::parsing_error If first character is not alphanumeric.
		/// @exception media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
		/// '^', '_', '.', '+'.
		constexpr static void validate_type(const std::string_view value)
		{
			if (value.empty() or value.size() > 127)
			{
				throw parsing_error {"media type: top-level type: lenght required to be [1..127] characters"};
			}

			if (not ascii::is_alphanumeric_lowercase(value.front()))
			{
				throw parsing_error {"media type: top-level type: first character required to be alphanumeric"};
			}

			if (not is_restricted_name(value))
			{
				throw parsing_error {"media type: top-level type: containing non-valid characters"};
			}
		}

		/// @brief Validate registration tree.
		/// @param input Registration tree in format '' (empty, standards tree) or 'tree.'
		/// @exception media_type::parsing_error If string is not less than 128 characters.
		/// @exception media_type::parsing_error If first character is not alphanumeric.
		/// @exception media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
		/// '^', '_'.
		/// @exception media_type::parsing_error If last character is not '.'.
		constexpr static void validate_tree(const std::string_view value)
		{
			if (value.empty())
			{
				return;
			}

			if (value.size() < 2 or value.size() > 127)
			{
				throw parsing_error {"media type: tree: lenght required to be [2..127] characters"};
			}
			else
			{
				if (not ascii::is_alphanumeric_lowercase(value.front()))
				{
					throw parsing_error {"media type: tree: first character required to be alphanumeric"};
				}

				if (value.back() != '.')
				{
					throw parsing_error {"media type: tree: last character required to be '.'"};
				}

				if (not is_modified_restricted_name({value.begin(), std::prev(value.end())}))
				{
					throw parsing_error {"media type: tree: containing non-valid characters"};
				}
			}
		}

		/// @brief Validate subtype.
		/// @param input Subtype in format 'subtype'
		/// @exception media_type::parsing_error If string is empty.
		/// @exception media_type::parsing_error If string is not less than 128 characters.
		/// @exception media_type::parsing_error If first character is not alphanumeric.
		/// @exception media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
		/// '^', '_', '.', '+'.
		constexpr static void validate_subtype(const std::string_view value)
		{
			if (value.empty() or value.size() > 127)
			{
				throw parsing_error {"media type: subtype: lenght required to be [1..127] characters"};
			}

			if (not ascii::is_alphanumeric(value.front()))
			{
				throw parsing_error {"media type: subtype: first character required to be alphanumeric"};
			}

			if (not is_restricted_name(value))
			{
				throw parsing_error {"media type: subtype: containing non-valid characters"};
			}
		}

		/// @brief Validate suffix.
		/// @param input Suffix in format '+suffix'.
		/// @exception media_type::parsing_error If string is less than 2 characters.
		/// @exception media_type::parsing_error If string is not less than 128 characters.
		/// @exception media_type::parsing_error If string does not begin with '+'.
		/// @exception media_type::parsing_error If second character is not alphanumeric.
		/// @exception media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
		/// '^', '_'.
		constexpr static void validate_suffix(const std::string_view value)
		{
			if (value.size() < 2 or value.size() > 127)
			{
				throw parsing_error {"media type: suffix: lenght required to be [2..127] characters"};
			}
			else
			{
				if (value.front() != '+')
				{
					throw parsing_error {"media type: suffix: first character required to be '+'"};
				}
				else
				{
					const std::string_view substring {std::next(value.begin()), value.end()};

					if (not ascii::is_alphanumeric(substring.front()))
					{
						throw parsing_error {"media type: suffix: second character required to be alphanumeric"};
					}

					if (not is_modified_restricted_name(substring))
					{
						throw parsing_error {"media type: suffix: containing non-valid characters"};
					}
				}
			}
		}

		constexpr void parse()
		{
			std::string_view view = value;

			offset.after_slash = [&view]
			{
				if (const auto slash_offset = view.find('/'); slash_offset == std::string_view::npos)
				{
					throw parsing_error {"media type: parsing: missing delimiter '/' after type"};
				}
				else
				{
					validate_type({view.begin(), slash_offset});
					view.remove_prefix(slash_offset + 1);
					return slash_offset + 1;
				}
			}();

			offset.after_possible_first_dot = [&view]
			{
				if (const auto first_dot_offset = view.find('.'); first_dot_offset == 0)
				{
					throw parsing_error {"media type: parsing: missing tree between delimiter '/' and '.'"};
				}
				else if (first_dot_offset == std::string_view::npos)
				{
					return std::size_t {};
				}
				else
				{
					validate_tree({view.begin(), first_dot_offset + 1});
					view.remove_prefix(first_dot_offset + 1);
					return first_dot_offset + 1;
				}
			}();

			offset.possible_last_plus = [&view]
			{
				if (const auto last_plus_offset = view.rfind('+'); last_plus_offset == std::string_view::npos)
				{
					validate_subtype(view);
					view = {};
					return std::size_t {};
				}
				else
				{
					validate_subtype({view.begin(), last_plus_offset});
					view.remove_prefix(last_plus_offset);
					return last_plus_offset;
				}
			}();

			if (not view.empty())
			{
				validate_suffix(view);
			}
		}

		constexpr void prepare()
		{
			/* As this class not currently supports parameters, the following lines ignores potentional parameters. */
			if (const auto sentiel = std::ranges::find(value, ';'); sentiel != value.end())
			{
				value = std::string {std::begin(value), sentiel};
			}

			std::ranges::transform(value, std::begin(value), ascii::to_lowercase);
		}

	  public:
		/// @brief Media type as defined by RFC 6838. NOTE: Currently no support for parameters.
		/// @param type Media type in format 'type/tree.subtype+suffix'
		/// @exception media_type::parsing_error If string fail to parse.
		[[nodiscard]] constexpr media_type(std::string&& type) : value {std::move(type)}
		{
			prepare();
			parse();
		}

		/// @brief Media type as defined by RFC 6838. NOTE: Currently no support for parameters.
		/// @param type Media type in format 'type/tree.subtype+suffix'
		/// @exception media_type::parsing_error If string fail to parse.
		[[nodiscard]] constexpr media_type(const std::string& type) : value {type}
		{
			prepare();
			parse();
		}

		[[nodiscard]] constexpr auto operator==(const media_type& other) const noexcept -> bool { return value == other.value; };
		[[nodiscard]] constexpr auto operator==(const std::string& other) const noexcept -> bool { return value == other; };
		[[nodiscard]] constexpr auto operator==(const std::string_view& other) const noexcept -> bool { return value == other; };
		[[nodiscard]] constexpr auto operator==(const char* const other) const noexcept -> bool { return value == other; };

		/// @brief Get media type as string in format 'type/tree.subtype+suffix'
		[[nodiscard]] constexpr auto string() && noexcept -> std::string { return std::move(value); }

		/// @brief Get media type as string in format 'type/tree.subtype+suffix'
		[[nodiscard]] constexpr auto string() const& -> const std::string& { return value; }

		/// @brief Get top-level type.
		[[nodiscard]] constexpr auto type() const -> std::string { return value.substr(0, offset.after_slash - 1); }

		/// @brief Get registration tree.
		[[nodiscard]] constexpr auto tree() const -> std::string
		{
			if (offset.after_possible_first_dot)
			{
				return value.substr(offset.after_slash, offset.after_possible_first_dot);
			}
			else
			{
				return {};
			}
		}

		/// @brief Get subtype.
		[[nodiscard]] constexpr auto subtype() const -> std::string
		{
			const auto start = static_cast<std::size_t>(offset.after_slash) + static_cast<std::size_t>(offset.after_possible_first_dot);

			if (offset.possible_last_plus)
			{
				return value.substr(start, offset.possible_last_plus);
			}
			else
			{
				return value.substr(start);
			}
		}

		/// @brief Get structured type name suffix.
		[[nodiscard]] constexpr auto suffix() const -> std::string
		{
			if (offset.possible_last_plus)
			{
				const auto start = static_cast<std::size_t>(offset.after_slash) + static_cast<std::size_t>(offset.after_possible_first_dot)
								   + static_cast<std::size_t>(offset.possible_last_plus);
				return value.substr(start);
			}
			else
			{
				return {};
			}
		}

		/// @brief Check if media type is in standards tree.
		/// @return True if media type is in standards tree.
		[[nodiscard]] constexpr auto in_standards_tree() const noexcept -> bool { return not offset.after_possible_first_dot; }

		class parsing_error : public std::invalid_argument
		{
		  public:
			[[nodiscard]] explicit parsing_error(const std::string& message) noexcept : std::invalid_argument {message} { };
			[[nodiscard]] explicit parsing_error(const char* const message) noexcept : std::invalid_argument {message} { };
		};
	};
}

/// @brief Media type as defined by RFC 6838. NOTE: Currently no support for parameters.
/// @param string Media type in format 'type/tree.subtype+suffix'
/// @exception media_type::parsing_error If string fail to parse.
[[nodiscard]] constexpr auto operator""_media_type(const char* const string, const std::size_t lenght)
{ return common_good::media_type {std::string {string, lenght}}; }

template<>
struct std::formatter<common_good::media_type> : std::formatter<std::string>
{
	constexpr auto format(const common_good::media_type& media_type, auto&& context) const
	{ return std::formatter<std::string>::format(media_type.string(), context); }
};