// Copyright VanstySanic. All Rights Reserved.

#pragma once


#define VS_DECLARE_HAS_GLOBAL_FUNC_IMPLICIT(FuncName)	\
template<typename R, typename... Args>	\
struct FVSHasGlobalFunc_##FuncName##_Implicit {	\
	private:	\
		template<typename U>	\
		static auto Test(int) -> decltype(	\
			foo(std::declval<Args>()...), std::true_type{});	\
	\
		template<typename>	\
		static std::false_type Test(...);	\
	public:	\
		static constexpr bool Value = decltype(Test<void>(0))::value;	\
};	\

#define VS_DECLARE_HAS_GLOBAL_FUNC_EXACT(FuncName)	\
template<typename R, typename... Args>	\
struct FVSHasGlobalFunc_##FuncName##_Exact {	\
	private:	\
		template<typename U>	\
		static auto Test(int) -> std::enable_if_t<	\
			std::is_same_v<R, decltype(foo(std::declval<Args>()...))>, std::true_type>;	\
	\
		template<typename>	\
		static std::false_type Test(...);	\
	public:	\
		static constexpr bool Value = decltype(Test<void>(0))::value;	\
};	\

#define VS_DECLARE_HAS_METHOD_IMPLICIT(FuncName)	\
template<typename T, bool IsConst, typename... Args>	\
struct FVSHasMethod_##FuncName##Implicit {	\
	private:	\
		template<typename U>	\
		static auto Test(int) -> decltype(	\
			std::declval<typename std::conditional<IsConst,	\
				const U,	\
				U>::type>().FuncName(std::declval<Args>()...), std::true_type{});	\
	\
		template<typename>	\
		static std::false_type Test(...);	\
	public:	\
		static constexpr bool Value = decltype(Test<T>(0))::value;	\
};		\

#define VS_DECLARE_HAS_METHOD_EXACT(FuncName)	\
template<typename T, typename R, bool IsConst, typename... Args>	\
struct FVSHasMethod_##FuncName##Exact {	\
	private:	\
		template<typename U>	\
		static auto Test(int) -> decltype(	\
			static_cast<	\
				typename std::conditional<IsConst,	\
					typename VS::MethodFuncExistence::MemberFunctionPointerHelper<U, R, Args...>::Const,	\
					typename VS::MethodFuncExistence::MemberFunctionPointerHelper<U, R, Args...>::NonConst	\
				>::type	\
			>(&U::FuncName), std::true_type{});	\
	\
		template<typename>	\
		static std::false_type Test(...);	\
	public:	\
		static constexpr bool Value = decltype(Test<T>(0))::value;	\
};	\


#define VS_HAS_GLOBAL_FUNC_IMPLICIT(FuncName, ...)	\
	FVSHasGlobalFunc_##FuncName##_Implicit<void, __VA_ARGS__>::Value

#define VS_HAS_GLOBAL_FUNC_EXACT(FuncName, R, ...)	\
	FVSHasGlobalFunc_##FuncName##_Exact<R, __VA_ARGS__>::Value

#define VS_HAS_METHOD_IMPLICIT(T, FuncName, IsConst, ...)	\
	FVSHasMethod_##FuncName##Implicit<T, IsConst, __VA_ARGS__>::Value

#define VS_HAS_METHOD_EXACT(T, FuncName, R, IsConst, ...)	\
	FVSHasMethod_##FuncName##Exact<T, R, IsConst, __VA_ARGS__>::Value


namespace VS
{
	namespace MethodFuncExistence
	{
		template<typename T, typename R, typename... Args>
		struct FMemberFunctionPointerHelper {
			using NonConst = R(T::*)(Args...);
			using Const = R(T::*)(Args...) const;
		};
	}
}
