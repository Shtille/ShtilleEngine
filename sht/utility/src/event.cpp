#include "../include/event.h"

#include <assert.h>

namespace sht {
	namespace utility {

		Event::Event(StringId type)
		: type_(type)
		, num_args_(0)
		{

		}
		Event::~Event()
		{

		}
		const StringId Event::type() const
		{
			return type_;
		}
		StringId Event::type()
		{
			return type_;
		}
		const int Event::GetValueInteger(StringId key) const
		{
			for (u32 i = 0; i < num_args_; ++i)
			{
				const KeyValuePair & pair = args_[i];
				if (pair.key == key)
				{
					assert(pair.value.type == Value::kInteger);
					return pair.value.as_integer;
				}
			}
			assert(!"Haven't found event key");
			return 0;
		}
		const float Event::GetValueFloat(StringId key) const
		{
			for (u32 i = 0; i < num_args_; ++i)
			{
				const KeyValuePair & pair = args_[i];
				if (pair.key == key)
				{
					assert(pair.value.type == Value::kFloat);
					return pair.value.as_float;
				}
			}
			assert(!"Haven't found event key");
			return 0.0f;
		}
		const bool Event::GetValueBool(StringId key) const
		{
			for (u32 i = 0; i < num_args_; ++i)
			{
				const KeyValuePair & pair = args_[i];
				if (pair.key == key)
				{
					assert(pair.value.type == Value::kBoolean);
					return pair.value.as_bool;
				}
			}
			assert(!"Haven't found event key");
			return false;
		}
		const StringId Event::GetValueStringId(StringId key) const
		{
			for (u32 i = 0; i < num_args_; ++i)
			{
				const KeyValuePair & pair = args_[i];
				if (pair.key == key)
				{
					assert(pair.value.type == Value::kStringId);
					return pair.value.as_string_id;
				}
			}
			assert(!"Haven't found event key");
			return 0;
		}
		void Event::ResetArgs()
		{
			num_args_ = 0;
		}
		void Event::AddArg(StringId key, int value)
		{
			if (num_args_ < kMaxArgs)
			{
				KeyValuePair & pair = args_[num_args_];
				pair.key = key;
				pair.value.Set(value);
				++num_args_;
			}
			else
				assert(!"Need larger args buffer");
		}
		void Event::AddArg(StringId key, float value)
		{
			if (num_args_ < kMaxArgs)
			{
				KeyValuePair & pair = args_[num_args_];
				pair.key = key;
				pair.value.Set(value);
				++num_args_;
			}
			else
				assert(!"Need larger args buffer");
		}
		void Event::AddArg(StringId key, bool value)
		{
			if (num_args_ < kMaxArgs)
			{
				KeyValuePair & pair = args_[num_args_];
				pair.key = key;
				pair.value.Set(value);
				++num_args_;
			}
			else
				assert(!"Need larger args buffer");
		}
		void Event::AddArg(StringId key, StringId value)
		{
			if (num_args_ < kMaxArgs)
			{
				KeyValuePair & pair = args_[num_args_];
				pair.key = key;
				pair.value.Set(value);
				++num_args_;
			}
			else
				assert(!"Need larger args buffer");
		}

	} // namespace utility
} // namespace sht