#pragma once
#include <memory>
#include <unordered_map>
#include <string_view>
#include <utility>

namespace mkanta::detail
{
    // Custom Your Project

#ifdef MKANTA_BIND_MAX_LINES
    inline constexpr size_t BIND_MAX_LINES = MKANTA_BIND_MAX_LINES;
#else
    inline constexpr size_t BIND_MAX_LINES = 500;
#endif

#ifdef MKANTA_CHARTYPE
    using char_type = MKANTA_CHARTYPE;
#else
    using char_type = char;
#endif 

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
    namespace detail
    {
        struct dummy {};

        template<auto x>
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
        consteval string_view nameof()
        {
            constexpr auto sig = signature<Type>();
            constexpr auto len = sig.length();
            constexpr auto prefixLen = sig.find(MKANTA_FIX_CHARTYPE_PREFIX(mkanta::detail::signature))
                + sizeof("mkanta::detail::signature<struct ") - 1;
            constexpr auto suffixLen = sizeof(">(void)") - 1;
            return sig.substr(prefixLen, len - prefixLen - suffixLen);
        }
        template<class Type, auto x>
        consteval string_view nameof()
        {
            constexpr auto sig = signature<x>();
            constexpr auto len = sig.length();
            constexpr auto prefixLen = sig.find(MKANTA_FIX_CHARTYPE_PREFIX(mkanta::detail::signature))
                + (std::is_member_function_pointer_v<decltype(x)> ?
                sizeof("mkanta::detail::signature<void __thiscall ") :
                sizeof("mkanta::detail::signature<void __cdecl "))
                - 1;
            constexpr auto typeLen = nameof<Type>().length();
#define MKANTA_PARA (
            constexpr auto suffixLen = sig.find_first_of(MKANTA_FIX_CHARTYPE_PREFIX(MKANTA_PARA), prefixLen + typeLen);
#undef MKANTA_PARA
            return sig.substr(prefixLen, suffixLen - prefixLen);
        }

        template<size_t Line>
        struct tag
        {
        };

        template<class Type, size_t LineNum>
        concept bindable_by = requires(Type * t, tag<LineNum> l)
        {
            t | l;
        };

        template <size_t... As, size_t... Bs>
        constexpr std::index_sequence<As..., Bs...> operator+(std::index_sequence<As...>, std::index_sequence<Bs...>)
        {
            return {};
        }
        template <class Type, size_t LineNum>
        constexpr auto filter_seq()
        {
            if constexpr (bindable_by<Type, LineNum>) {
                return std::index_sequence<LineNum>{};
            } else {
                return std::index_sequence<>{};
            }
        }
        template <class Type, size_t ...Seq>
        constexpr auto make_sequence_impl(std::index_sequence<Seq...>)
        {
            return (filter_seq<Type, Seq>() + ...);
        }

        template <class Type>
        constexpr auto make_sequence()
        {
            return make_sequence_impl<Type>(std::make_index_sequence<BIND_MAX_LINES>());
        }

        template<class Type, size_t LineNum>
        void bind(Type* p)
        {
            p | tag<LineNum>{};
        }
        template<class Type, size_t ...Seq>
        void bind_all_impl(Type* p, std::index_sequence<Seq...>)
        {
            (bind<Type, Seq>(p), ...);
        }
        template<class Type>
        void bind_all(Type* p)
        {
            bind_all_impl(p, detail::make_sequence<Type>());
        }
    }

    struct gobal_scope{};

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
            constinit static std::unique_ptr<std::unordered_map<detail::string_view, PointerType>> addresses = nullptr;
            if (!addresses) {
                addresses = std::make_unique<std::unordered_map<detail::string_view, PointerType>>();
            }
            return (*addresses)[name];
        }
    };

    template<class Type>
    struct initilizer
    {
        initilizer()
        {
            [[maybe_unused]] static const bool init = [] {
                Type* p = nullptr;
                detail::bind_all(p);
                return true;
            }();
        }
    };

    template<class Type>
    inline initilizer<Type> initilizer_v;
}

#define MKANTA_PP_IMPL_OVERLOAD(e1, e2, NAME, ...) NAME
#define MKANTA_PP_IMPL_3(name, line, type)\
]] friend void operator | (auto* a, ::mkanta::detail::tag<line>)\
{\
    static_assert(line < ::mkanta::detail::BIND_MAX_LINES);\
    using this_type = std::decay_t<decltype(*a)>;\
    [[maybe_unused]]static const auto regist = ::mkanta::reflect<this_type>::regist(MKANTA_FIX_CHARTYPE_PREFIX(name), static_cast<type>(&this_type::name));\
    [[maybe_unused]]static const auto regist_global = ::mkanta::reflect<>::regist(::mkanta::detail::nameof<this_type, static_cast<type>(&this_type::name)>(), static_cast<type>(&this_type::name));\
}[[

#define MKANTA_PP_IMPL_2(name, line)\
]] friend void operator | (auto* a, ::mkanta::detail::tag<line>)\
{\
    static_assert(line < ::mkanta::detail::BIND_MAX_LINES);\
    using this_type = std::decay_t<decltype(*a)>;\
    [[maybe_unused]]static const auto regist = ::mkanta::reflect<this_type>::regist(MKANTA_FIX_CHARTYPE_PREFIX(name), &this_type::name);\
    [[maybe_unused]]static const auto regist_global = ::mkanta::reflect<>::regist(::mkanta::detail::nameof<this_type, &this_type::name>(), &this_type::name);\
}[[
#define MKANTA_EXPAND(x) x
#define REFLECTION2(name, ...) MKANTA_EXPAND(MKANTA_PP_IMPL_OVERLOAD(__VA_ARGS__, MKANTA_PP_IMPL_3, MKANTA_PP_IMPL_2)(name, __VA_ARGS__))

#define REFLECTION(name, ...) REFLECTION2(name, __LINE__, __VA_ARGS__)


#define REFLECTION_EXPORT(key, type_name) \
]] type_name; inline static const ::mkanta::initilizer<type_name> reflect_init_##type_name## = {}; key [[
#define REFLECTION_STRUCT(type_name) REFLECTION_EXPORT(struct, type_name)
#define REFLECTION_CLASS(type_name) REFLECTION_EXPORT(class, type_name)