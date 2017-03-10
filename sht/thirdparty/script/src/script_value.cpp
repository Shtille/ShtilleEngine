#include "script_value.h"
#include "script_lexem.h"
#include "script_value_data.h"
#include <assert.h>
#include <regex> // requires C++11

namespace console_script {

	Value::Value() :
		data_(nullptr), type_(kUnknown)
	{
	}
	Value::Value(const Value& value) :
		data_(nullptr), type_(kUnknown)
	{
		operator =(value);
	}
	Value::Value(Lexem* lexem) :
		data_(nullptr), type_(kUnknown)
	{
		switch (lexem->type)
		{
		case Lexem::kConstant:
			Evaluate(lexem->str);
			break;
		default:
			break;
		}
	}
	Value::~Value()
	{
		if (data_)
			delete data_;
	}
	void Value::Assign(void *data)
	{
		data_->Assign(data);
	}
	Value& Value::operator = (bool b)
	{
		assert(data_);
		*data_ = b;
		return *this;
	}
	Value& Value::operator = (int i)
	{
		assert(data_);
		*data_ = i;
		return *this;
	}
	Value& Value::operator = (Float d)
	{
		assert(data_);
		*data_ = d;
		return *this;
	}
	Value& Value::operator = (const String& str)
	{
		assert(data_);
		*data_ = str;
		return *this;
	}
	Value& Value::operator = (const Value& value)
	{
		if (!data_ || !data_->is_reference())
			set_type(value.type_);
		*data_ = *value.data_;
		return *this;
	}
	Value& Value::operator += (const Value& value)
	{
		*data_ += *value.data_;
		return *this;
	}
	Value& Value::operator -= (const Value& value)
	{
		*data_ -= *value.data_;
		return *this;
	}
	Value& Value::operator *= (const Value& value)
	{
		*data_ *= *value.data_;
		return *this;
	}
	Value& Value::operator /= (const Value& value)
	{
		*data_ /= *value.data_;
		return *this;
	}
	Value& Value::operator %= (const Value& value)
	{
		*data_ %= *value.data_;
		return *this;
	}
	Value& Value::operator |= (const Value& value)
	{
		*data_ |= *value.data_;
		return *this;
	}
	Value& Value::operator ^= (const Value& value)
	{
		*data_ ^= *value.data_;
		return *this;
	}
	Value& Value::operator &= (const Value& value)
	{
		*data_ &= *value.data_;
		return *this;
	}
	Value& Value::operator <<= (const Value& value)
	{
		*data_ <<= *value.data_;
		return *this;
	}
	Value& Value::operator >>= (const Value& value)
	{
		*data_ >>= *value.data_;
		return *this;
	}
	Value& Value::operator ++()
	{
		++(*data_);
		return *this;
	}
	Value Value::operator ++(int)
	{
		Value temp(*this);
		++(*data_);
		return temp;
	}
	Value& Value::operator --()
	{
		--(*data_);
		return *this;
	}
	Value Value::operator --(int)
	{
		Value temp(*this);
		--(*data_);
		return temp;
	}
	Value Value::operator -()
	{
		Value temp(*this);
		-*temp.data_;
		return temp;
	}
	void Value::set_type(Type type)
	{
		type_ = type;
		if (data_) delete data_;
		switch (type_)
		{
		case kBoolean:
			data_ = new BooleanValueData();
			break;
		case kInteger:
			data_ = new IntegerValueData();
			break;
		case kFloat:
			data_ = new FloatValueData();
			break;
		case kString:
			data_ = new StringValueData();
			break;
		default:
			break;
		}
	}
	Value::Type Value::type() const
	{
		return type_;
	}
	bool Value::valid() const
	{
		return data_ != nullptr;
	}
	bool Value::AsBool() const
	{
		return data_->AsBool();
	}
	int Value::AsInteger() const
	{
		return data_->AsInteger();
	}
	Float Value::AsFloat() const
	{
		return data_->AsFloat();
	}
	String Value::AsString() const
	{
		return data_->AsString();
	}
	void Value::Evaluate(const String& str)
	{
		if (FromString(str)) return;
		if (FromInteger(str)) return;
		if (FromFloat(str)) return;
		if (FromBool(str)) return;
		assert(false && "unknown constant type");
	}
	bool Value::FromString(const String& str)
	{
		if (str.size() >= 2 && str.front() == CS_TEXT('\"') && str.back() == CS_TEXT('\"'))
		{
			String s = str.substr(1, str.length() - 2);
			type_ = kString;
			data_ = new StringValueData(s);
			return true;
		}
		else
			return false;
	}
	bool Value::FromInteger(const String& str)
	{
		if (str.empty() || ((!isdigit(str[0])) && (str[0] != CS_TEXT('-')) && (str[0] != CS_TEXT('+'))))
			return false;
		CS_CHAR *p;
		int val = 
#ifndef PARSER_WIDE_STRING
		strtol
#else
		wcstol
#endif
		(str.data(), &p, 10);
		if (*p == 0)
		{
			// We do not check out of range statement
			type_ = kInteger;
			data_ = new IntegerValueData(val);
			return true;
		}
		else
			return false;
	}
	bool Value::FromFloat(const String& str)
	{
		if (str.empty())
			return false;
		CS_CHAR *p;
		Float d = static_cast<Float>(
#ifndef PARSER_WIDE_STRING
			strtod
#else
			wcstod
#endif
			(str.data(), &p));
		if (*p == 0)
		{
			// We do not check out of range statement
			type_ = kFloat;
			data_ = new FloatValueData(d);
			return true;
		}
		else
			return false;
	}
	bool Value::FromBool(const String& str)
	{
		bool b = str == CS_TEXT("true");
		if (b || str == CS_TEXT("false"))
		{
			type_ = kBoolean;
			data_ = new BooleanValueData(b);
			return true;
		}
		else
			return false;
	}
	bool Value::IsGoodFunctionName(const String& str)
	{
		static
#ifndef PARSER_WIDE_STRING
		std::regex
#else
		std::wregex
#endif
		func_regex(CS_TEXT("[a-zA-Z_][a-zA-Z0-9_]*"));
		return std::regex_match(str, func_regex);
	}
	bool Value::IsGoodVariableName(const String& str)
	{
		static
#ifndef PARSER_WIDE_STRING
		std::regex
#else
		std::wregex
#endif
		var_regex(CS_TEXT("[a-zA-Z_][a-zA-Z0-9_]*"));
		return std::regex_match(str, var_regex);
	}

} // namespace script