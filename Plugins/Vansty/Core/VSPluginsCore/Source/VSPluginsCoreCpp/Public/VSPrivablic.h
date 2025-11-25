// Copyright VanstySanic. All Rights Reserved.

#pragma once

#define VS_DECLARE_PRIVABLIC_MEMBER(CLASS, MEMBER, TYPE) \
    struct FVSPrivablic_Member_##CLASS##_##MEMBER { using Type = TYPE CLASS::*; }; \
    template struct VS::Privablic::FPrivateMember<FVSPrivablic_Member_##CLASS##_##MEMBER, &CLASS::MEMBER>;

#define VS_DECLARE_PRIVABLIC_MEMBER_STATIC(CLASS, MEMBER, TYPE) \
    struct FVSPrivablic_Member_Static_##CLASS##_##MEMBER { typedef TYPE* Type; }; \
    template struct VS::Privablic::FPrivateMember<FVSPrivablic_Member_Static_##CLASS##_##MEMBER, &CLASS::MEMBER>;

#define VS_DECLARE_PRIVABLIC_MEMBER_ADDRESS(CLASS, MEMBER, TYPE) \
    struct FVSPrivablic_Member_Address_##CLASS##_##MEMBER { using Type = TYPE CLASS::*; }; \
    template struct VS::Privablic::FPrivateMember<FVSPrivablic_Member_Address_##CLASS##_##MEMBER, &CLASS::MEMBER>;

#define VS_DECLARE_PRIVABLIC_METHOD(CLASS, METHOD, RETURN_TYPE, ...) \
    struct FVSPrivablic_Method_##CLASS##_##METHOD { using Type = RETURN_TYPE (CLASS::*)(__VA_ARGS__); }; \
    template struct VS::Privablic::FPrivateMethod<FVSPrivablic_Method_##CLASS##_##METHOD, &CLASS::METHOD>;

#define VS_DECLARE_PRIVABLIC_METHOD_CONST(CLASS, METHOD, RETURN_TYPE, ...) \
    struct FVSPrivablic_Method_Const_##CLASS##_##METHOD { using Type = RETURN_TYPE (CLASS::*)(__VA_ARGS__) const; }; \
    template struct VS::Privablic::FPrivateMethod<FVSPrivablic_Method_Const_##CLASS##_##METHOD, &CLASS::METHOD>;

#define VS_DECLARE_PRIVABLIC_METHOD_STATIC(CLASS, METHOD, RETURN_TYPE, ...) \
    struct FVSPrivablic_Method_Static_##CLASS##_##METHOD { using Type = RETURN_TYPE (*)(__VA_ARGS__); }; \
    template struct VS::Privablic::FPrivateMethod<FVSPrivablic_Method_Static_##CLASS##_##METHOD, &CLASS::METHOD>;

#define VS_DECLARE_PRIVABLIC_METHOD_ADDRESS(CLASS, METHOD, RETURN_TYPE, ...) \
    struct FVSPrivablic_Method_Address_##CLASS##_##METHOD { using Type = RETURN_TYPE (CLASS::*)(__VA_ARGS__); }; \
    template struct VS::Privablic::FPrivateMethod<FVSPrivablic_Method_Address_##CLASS##_##METHOD, &CLASS::METHOD>;


#define VS_PRIVABLIC_MEMBER(OBJECT, CLASS, MEMBER) \
    ((OBJECT)->*VS::Privablic::FMember<FVSPrivablic_Member_##CLASS##_##MEMBER>::Value)

#define VS_PRIVABLIC_MEMBER_STATIC(CLASS, MEMBER) \
    (*VS::Privablic::FMember<FVSPrivablic_Member_Static_##CLASS##_##MEMBER>::Value)

#define VS_PRIVABLIC_MEMBER_ADDRESS(CLASS, MEMBER) \
    VS::Privablic::FMember<FVSPrivablic_Member_Address_##CLASS##_##MEMBER>::Value

#define VS_PRIVABLIC_METHOD(OBJECT, CLASS, METHOD) \
    ((OBJECT)->*VS::Privablic::FMethod<FVSPrivablic_Method_##CLASS##_##METHOD>::Ptr)

#define VS_PRIVABLIC_METHOD_CONST(OBJECT, CLASS, METHOD) \
    ((OBJECT)->*VS::Privablic::FMethod<FVSPrivablic_Method_Const_##CLASS##_##METHOD>::Ptr)

#define VS_PRIVABLIC_METHOD_STATIC(CLASS, METHOD) \
    (*VS::Privablic::FMethod<FVSPrivablic_Method_Static_##CLASS##_##METHOD>::Ptr)

#define VS_PRIVABLIC_METHOD_ADDRESS(CLASS, METHOD) \
    VS::Privablic::FMethod<FVSPrivablic_Method_Address_##CLASS##_##METHOD>::Ptr

namespace VS
{
    namespace Privablic
	{
#if __cplusplus >= 201703L
    	template <class StructClass>
    	struct FMember
    	{
    		static inline typename StructClass::Type Value{};
    	};

    	template <class StructClass, typename StructClass::Type MemberAddr>
		struct FPrivateMember
    	{
    		struct FInit
    		{
    			FInit() noexcept
    			{
    				FMember<StructClass>::Value = MemberAddr;
    			}
    		};
    		static inline FInit Instance{};
    		static constexpr typename StructClass::Type Value = MemberAddr;
    	};

    	template <typename StructClass>
		struct FMethod
    	{
    		using Type = typename StructClass::Type;
    		static inline Type Ptr{};
    	};

    	template <typename StructClass, typename StructClass::Type FuncAddr>
		struct FPrivateMethod : FMethod<StructClass>
    	{
    		struct FInit {
    			FInit() noexcept {
    				FMethod<StructClass>::Ptr = FuncAddr;
    			}
    		};
    		static inline FInit PrivateMethodObject{};
    		static constexpr typename StructClass::Type Ptr = FuncAddr;
    	};
#else
		template <class StructClass>
		struct FMember
		{
			static typename StructClass::Type Value;
		};
		template <class StructClass> 
		typename StructClass::Type FMember<StructClass>::Value;
	
		template <class StructClass, typename StructClass::Type MemberAddr>
		struct FPrivateMember
		{
			FPrivateMember() { FMember<StructClass>::Value = MemberAddr; }
			static FPrivateMember Instance;
		};
		template <class StructClass, typename StructClass::Type MemberAddr> 
		FPrivateMember<StructClass, MemberAddr> FPrivateMember<StructClass, MemberAddr>::Instance;
	
		template<typename StructClass>
		struct FMethod
		{
			typedef typename StructClass::Type Type;
			static Type Ptr;
		};
		template<typename StructClass>
		typename FMethod<StructClass>::Type FMethod<StructClass>::Ptr;
	
		template<typename StructClass, typename StructClass::Type FuncAddr>
		struct FPrivateMethod : FMethod<StructClass>
		{
			struct FPrivateMethodStruct
			{
				FPrivateMethodStruct() { FMethod<StructClass>::Ptr = FuncAddr; }
			};
			static FPrivateMethodStruct PrivateMethodObject;
		};
		template<typename StructClass, typename StructClass::Type FuncAddr>
		typename FPrivateMethod<StructClass, FuncAddr>::FPrivateMethodStruct FPrivateMethod<StructClass, FuncAddr>::PrivateMethodObject;
#endif
    }
}