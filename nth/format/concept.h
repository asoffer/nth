#ifndef NTH_FORMAT_CONCEPT_H
#define NTH_FORMAT_CONCEPT_H

namespace nth {

template <typename T, typename W, typename F>
concept formattable_with_ftadle =
    requires(W w, F f, T value) { NthFormat(w, f, value); };

template <typename T, typename W, typename F>
concept formattable_by_formatter =
    requires(W w, F f, T value) { f.format(w, value); };

template <typename T, typename W, typename F>
concept formattable_with =
    formattable_by_formatter<T, W, F> or formattable_with_ftadle<T, W, F>;

}  // namespace nth

#endif  // NTH_FORMAT_CONCEPT_H
