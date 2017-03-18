#pragma once
#ifndef __CONSOLE_SCRIPT_BASE_H__
#define __CONSOLE_SCRIPT_BASE_H__

#include "script_lexem.h"
#include "script_node.h"
#include "variant.h" // using own variant class, since standart one is from C++17

#include <unordered_map>
#include <vector>
#include <memory> // for unique_ptr
#include <utility> // for std::index_sequence, requires C++14

namespace console_script {

	const int kLowestPriority = 0;

	typedef void (*OperatorPtr)(const std::list<Node*>& list, Value* value);
	typedef console_script::variant<bool, int, Float, String> Variant;

	struct MatchInfo {
		int count;
		bool full_match;

		void clear() {
			count = 0;
			full_match = false;
		}
	};

	class OperatorInfo {
		friend class Base;

	public:
		OperatorInfo() {}

		int priority() { return priority_; }
		Operator::Type type() { return type_; }
		int form() { return form_; }
		int value_types() { return value_types_; }
		Value::Type return_type() { return return_type_; }
		bool associativity() { return associativity_; }

	private:
		int priority_;			//!< operation priority
		int form_;				//!< form flag
		int value_types_;		//!< allowed value types flag
		Value::Type return_type_; //!< return value type (if kAll then use same value type)
		bool associativity_;	//!< is left-to-right (right-to-left otherwise)
		Operator::Type type_;	//!< operation type
	};

	class VariableInfo {
		friend class Base;

	public:
		VariableInfo() {}
		VariableInfo(void * ptr, Value::Type type) :
			ptr_(ptr), type_(type)
		{}

		void * ptr() { return ptr_; }
		Value::Type type() { return type_; }
	private:
		void * ptr_;
		Value::Type type_;
	};

	template <typename R, typename... Args>
	class FunctionCaller {
	public:
		template <std::size_t ... Is>
		static void Call(Value* ret, std::vector<Variant>& args_vec, std::index_sequence<Is...>, R(*f)(Args...)) {
			*ret = f(args_vec.at(Is).get<Args>()...);
		}
	};
	template <typename... Args>
	class FunctionCaller <void, Args...> {
	public:
		template <std::size_t ... Is>
		static void Call(Value* ret, std::vector<Variant>& args_vec, std::index_sequence<Is...>, void(*f)(Args...)) {
			f(args_vec.at(Is).get<Args>()...);
		}
	};

	template <typename R, typename C, typename... Args>
	class ClassFunctionCaller {
	public:
		template <std::size_t ... Is>
		static void Call(Value* ret, std::vector<Variant>& args_vec, std::index_sequence<Is...>, R(C::*f)(Args...), C * obj) {
			*ret = (obj->*f)(args_vec.at(Is).get<Args>()...);
		}
	};
	template <typename C, typename... Args>
	class ClassFunctionCaller <void, C, Args...> {
	public:
		template <std::size_t ... Is>
		static void Call(Value* ret, std::vector<Variant>& args_vec, std::index_sequence<Is...>, void(C::*f)(Args...), C * obj) {
			(obj->*f)(args_vec.at(Is).get<Args>()...);
		}
	};

	template <typename R, typename...Args>
	class FunctionTypeObtainer {
	public:
		static void Get(Value::Type &return_type, std::vector<Value::Type> &arguments_type) {
			ValueTypeSetter<R>::Fill(return_type);
			arguments_type.resize(sizeof...(Args));
			for (size_t i = 0; i < sizeof...(Args); ++i)
			{
				ObtainType(i, arguments_type[i]);
			}
		}
	private:
		template <size_t n>
		static void ObtainTypeImpl(size_t i, Value::Type& t)
		{
			if (i == n)
				ValueTypeSetter<typename std::tuple_element<n, std::tuple<Args...> >::type>::Fill(t);
			else if (n == sizeof...(Args)-1)
				throw std::out_of_range("Tuple element out of range.");
			else
				ObtainTypeImpl<(n < sizeof...(Args)-1 ? n + 1 : 0)>(i, t);
		}
		static void ObtainType(size_t i, Value::Type& t)
		{
			ObtainTypeImpl<0>(i, t);
		}
	};
	template <typename R>
	class FunctionTypeObtainer <R> {
	public:
		static void Get(Value::Type &return_type, std::vector<Value::Type> &arguments_type) {
			ValueTypeSetter<R>::Fill(return_type);
		}
	};

	class BaseFunc {
	public:
		virtual ~BaseFunc() = default;

		virtual void Call(Value* ret, std::vector<Variant>& args_vec) const = 0;
	};

	template <typename R, typename... Args>
	class Function : public BaseFunc
	{
	public:
		Function(R(*f)(Args...)) : f(f) {}
		void Call(Value* ret, std::vector<Variant>& args_vec) const override
		{
			FunctionCaller<R, Args...>::Call(ret, args_vec, std::make_index_sequence<sizeof...(Args)>{}, f);
		}

	private:
		R(*f)(Args...);
	};

	template <typename R, typename C, typename... Args>
	class ClassFunction : public BaseFunc
	{
	public:
		ClassFunction(R(C::*f)(Args...), C * object) : f(f), object(object) {}
		void Call(Value* ret, std::vector<Variant>& args_vec) const override
		{
			ClassFunctionCaller<R, C, Args...>::Call(ret, args_vec, std::make_index_sequence<sizeof...(Args)>{}, f, object);
		}

	private:
		R(C::*f)(Args...);
		C * object;
	};

	class FunctionInfo {
		friend class Base;
	public:
		Value::Type& return_type() { return return_type_; }
		std::vector<Value::Type>& arguments_type() { return arguments_type_; }
	private:
		std::unique_ptr<BaseFunc> func_;
		Value::Type return_type_;
		std::vector<Value::Type> arguments_type_;
	};

	class Base {
		typedef std::unordered_map<String, OperatorInfo> OperatorInfoMap;
		typedef std::unordered_map<Operator::Type, OperatorPtr> OperatorMap;
		typedef std::unordered_map<String, FunctionInfo> FunctionMap;
		typedef std::unordered_map<String, VariableInfo> VariableMap;
		// Iteration thru hash map is very slow, so I've decided to use another container
		typedef std::vector<String> OperatorList;

	public:
		static void AddInstance();
		static void RemoveInstance();
		static Base& GetInstance();

		void CountOperatorMatches(const String& str, MatchInfo& match);
		bool OperatorExists(const String& str);
		bool FunctionExists(const String& str);
		bool VariableExists(const String& str);

		void AddVariable(const String& str, void* ptr, Value::Type type);

		template <typename R, typename... Args>
		void AddFunction(const String& str, R(*f)(Args...)) {
			FunctionInfo& info = function_ptrs_[str];
			info.func_ = std::make_unique< Function<R, Args...> >(f);
			FunctionTypeObtainer<R, Args...>::Get(info.return_type_, info.arguments_type_);
		}
		template <typename R, typename C, typename... Args>
		void AddClassFunction(const String& str, R(C::*f)(Args...), C * object) {
			FunctionInfo& info = function_ptrs_[str];
			info.func_ = std::make_unique< ClassFunction<R, C, Args...> >(f, object);
			FunctionTypeObtainer<R, Args...>::Get(info.return_type_, info.arguments_type_);
		}
		void CallFunction(const String& func_name, std::vector<Variant>& args_vec, Value* ret);

		OperatorInfo* GetOperatorInfo(const String& str);
		VariableInfo* GetVariableInfo(const String& str);
		FunctionInfo* GetFunctionInfo(const String& str);

		OperatorPtr GetOperatorPtr(Operator::Type type);

	protected:
		void FillOperatorsInfo();
		void FillOperatorPtrs();
		void AddOperatorInfo(const String& str, int priority, Operator::Type type, int value_types, 
			int form = Operator::kBinary, Value::Type return_type = Value::kAll, bool associativity = true);

	private:
		Base();
		~Base() = default;
		// Don't allow to copy
		Base(const Base&);
		void operator =(const Base&);

		static Base * instance_;

		OperatorInfoMap operators_info_;
		OperatorList operator_list_;
		OperatorMap operator_ptrs_;
		FunctionMap function_ptrs_;
		VariableMap variable_ptrs_;
	};

} // namespace console_script

#endif