#pragma once
#ifndef __CONSOLE_SCRIPT_VALUE_DATA_H__
#define __CONSOLE_SCRIPT_VALUE_DATA_H__

#include "script_defines.h"

#include <string>

namespace console_script {

	class ValueData {
	public:
		ValueData();
		virtual ~ValueData();

		virtual void Assign(void *data) = 0;

		virtual void operator = (bool b);
		virtual void operator = (int i);
		virtual void operator = (Float d);
		virtual void operator = (const String& str);

		virtual void operator = (const ValueData& data) = 0;
		virtual void operator += (const ValueData& data);
		virtual void operator -= (const ValueData& data);
		virtual void operator *= (const ValueData& data);
		virtual void operator /= (const ValueData& data);
		virtual void operator %= (const ValueData& data);
		virtual void operator |= (const ValueData& data);
		virtual void operator ^= (const ValueData& data);
		virtual void operator &= (const ValueData& data);
		virtual void operator <<= (const ValueData& data);
		virtual void operator >>= (const ValueData& data);

		virtual void operator ++();
		virtual void operator --();
		virtual void operator -();

		virtual bool AsBool() const = 0;
		virtual int AsInteger() const = 0;
		virtual Float AsFloat() const = 0;
		virtual String AsString() const = 0;

		bool is_reference() const;

	protected:
		bool is_reference_; //!< is it just a reference to the outer value (like variable)
	};

	class BooleanValueData : public ValueData {
	public:
		BooleanValueData();
		BooleanValueData(bool b);
		~BooleanValueData();

		void Assign(void *data);

		void operator = (bool b);

		void operator = (const ValueData& data);

		bool AsBool() const;
		int AsInteger() const;
		Float AsFloat() const;
		String AsString() const;

	private:
		bool *data_;
	};

	class IntegerValueData : public ValueData {
	public:
		IntegerValueData();
		IntegerValueData(int i);
		~IntegerValueData();

		void Assign(void *data);

		void operator = (int i);

		void operator = (const ValueData& data);
		void operator += (const ValueData& data);
		void operator -= (const ValueData& data);
		void operator *= (const ValueData& data);
		void operator /= (const ValueData& data);
		void operator %= (const ValueData& data);
		void operator |= (const ValueData& data);
		void operator ^= (const ValueData& data);
		void operator &= (const ValueData& data);
		void operator <<= (const ValueData& data);
		void operator >>= (const ValueData& data);

		void operator ++();
		void operator --();
		void operator -();

		bool AsBool() const;
		int AsInteger() const;
		Float AsFloat() const;
		String AsString() const;

	private:
		int *data_;
	};

	class FloatValueData : public ValueData {
	public:
		FloatValueData();
		FloatValueData(Float d);
		~FloatValueData();

		void Assign(void *data);

		void operator = (Float d);

		void operator = (const ValueData& data);
		void operator += (const ValueData& data);
		void operator -= (const ValueData& data);
		void operator *= (const ValueData& data);
		void operator /= (const ValueData& data);
		void operator -();

		bool AsBool() const;
		int AsInteger() const;
		Float AsFloat() const;
		String AsString() const;

	private:
		Float *data_;
	};

	class StringValueData : public ValueData {
	public:
		StringValueData();
		StringValueData(const String& str);
		~StringValueData();

		void Assign(void *data);

		void operator = (const String& str);

		void operator = (const ValueData& data);
		void operator += (const ValueData& data);

		bool AsBool() const;
		int AsInteger() const;
		Float AsFloat() const;
		String AsString() const;

	private:
		String *data_;
	};

} // namespace console_script

#endif