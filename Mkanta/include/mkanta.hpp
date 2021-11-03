#pragma once
#include <memory>
#include <unordered_map>
#include <string_view>
#include <utility>

namespace mkanta::detail
{
    // Custom Your Project

#ifdef MKANTA_CHARTYPE
    using char_type = MKANTA_CHARTYPE;
#else
    using char_type = char;
#endif 

    using string_type = std::basic_string<char_type>;
    using string_view = std::basic_string_view<char_type>;
}

#define MKANTA_FIX_CHARTYPE_PREFIX_2(name)\
[] {\
    if constexpr(std::same_as<::mkanta::detail::char_type, char8_t>) {\
        return u8""#name;\
    } else if constexpr(std::same_as<::mkanta::detail::char_type, char16_t>) {\
        return u""#name;\
    } else if constexpr(std::same_as<::mkanta::detail::char_type, char32_t>) {\
        return U""#name;\
    } else if constexpr(std::same_as<::mkanta::detail::char_type, wchar_t>) {\
        return L""#name;\
    } else {\
        return #name; \
    }\
}()
#define MKANTA_FIX_CHARTYPE_PREFIX(name) MKANTA_FIX_CHARTYPE_PREFIX_2(name)

namespace mkanta
{
    struct gobal_scope {};

    namespace detail
    {
        struct dummy {};

        template<class Type>
        consteval string_view signature()
        {
            if constexpr (std::same_as<char_type, char8_t>) {
                return u8"" __FUNCSIG__;
            } else if constexpr (std::same_as<char_type, char16_t>) {
                return u"" __FUNCSIG__;
            } else if constexpr (std::same_as<char_type, char32_t>) {
                return U"" __FUNCSIG__;
            } else if constexpr (std::same_as<char_type, wchar_t>) {
                return L"" __FUNCSIG__;
            } else {
                return __FUNCSIG__;
            }
        }
        template<class Type>
        consteval string_view nameof_ns()
        {
            constexpr auto sig = signature<Type>();
            constexpr auto len = sig.length();
            constexpr auto prefixLen = sig.find(MKANTA_FIX_CHARTYPE_PREFIX(mkanta::detail::signature))
                + sizeof("mkanta::detail::signature<struct ") - 1;
            constexpr auto suffixLen = sizeof(">(void)") - 1;
            return sig.substr(prefixLen, len - prefixLen - suffixLen);
        }
        template<class Type>
        consteval string_view nameof()
        {
            constexpr auto result = nameof_ns<Type>();
            return result.substr(result.find_last_of(MKANTA_FIX_CHARTYPE_PREFIX(:)) + 1);
        }

        template<size_t Line>
        struct tag
        {
        };

        template<class Type, class Name, auto FuncPointer>
        struct initializer
        {
        private:
            inline static string_view name = nameof<Name>();
            inline static string_type fullName = string_type(nameof_ns<Type>()) + string_type(MKANTA_FIX_CHARTYPE_PREFIX(::)) + string_type(name);
            inline static const detail::dummy regist = reflect<Type>::regist(name, FuncPointer);
            inline static const detail::dummy regist_global = reflect<gobal_scope>::regist(fullName, FuncPointer);
        };
    }

    template<class Type = gobal_scope>
    struct reflect
    {
        template<class PointerType>
        static detail::dummy regist(detail::string_view name, PointerType adress)
        {
            regist_internal<PointerType>(name) = adress;
            return {};
        }

        template<class PointerType>
        static PointerType find(detail::string_view name)
        {
            return regist_internal<PointerType>(name);
        }
    private:
        template<class PointerType>
        static PointerType& regist_internal(detail::string_view name)
        {
            static std::unordered_map<detail::string_view, PointerType> addresses;
            return addresses[name];
        }
    };
}

#define MKANTA_PP_IMPL_OVERLOAD(e1, e2, NAME, ...) NAME
#define MKANTA_PP_IMPL_3(name, line, type)\
]] auto _reflection_init(::mkanta::detail::tag<line>)\
{\
    return ::mkanta::detail::initializer<\
        std::decay_t<decltype(*this)>,\
        struct name,\
        static_cast<type>(&std::decay_t<decltype(*this)>::name)\
    >{};\
}\
[[
#define MKANTA_PP_IMPL_2(name, line)\
]] auto _reflection_init(::mkanta::detail::tag<line>)\
{\
    return ::mkanta::detail::initializer<\
        std::decay_t<decltype(*this)>,\
        struct name,\
        &std::decay_t<decltype(*this)>::name\
    >{};\
}\
[[
#define MKANTA_EXPAND(x) x
#define REFLECTION2(name, ...) MKANTA_EXPAND(MKANTA_PP_IMPL_OVERLOAD(__VA_ARGS__, MKANTA_PP_IMPL_3, MKANTA_PP_IMPL_2)(name, __VA_ARGS__))

#define REFLECTION(name, ...) REFLECTION2(name, __LINE__, __VA_ARGS__)
