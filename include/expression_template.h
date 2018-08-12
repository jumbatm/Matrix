template <typename T>
using make_collapsible_t = T&;

#define JUMBATM_MAT_OPERATOR_EXPR_TEMPLATE(NAME_, OPERATOR_)                  \
    template <typename LeftExpr, typename RightExpr>                          \
    struct NAME_ : public _expression<NAME_<LeftExpr, RightExpr>>             \
    {                                                                         \
        using value_type = decltype(typename LeftExpr::value_type{} *         \
                                    typename RightExpr::value_type{});        \
                                                                              \
        const LeftExpr lhs;                                                   \
        const RightExpr rhs;                                                  \
                                                                              \
        static_assert((LeftExpr::rows() == RightExpr::rows()                  \
                       && LeftExpr::cols() == RightExpr::cols())              \
                          || (LeftExpr::rows() == 1 && LeftExpr::rows() == 1) \
                          || (RightExpr::rows() == 1                          \
                              && RightExpr::cols() == 1),                     \
                      "Matrices must be the same size.");                     \
                                                                              \
        NAME_(const LeftExpr& left, const RightExpr& right)                   \
          : lhs(std::move(left)), rhs(std::move(right))                       \
        {                                                                     \
        }                                                                     \
                                                                              \
        value_type operator[](size_t index) const                             \
        {                                                                     \
            return lhs[index] OPERATOR_ rhs[index];                           \
        }                                                                     \
        value_type at(size_t row, size_t column)                              \
        {                                                                     \
            size_t idx = LeftExpr::convertToFlatIndex(row, column);           \
            return lhs[idx] OPERATOR_ rhs[idx];                               \
        }                                                                     \
                                                                              \
        constexpr size_t size() const                                         \
        {                                                                     \
            return rows() * cols();                                           \
        }                                                                     \
        constexpr static size_t rows()                                        \
        {                                                                     \
            return LeftExpr::rows();                                          \
        }                                                                     \
        constexpr static size_t cols()                                        \
        {                                                                     \
            return RightExpr::cols();                                         \
        }                                                                     \
    }
