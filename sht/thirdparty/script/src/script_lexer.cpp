#include "script.h"
#include "script_base.h"

#include <cstring>

namespace {
	const CS_CHAR * kTrueString = CS_TEXT("true");
	const CS_CHAR * kFalseString = CS_TEXT("false");
}

namespace console_script {

	static bool partial_match_bool(const String& str)
	{
#ifndef PARSER_WIDE_STRING
		return strstr(kTrueString, str.data()) == kTrueString || strstr(kFalseString, str.data()) == kFalseString;
#else
		return wcsstr(kTrueString, str.data()) == kTrueString || wcsstr(kFalseString, str.data()) == kFalseString;
#endif
	}
	static bool full_match_bool(const String& str)
	{
#ifndef PARSER_WIDE_STRING
		return strcmp(kTrueString, str.data()) == 0 || strcmp(kFalseString, str.data()) == 0;
#else
		return wcscmp(kTrueString, str.data()) == 0 || wcscmp(kFalseString, str.data()) == 0;
#endif
	}
	static bool match_int(const String& str)
	{
		const CS_CHAR * p = str.c_str();
		const CS_CHAR * s = p;
		bool is_match = true;
		while (is_match && *p != CS_TEXT('\0'))
		{
			if (s == p) // first symbol
			{
				if (!(*p == CS_TEXT('+') || *p == CS_TEXT('-') || (*p >= CS_TEXT('0') && *p <= CS_TEXT('9'))))
					is_match = false;
			}
			else // not first
				if (!(*p >= CS_TEXT('0') && *p <= CS_TEXT('9')))
					is_match = false;
			++p;
		}
		return is_match;
	}
	static bool match_float(const String& str)
	{
		const CS_CHAR * p = str.c_str();
		const CS_CHAR * s = p;
		bool is_match = true;
		bool has_dot = false;
		bool has_exp = false;
		bool exp_before = false;
		bool last_digit = false;
		while (is_match && *p != CS_TEXT('\0'))
		{
			bool is_digit = (*p >= CS_TEXT('0') && *p <= CS_TEXT('9'));
			if (s == p) // first symbol
			{
				if (!(is_digit || *p == CS_TEXT('+') || *p == CS_TEXT('-')))
					is_match = false;
			}
			else // not first
			{
				if (is_digit)
				{
					if (exp_before)
						exp_before = false;
				}
				else // not digit
				{
					if (*p == CS_TEXT('.')) // dot
					{
						if (!(!has_dot && last_digit)) // dot is only allowed after digit
							is_match = false;
						has_dot = true;
					}
					else if (*p == CS_TEXT('e') || *p == CS_TEXT('E')) // exponent
					{
						if (!(!has_exp && last_digit)) // exp is only allowed after digit
							is_match = false;
						has_exp = true;
						exp_before = true;
					}
					else
					{
						if (!(exp_before && (*p == CS_TEXT('+') || *p == CS_TEXT('-')))) // 1 sign symbol is allowed after E
						{
							exp_before = false;
							is_match = false;
						}
					}
				}
			}
			last_digit = is_digit;
			++p;
		}
		return is_match;
	}
	static bool match_string(const String& str)
	{
		const CS_CHAR * p = str.c_str();
		const CS_CHAR * s = p;
		bool is_match = true;
		bool has_ending_quote = false;
		while (is_match && *p != CS_TEXT('\0'))
		{
			if (s == p) // first symbol
			{
				if (*p != CS_TEXT('\"'))
					is_match = false;
			}
			else // not first
			{
				if (has_ending_quote)
				{
					is_match = false;
				}
				else
				{
					if (*p == CS_TEXT('\"'))
						has_ending_quote = true;
				}
			}
			++p;
		}
		return is_match;
	}
	static bool partial_match_constant(const String& str)
	{
		return match_int(str) || match_float(str) || match_string(str) || partial_match_bool(str);
	}
	static bool full_match_constant(const String& str)
	{
		return match_int(str) || match_float(str) || match_string(str) || full_match_bool(str);
	}
	static bool match_variable(const String& str)
	{
		const CS_CHAR * p = str.c_str();
		const CS_CHAR * s = p;
		bool is_match = true;
		bool has_number = false;
		while (is_match && *p != CS_TEXT('\0'))
		{
			bool is_alpha = (*p >= CS_TEXT('a') && *p <= CS_TEXT('z')) || 
							(*p >= CS_TEXT('A') && *p <= CS_TEXT('Z')) ||
							(*p == CS_TEXT('_'));
			if (s == p) // first symbol
			{
				if (!is_alpha)
					is_match = false;
			}
			else // not first
			{
				bool is_digit = (*p >= CS_TEXT('0') && *p <= CS_TEXT('9'));
				has_number = has_number || is_digit;
				if ((has_number && is_alpha) || (has_number && !is_digit) || !(has_number || is_alpha))
					is_match = false;
			}
			++p;
		}
		return is_match;
	}

	bool Parser::ParseLexems(const String& str)
	{
		Base& base = Base::GetInstance();

		int i_pos = 0;
		for (String::size_type i = 0; i < str.size(); ++i)
		{
			if (str[i] == CS_TEXT('\0')) // assume 0 symbol as the end of input
				break;
			switch (str[i])
			{
			case CS_TEXT(' '): // skip spaces
			case CS_TEXT('\t'):
				continue;
			case CS_TEXT('\r'): // new line symbols
			case CS_TEXT('\n'):
				continue;
			}

			String new_str;
			new_str += str[i]; // just 1 symbol

			// Check for operators match
			MatchInfo new_info;
			base.CountOperatorMatches(new_str, new_info);
			if (new_info.count > 1)
			{
				String cur_str = new_str;
				// try to find the only match further
				bool try_other = false;
				bool was_full_match = new_info.full_match;
				String last_match_str = cur_str;
				String::size_type last_match_j = i;
				MatchInfo info;
				bool pushed = false;
				for (String::size_type j = i + 1; j < str.size(); ++j)
				{
					new_str += str[j];
					base.CountOperatorMatches(new_str, info);
					if (info.count > 1) // its still possible
					{
						if (info.full_match)
						{
							was_full_match = true;
							last_match_str = new_str;
							last_match_j = j;
						}
						continue;
					}
					else if (info.count == 1) // 1 match
					{
						if (info.full_match) // found the only match
						{
							pushed = true;
							AddLexem(new_str, Lexem::Type::kOperator, ++i_pos);
							i = j;
							break;
						}
						else
							continue;
					}
					else // 0 matches
					{
						if (was_full_match) // we still have full match
						{
							pushed = true;
							AddLexem(last_match_str, Lexem::Type::kOperator, ++i_pos);
							i = last_match_j;
							break;
						}
						else // weird situation, try other naming rules (or error?)
						{
							try_other = true;
							new_str = cur_str;
							break;
						}
					}
				}
				if (!pushed && !try_other) // end of the string
				{
					AddLexem(new_str, Lexem::Type::kOperator, ++i_pos);
					i = str.size() - 1;
				}
				if (!try_other)
					continue; // there wont be any other type (variable, function, constant)
			}
			else if (new_info.count == 1) // found the only match
			{
				bool try_other = false;
				if (new_info.full_match) // match has been found
				{
					AddLexem(new_str, Lexem::Type::kOperator, ++i_pos);
				}
				else // try to find it further
				{
					String cur_str = new_str;
					MatchInfo info;
					bool pushed = false;
					for (String::size_type j = i + 1; j < str.size(); ++j)
					{
						new_str += str[j];
						base.CountOperatorMatches(new_str, info);
						if (info.count == 1) // still 1 match
						{
							if (info.full_match)
							{
								pushed = true;
								AddLexem(new_str, Lexem::Type::kOperator, ++i_pos);
								i = j;
								break;
							}
							else
								continue;
						}
						else // 0 matches
						{
							try_other = true;
							new_str = cur_str;
							break;
						}
					}
					if (!pushed && !try_other) // end of the string
					{
						AddLexem(new_str, Lexem::Type::kOperator, ++i_pos);
					}
				}
				if (!try_other)
					continue;
			}
			// Variable/Function (same naming rule)
			if (match_variable(new_str))
			{
				bool pushed = false;
				for (String::size_type j = i + 1; j < str.size(); ++j)
				{
					new_str += str[j];
					if (match_variable(new_str))
					{
						continue;
					}
					else
					{
						pushed = true;
						new_str.pop_back();
						if (full_match_bool(new_str))
							AddLexem(new_str, Lexem::Type::kConstant, ++i_pos);
						else
							AddLexem(new_str, Lexem::Type::kUnprocessed, ++i_pos);
						i = j - 1;
						break;
					}
				}
				if (!pushed) // end of the string
				{
					AddLexem(new_str, Lexem::Type::kUnprocessed, ++i_pos);
					i = str.size() - 1;
				}
				continue;
			}
			// Constant
			if (partial_match_constant(new_str))
			{
				bool pushed = false;
				for (String::size_type j = i + 1; j < str.size(); ++j)
				{
					new_str += str[j];
					if (partial_match_constant(new_str))
					{
						continue;
					}
					else
					{
						new_str.pop_back();
						if (full_match_constant(new_str))
						{
							pushed = true;
							AddLexem(new_str, Lexem::Type::kConstant, ++i_pos);
							i = j - 1;
						}
						break;
					}
				}
				if (!pushed) // end of the string
				{
					if (full_match_constant(new_str))
						AddLexem(new_str, Lexem::Type::kConstant, ++i_pos);
					else
						AddLexem(new_str, Lexem::Type::kUnprocessed, ++i_pos);
					i = str.size() - 1;
				}
				continue;
			}
			// If we are here, there is a syntax error
			return false;
		} // for
		return true;
	}

} // namespace script