#pragma once

namespace common_good::ascii
{
	/// @brief Test if character is digit [0-9].
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_digit(const char character) noexcept -> bool { return character >= '0' and character <= '9'; }

	/// @brief Test if character is alphabetic lowercase [a-z].
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_alphabetic_lowercase(const char character) noexcept -> bool
	{
		return character >= 'a' and character <= 'z';
	}

	/// @brief Test if character is alphabetic uppercase [A-Z].
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_alphabetic_uppercase(const char character) noexcept -> bool
	{
		return character >= 'A' and character <= 'Z';
	}

	/// @brief Test if character is alphabetic uppercase or digit [A-Z0-9].
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_alphanumeric_uppercase(const char character) noexcept -> bool
	{
		return is_alphabetic_uppercase(character) or is_digit(character);
	}

	/// @brief Test if character is alphabetic lowercase or digit [a-z0-9].
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_alphanumeric_lowercase(const char character) noexcept -> bool
	{
		return is_alphabetic_lowercase(character) or is_digit(character);
	}

	/// @brief Convert character to lowercase.
	/// @param character Ascii character to convert.
	/// @return Lowercase character if input is [A-Z], otherwise input unchanged.
	[[nodiscard]] constexpr auto to_lowercase(const char character) noexcept -> char
	{
		return is_alphabetic_uppercase(character) ? character + 32 : character;
	}

	/// @brief Convert character to uppercase.
	/// @param character Ascii character to convert.
	/// @return Uppercase character if input is [a-z], otherwise input unchanged.
	[[nodiscard]] constexpr auto to_uppercase(const char character) noexcept -> char
	{
		return is_alphabetic_lowercase(character) ? character - 32 : character;
	}

	/// @brief Test if character is Null, Start of Heading, Start of Text, End of Text, End of Transmission, Enquiry, Acknowledge, Bell,
	/// Backspace, Horizontal Tabulation, Line Feed, Vertical Tabulation, Form Feed, Carriage Return, Shift Out, Shift In, Data Link Escape,
	/// Device Control One, Device Control Two, Device Control Three, Device Control Four, Negative Acknowledge, Synchronous Idle, End of
	/// Transmission Block, Cancel, End of medium, Substitute, Escape, File Separator, Group Separator, Record Separator, Unit Separator.
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_control(const char character) noexcept -> bool
	{
		return character == char {0} or (character > char {0} and character <= char {31}) or (character == char {127});
	}

	/// @brief Test if character is space, alphanumeric [a-zA-Z0-9], punctuation.
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_printable(const char character) noexcept -> bool
	{
		return character >= char {32} and character <= char {126};
	}

	/// @brief Test if character is alphanumeric [a-zA-Z0-9], punctuation.
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_graphical(const char character) noexcept -> bool
	{
		return is_printable(character) and character != ' ';
	}

	/// @brief Test if character is space or horizontal tabulation.
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_blank(const char character) noexcept -> bool { return character == ' ' or character == '\t'; }

	/// @brief Test if character is alphabetic [a-zA-Z].
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_alphabetic(const char character) noexcept -> bool
	{
		return is_alphabetic_uppercase(character) or is_alphabetic_lowercase(character);
	}

	/// @brief Test if character is alphabetic or digit [a-zA-Z0-9].
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_alphanumeric(const char character) noexcept -> bool
	{
		return is_alphabetic(character) or is_digit(character);
	}

	/// @brief Test if character is hexadecimal digit [0-9a-fA-F].
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_hexadecimal(const char character) noexcept -> bool
	{
		return is_digit(character) or (character >= 'A' and character <= 'F') or (character >= 'a' and character <= 'f');
	}

	/// @brief Test if character is space, form feed, new line, carriage return, horizontal tabulation or vertical tabulation.
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_space(const char character) noexcept -> bool
	{
		return (character >= char {9} and character <= char {13}) or (character == ' ');
	}

	/// @brief Test if character is ! " # $ % & ' ( ) * + , - . / : ; < = > ? @ [ \ ] ^ _ ` { | } ~
	/// @param character Ascii character to test.
	/// @return True if character match.
	[[nodiscard]] constexpr auto is_punctuation(const char character) noexcept -> bool
	{
		return (character >= char {33} and character <= char {47}) or (character >= char {58} and character <= char {64})
			   or (character >= char {91} and character <= char {96}) or (character >= char {123} and character <= char {126});
	}
}