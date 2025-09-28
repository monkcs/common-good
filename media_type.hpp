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
	/// @brief Media Type as defined by RFC 6838.
	namespace rfc6838
	{
		/// @brief Test if string only contain alphanumeric characters or '!', '#', '$', '&', '-', '^', '_', '.', '+'.
		/// @param string String to test.
		/// @return True if string only contain allowed characters.
		[[nodiscard]] static constexpr auto is_restricted_name(const std::string_view string) noexcept
		{
			auto predicate = [](const char character) noexcept
			{
				static constexpr auto list = {'!', '#', '$', '&', '-', '^', '_', '.', '+'};
				return ascii::is_alphanumeric(character) or std::ranges::contains(list, character);
			};

			return std::ranges::all_of(string, predicate);
		}

		/// @brief Test if string only contain alphanumeric characters or '!', '#', '$', '&', '-', '^', '_'.
		/// @brief Same as `is_restricted_name(string_view)` but not allowing '.' or '+'.
		/// @param string String to test.
		/// @return True if string only contain allowed characters.
		[[nodiscard]] static constexpr auto is_modified_restricted_name(const std::string_view string) noexcept
		{
			auto predicate = [](const char character) noexcept
			{
				static constexpr auto list = {'!', '#', '$', '&', '-', '^', '_'};
				return ascii::is_alphanumeric(character) or std::ranges::contains(list, character);
			};

			return std::ranges::all_of(string, predicate);
		}

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
		class type
		{
			std::string value;

		  public:
			/// @brief Media Type top-level type.
			/// @param input Top-level type in format 'type'.
			/// @exception media_type::parsing_error If string is empty.
			/// @exception media_type::parsing_error If string is not less than 128 characters.
			/// @exception media_type::parsing_error If first character is not alphanumeric.
			/// @exception media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
			/// '^', '_', '.', '+'.
			[[nodiscard]] constexpr type(std::string input) : value {std::move(input)}
			{
				if (value.empty() or value.size() > 127)
				{
					throw parsing_error {"media type: top-level type: lenght required to be [1..127] characters"};
				}

				if (not ascii::is_alphanumeric(value.front()))
				{
					throw parsing_error {"media type: top-level type: first character required to be alphanumeric"};
				}

				if (not is_restricted_name(value))
				{
					throw parsing_error {"media type: top-level type: containing non-valid characters"};
				}

				std::ranges::transform(value, value.begin(), ascii::to_lowercase);
			}

			[[nodiscard]] constexpr auto operator==(const type&) const noexcept -> bool = default;

			/// @brief Get string representing top-level type.
			/// @return Const reference to string.
			[[nodiscard]] constexpr const auto& string() const& noexcept { return value; }

			/// @brief Get string representing top-level type.
			[[nodiscard]] constexpr auto string() && noexcept { return std::move(value); }
		};

		/// @brief Media Type registration tree.
		class tree
		{
			std::string value;

		  public:
			/// @brief Media Type registration tree.
			/// @param input Registration tree in format '' (empty, standards tree) or 'tree.'
			/// @exception media_type::parsing_error If string is not less than 128 characters.
			/// @exception media_type::parsing_error If first character is not alphanumeric.
			/// @exception media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
			/// '^', '_'.
			/// @exception media_type::parsing_error If last character is not '.'.
			[[nodiscard]] constexpr tree(std::string input) : value {std::move(input)}
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
					if (not ascii::is_alphanumeric(value.front()))
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

					std::ranges::transform(value, value.begin(), ascii::to_lowercase);
				}
			}

			[[nodiscard]] constexpr auto operator==(const tree&) const noexcept -> bool = default;

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
		class subtype
		{
			std::string value;

		  public:
			/// @brief Media Type subtype.
			/// @param input Subtype in format 'subtype'
			/// @exception media_type::parsing_error If string is empty.
			/// @exception media_type::parsing_error If string is not less than 128 characters.
			/// @exception media_type::parsing_error If first character is not alphanumeric.
			/// @exception media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
			/// '^', '_', '.', '+'.
			[[nodiscard]] constexpr subtype(std::string input) : value {std::move(input)}
			{
				if (value.empty() or value.size() > 127)
				{
					throw parsing_error {"media type: subtype: lenght required to be [1..127] characters"};
				}
				else
				{
					if (not ascii::is_alphanumeric(value.front()))
					{
						throw parsing_error {"media type: subtype: first character required to be alphanumeric"};
					}

					if (not is_restricted_name(value))
					{
						throw parsing_error {"media type: subtype: containing non-valid characters"};
					}

					std::ranges::transform(value, value.begin(), ascii::to_lowercase);
				}
			}

			[[nodiscard]] constexpr auto operator==(const subtype&) const noexcept -> bool = default;

			/// @brief Get string representing subtype.
			/// @return Const reference to string.
			[[nodiscard]] constexpr auto& string() const& noexcept { return value; }

			/// @brief Get string representing subtype.
			[[nodiscard]] constexpr auto string() && noexcept { return std::move(value); }
		};

		/// @brief Media Type suffix.
		class suffix
		{
			std::string value;

		  public:
			/// @brief Media Type suffix.
			/// @param input Suffix in format '+suffix'.
			/// @exception media_type::parsing_error If string is less than 2 characters.
			/// @exception media_type::parsing_error If string is not less than 128 characters.
			/// @exception media_type::parsing_error If string does not begin with '+'.
			/// @exception media_type::parsing_error If second character is not alphanumeric.
			/// @exception media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
			/// '^', '_'.
			[[nodiscard]] constexpr suffix(std::string input) : value {std::move(input)}
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

						std::ranges::transform(value, value.begin(), ascii::to_lowercase);
					}
				}
			}

			[[nodiscard]] constexpr auto operator==(const suffix&) const noexcept -> bool = default;

			/// @brief Get string representing suffix.
			/// @return Const reference to string.
			[[nodiscard]] constexpr auto& string() const& noexcept { return value; }

			/// @brief Get string representing suffix.
			[[nodiscard]] constexpr auto string() && noexcept { return std::move(value); }
		};

		/// @brief Media Type parameter name.
		class parameter_name
		{
			std::string value;

		  public:
			/// @brief Media Type parameter name.
			/// @param input Parameter name in format 'name'
			/// @exception media_type::parsing_error If string is empty.
			/// @exception media_type::parsing_error If string is not less than 128 characters.
			/// @exception media_type::parsing_error If first character is not alphanumeric.
			/// @exception media_type::parsing_error If remainder of string is not alphanumeric or '!', '#', '$', '&', '-',
			/// '^', '_', '.', '+'.
			[[nodiscard]] constexpr parameter_name(std::string input) : value {std::move(input)}
			{
				if (value.empty() or value.size() > 127)
				{
					throw parsing_error {"media type: parameter name: lenght required to be [1..127] characters"};
				}
				else
				{
					if (not ascii::is_alphanumeric(value.front()))
					{
						throw parsing_error {"media type: parameter name: first character required to be alphanumeric"};
					}

					if (not is_restricted_name(value))
					{
						throw parsing_error {"media type: parameter name: containing non-valid characters"};
					}

					std::ranges::transform(value, value.begin(), ascii::to_lowercase);
				}
			}

			[[nodiscard]] constexpr auto operator<=>(const parameter_name&) const noexcept = default;

			/// @brief Get string representing parameter attribute.
			/// @return Const reference to string.
			[[nodiscard]] constexpr auto& string() const& noexcept { return value; }

			/// @brief Get string representing parameter attribute.
			[[nodiscard]] constexpr auto string() && noexcept { return std::move(value); }
		};

		/// @brief Media Type parameter value.
		class parameter_value
		{
			std::string backend;

			[[nodiscard]] constexpr auto inner_string() const noexcept { return std::string_view {backend.begin() + 1, backend.end() - 1}; }

		  public:
			/// @brief Media Type parameter value.
			[[nodiscard]] constexpr parameter_value(std::string input) : backend {std::move(input)}
			{
				if (backend.empty() or backend.size() > 127)
				{
					throw parsing_error {"media type: parameter value: lenght required to be [1..127] characters"};
				}
				else
				{
					if (backend.front() == '"')
					{
						if (backend.size() == 1 or backend.back() != '"')
						{
							throw parsing_error {"media type: parameter value: quoted string missing trailing '\"'"};
						}

						if (backend.size() == 2)
						{
							throw parsing_error {"media type: parameter value: quoted string empty"};
						}

						if (std::ranges::contains(inner_string(), '"'))
						{
							throw parsing_error {"media type: parameter value: quoted string containing '\"'"};
						}
					}
					else
					{
						if (not is_restricted_name(backend))
						{
							throw parsing_error {"media type: parameter value: non-quoted string containing non-valid characters"};
						}
					}
				}
			}

			/// @brief Get string representing parameter value, may be quoted.
			/// @return Const reference to string.
			[[nodiscard]] constexpr auto& string() const& noexcept { return backend; }

			/// @brief Get string representing parameter value, may be quoted.
			[[nodiscard]] constexpr auto string() && noexcept { return std::move(backend); }

			[[nodiscard]] constexpr auto value_view() const& noexcept
			{
				return (backend.front() == '"') ? inner_string() : std::string_view {backend};
			}
			auto value_view() && = delete("Use value() instead on a r-value object");

			[[nodiscard]] constexpr auto value() const& noexcept { return backend.front() == '"' ? std::string {inner_string()} : backend; }

			[[nodiscard]] constexpr auto value() && noexcept
			{
				return backend.front() == '"' ? std::string {inner_string()} : std::move(backend);
			}

			[[nodiscard]] friend constexpr auto operator==(const parameter_value& first, const parameter_value& second) noexcept -> bool
			{
				return first.value_view() == second.value_view();
			}
		};

		/// @brief Media Type as defined by RFC 6838.
		class media_type
		{
			[[nodiscard]] constexpr static auto parser(std::string_view input) -> media_type
			{
				/* As this class not currently supports parameters, the following line ignores potentional parameters. */
				input = {std::begin(input), std::ranges::find(input, ';')};

				const auto type = [begin = std::begin(input), &input] -> std::string_view
				{
					const auto match = std::ranges::find(input, '/');

					if (match == std::end(input))
					{
						throw parsing_error {"media type: parsing: missing delimiter '/' after type"};
					}
					else
					{
						input = {std::next(match), std::end(input)};
						return {begin, match};
					}
				}();

				const auto tree = [begin = std::begin(input), &input] -> std::string_view
				{
					const auto match = std::ranges::find(input, '.');

					if (match == begin)
					{
						throw parsing_error {"media type: parsing: missing tree between '/' and '.'"};
					}
					else if (match == std::end(input))
					{
						return {""};
					}
					else
					{
						input = {std::next(match), std::end(input)};
						return {begin, std::next(match)};
					}
				}();

				const auto subtype = [begin = std::begin(input), &input] -> std::string_view
				{
					const auto match = std::begin(std::ranges::find_last(input, '+'));
					input = {match, std::end(input)};
					return {begin, match};
				}();

				const auto suffix = input.empty() ? std::nullopt : std::optional<rfc6838::suffix> {std::string {input}};

				return media_type {{std::string {type}}, {std::string {tree}}, {std::string {subtype}}, suffix};
			}

		  public:
			/// @brief Media Type parsing error.
			using parsing_error = rfc6838::parsing_error;

			/// @brief Media Type as defined by RFC 6838. NOTE: Currently no support for parameters.
			/// @param input Media type in format 'type/tree.subtype+suffix'
			/// @exception media_type::parsing_error If string fail to parse.
			constexpr media_type(std::string_view input) : media_type {parser(input)} { };

			/// @brief Media Type as defined by RFC 6838. NOTE: Currently no support for parameters.
			/// @param type Top-level type.
			/// @param tree Registration tree.
			/// @param subtype Subtype.
			/// @param suffix Optional suffix.
			constexpr media_type(rfc6838::type type,
								 rfc6838::tree tree,
								 rfc6838::subtype subtype,
								 std::optional<rfc6838::suffix> suffix = {}) noexcept :
				type {std::move(type)}, tree {std::move(tree)}, subtype {std::move(subtype)}, suffix {std::move(suffix)} { };

			/// @brief Top-level type.
			rfc6838::type type;

			/// @brief Registration tree.
			rfc6838::tree tree;

			/// @brief Subtype.
			rfc6838::subtype subtype;

			/// @brief Optional structured type name suffix.
			std::optional<rfc6838::suffix> suffix;

			[[nodiscard]] constexpr auto operator==(const media_type&) const noexcept -> bool = default;

			/// @brief Get media type as string in format 'type/tree.subtype+suffix'
			[[nodiscard]] constexpr operator std::string() const
			{
				/* std::format is not as of C++26 constexpr. */
				if consteval
				{
					return std::string {type.string()}
						.append("/")
						.append(tree.string())
						.append(subtype.string())
						.append(suffix ? suffix->string() : "");
				}
				else
				{
					return std::format("{}/{}{}{}", type.string(), tree.string(), subtype.string(), suffix ? suffix->string() : "");
				}
			}

			/// @brief Get media type as string in format 'type/tree.subtype+suffix'
			[[nodiscard]] constexpr auto string() const -> std::string { return operator std::string(); }

			/// @brief Get copy of media type without a suffix.
			/// @return Media type consisting of type, tree and subtype.
			[[nodiscard]] constexpr auto without_suffix() const& noexcept -> media_type { return {type, tree, subtype}; }

			/// @brief Get media type without a suffix.
			/// @return Media type consisting of type, tree and subtype.
			[[nodiscard]] constexpr auto without_suffix() && noexcept -> media_type
			{
				suffix.reset();
				return std::move(*this);
			}
		};
	}

	using rfc6838::media_type;
}

/// @brief Media Type as defined by RFC 6838. NOTE: Currently no support for parameters.
/// @param string Media type in format 'type/tree.subtype+suffix'
/// @exception media_type::parsing_error If string fail to parse.
constexpr auto operator""_media_type(const char* string, const std::size_t lenght) { return common_good::media_type {{string, lenght}}; }

template<>
struct std::formatter<common_good::rfc6838::type> : std::formatter<std::string>
{
	constexpr auto format(const common_good::rfc6838::type& type, auto&& context) const
	{
		return std::formatter<std::string>::format(type.string(), context);
	}
};

template<>
struct std::formatter<common_good::rfc6838::tree> : std::formatter<std::string>
{
	constexpr auto format(const common_good::rfc6838::tree& tree, auto&& context) const
	{
		return std::formatter<std::string>::format(tree.string(), context);
	}
};

template<>
struct std::formatter<common_good::rfc6838::subtype> : std::formatter<std::string>
{
	constexpr auto format(const common_good::rfc6838::subtype& subtype, auto&& context) const
	{
		return std::formatter<std::string>::format(subtype.string(), context);
	}
};

template<>
struct std::formatter<common_good::rfc6838::suffix> : std::formatter<std::string>
{
	constexpr auto format(const common_good::rfc6838::suffix& suffix, auto&& context) const
	{
		return std::formatter<std::string>::format(suffix.string(), context);
	}
};

template<>
struct std::formatter<std::optional<common_good::rfc6838::suffix>> : std::formatter<std::string>
{
	constexpr auto format(const std::optional<common_good::rfc6838::suffix>& optional_suffix, auto&& context) const
	{
		return std::formatter<std::string>::format(optional_suffix ? optional_suffix->string() : "", context);
	}
};

template<>
struct std::formatter<common_good::rfc6838::parameter_name> : public std::formatter<std::string>
{
	constexpr auto format(const common_good::rfc6838::parameter_name& attribute, auto&& context) const
	{
		return std::formatter<std::string>::format(attribute.string(), context);
	}
};

template<>
struct std::formatter<common_good::rfc6838::parameter_value> : public std::formatter<std::string>
{
	constexpr auto format(const common_good::rfc6838::parameter_value& value, auto&& context) const
	{
		return std::formatter<std::string>::format(value.string(), context);
	}
};

template<>
struct std::formatter<common_good::media_type> : std::formatter<std::string>
{
	constexpr auto format(const common_good::media_type& media_type, auto&& context) const
	{
		return std::formatter<std::string>::format(media_type.string(), context);
	}
};

/*  backburner:

	// std::map<parameter_name, parameter_value> parameters;

	// parameters {std::move(parameters)}

	// std::map<parameter_name, parameter_value> parameters = {}

	const std::string_view input = "; charset=UTF-1;charset=UTF-2;charset=UTF-3";
	std::map<common_good::rfc6838::parameter_name, common_good::rfc6838::parameter_value> map {};

	for (const auto& p : input | std::views::split(std::string_view {";"}) | std::views::filter(std::not_fn(std::ranges::empty)))
	{
		std::println("'{}'", p | std::ranges::to<std::string>());
	}
*/