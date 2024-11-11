#pragma once

/*
 * parser.hh - v1.0.0 - Opinionated Queer License v1.2 - https://github.com/cathyprime/cat_library
 *
 * Author: Magdalena Jaroszyńska
 * Last Edited: 11-11-2024
 */

#include <vector>
#include <string_view>

namespace cat { namespace cmdline_parse {

struct Option {
    const char *m_short;
    const char *m_long;
    bool takes_arg;
};

struct Result {
    Option *option;
    std::string_view arg;
};

std::vector<Result> parse(std::vector<Option>*, int, char**);

}} // namespace cat

#define shift(xs, xs_sz) ((xs_sz)--, *(xs)++)
#define NULL_RESULT_1(x) cat::cmdline_parse::Result{.option = nullptr, .arg = x}
#define NULL_RESULT_2(x, y) cat::cmdline_parse::Result{.option = nullptr, .arg = std::string_view(x, y)}
#define GET_NULL_RESULT_MACRO(_1, _2, NAME, ...) NAME
#define NULL_RESULT(...) GET_NULL_RESULT_MACRO(__VA_ARGS__, NULL_RESULT_2, NULL_RESULT_1)(__VA_ARGS__)

namespace {

enum class Dashiness {
    Zero = 0,
    One,
    Two,
};

Dashiness dashiness(std::string_view str)
{
    if (str[0] != '-') return Dashiness::Zero;
    if (str[1] != '-') return Dashiness::One;
    return Dashiness::Two;
}

cat::cmdline_parse::Result zero_dash(std::string_view str)
{
    return NULL_RESULT(str);
}

cat::cmdline_parse::Option *get_option_short(char ch, std::vector<cat::cmdline_parse::Option> *options)
{
    for (size_t x = 0; x < options->size(); ++x) {
        cat::cmdline_parse::Option* opt = &(*options)[x];
        if (opt->m_short == nullptr) continue;
        std::string_view str = opt->m_short;

        if (str.find(ch) != -1) {
            return opt;
        }
    }
    return nullptr;
}

std::vector<cat::cmdline_parse::Result> one_dash(std::string_view str, int *argc, char ***argv,
                     std::vector<cat::cmdline_parse::Option> *options)
{
    std::string_view s = str.substr(1, str.size());
    std::vector<cat::cmdline_parse::Result> r;

    for (size_t x = 0; x < s.size(); ++x) {
        auto opt = get_option_short(s[x], options);
        if (opt == nullptr) {
            r.push_back(NULL_RESULT(&s[x], 1));
            continue;
        }

        if (opt->takes_arg) {
            if (x == s.size() - 1) {
                r.push_back(cat::cmdline_parse::Result{
                        .option = opt,
                        .arg = shift(*argv, *argc)
                        });
                continue;
            }
            r.push_back(NULL_RESULT(&s[x], 1));
            continue;
        }

        r.push_back(cat::cmdline_parse::Result{
                .option = opt,
                .arg = ""
                });

    }

    return r;
}

cat::cmdline_parse::Result two_dash(std::string_view str, int *argc, char ***argv,
                     std::vector<cat::cmdline_parse::Option> *options)
{
    std::string_view s;
    size_t equal = str.find('=');

    if (equal != -1) s = str.substr(2, equal-2);
    else s = str.substr(2, str.size());

    cat::cmdline_parse::Option* opt;
    for (size_t x = 0; x < options->size(); ++x) {
        cat::cmdline_parse::Option* current_opt = &(*options)[x];
        if (current_opt->m_long == nullptr) continue;

        std::string_view long_str = current_opt->m_long;
        if (long_str.find(s) != -1) {
            opt = current_opt;
            break;
        }
    }

    cat::cmdline_parse::Result result = {
        .option = opt,
        .arg = ""
    };

    if (opt->takes_arg) {
        if (equal != -1) {
            result.arg = str.data() + equal + 1;
        } else {
            result.arg = shift(*argv, *argc);
        }
    }

    return result;
}

} // anonymous namespace

inline std::vector<cat::cmdline_parse::Result> cat::cmdline_parse::parse(std::vector<Option> *options, int argc, char **argv)
{
    std::vector<cat::cmdline_parse::Result> results;
    results.reserve(options->size());

    while (argc > 0) {
        std::string_view str = shift(argv, argc);
        switch(dashiness(str)) {
            case Dashiness::Zero:
                results.push_back(zero_dash(str));
                break;
            case Dashiness::One: {
                std::vector<cat::cmdline_parse::Result> r = one_dash(str, &argc, &argv, options);
                results.insert(
                        results.end(),
                        std::make_move_iterator(r.begin()),
                        std::make_move_iterator(r.end()));
            } break;
            case Dashiness::Two:
                results.push_back(two_dash(str, &argc, &argv, options));
                break;
        }
    }

    return results;
}

#undef shift
#undef NULL_RESULT_1
#undef NULL_RESULT_2
#undef GET_NULL_RESULT_MACRO
#undef NULL_RESULT
