#pragma once
#ifndef __SHT_SYSTEM_EVENT_H__
#define __SHT_SYSTEM_EVENT_H__

#include "common/types.h"
#include "string_id.h"

namespace sht {
	namespace utility {
	
		//! Event class
		class Event {

			struct Value {
				enum Type {
					kInteger,
					kFloat,
					kBoolean,
					kStringId
				};
				Type type;
				union {
					int as_integer;
					float as_float;
					bool as_bool;
					StringId as_string_id;
				};
				void Set(int value)
				{
					type = kInteger;
					as_integer = value;
				}
				void Set(float value)
				{
					type = kFloat;
					as_float = value;
				}
				void Set(bool value)
				{
					type = kBoolean;
					as_bool = value;
				}
				void Set(StringId value)
				{
					type = kStringId;
					as_string_id = value;
				}
			};
			struct KeyValuePair {
				StringId key;
				Value value;
			};

		public:

			explicit Event(StringId type);
			~Event();

			const StringId type() const;
			StringId type();

			const int GetValueInteger(StringId key) const;
			const float GetValueFloat(StringId key) const;
			const bool GetValueBool(StringId key) const;
			const StringId GetValueStringId(StringId key) const;

			void ResetArgs();
			void AddArg(StringId key, int value);
			void AddArg(StringId key, float value);
			void AddArg(StringId key, bool value);
			void AddArg(StringId key, StringId value);

		private:

			static constexpr u32 kMaxArgs = 4;
			StringId type_;
			u32 num_args_;
			KeyValuePair args_[kMaxArgs];
		};

		//! Event listener interface class
		class EventListenerInterface {
		public:
			virtual void OnEvent(const Event * event);
		};
	
	} // namespace utility
} // namespace sht

#endif