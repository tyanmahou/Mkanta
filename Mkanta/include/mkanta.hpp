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
}

namespace mkanta
{
    namespace detail
    {
        struct dummy {};

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
        void bind_all(Type* p, std::index_sequence<Seq...>)
        {
            (bind<Type, Seq>(p), ...);
        }
    }

    template<class Type>
    struct reflect
    {
        template<class PointerType>
        static detail::dummy regist(const detail::char_type* name, PointerType adress)
        {
            regist_internal<PointerType>(name) = adress;
            return {};
        }

        template<class PointerType>
        static PointerType find(std::basic_string_view<detail::char_type> name)
        {
            [[maybe_unused]] static const bool init = [] {
                Type* p = nullptr;
                detail::bind_all(p, detail::make_sequence<Type>());
                return true;
            }();
            return regist_internal<PointerType>(name);
        }
    private:
        template<class PointerType>
        static PointerType& regist_internal(std::basic_string_view<detail::char_type> name)
        {
            constinit static std::unique_ptr<std::unordered_map<std::basic_string_view<detail::char_type>, PointerType>> addresses = nullptr;
            if (!addresses) {
                addresses = std::make_unique<std::unordered_map<std::basic_string_view<detail::char_type>, PointerType>>();
            }
            return (*addresses)[name];
        }
    };

}

#define MKANTA_FIX_CHARTYPE_PREFIX(name)\
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

#define REFLECTION2(name, line, ...)\
]] friend void operator | (auto* a, ::mkanta::detail::tag<line>)\
{\
    static_assert(line < ::mkanta::detail::BIND_MAX_LINES);\
    using this_type = std::decay_t<decltype(*a)>;\
    [[maybe_unused]]static const auto regist = ::mkanta::reflect<this_type>::regist(MKANTA_FIX_CHARTYPE_PREFIX(name), __VA_OPT__((__VA_ARGS__))&this_type::name);\
}[[

#define REFLECTION(name, ...) REFLECTION2(name, __LINE__, __VA_ARGS__)