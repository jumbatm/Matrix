#define JUMBATM_MAT_OPERATOR_EXPR_TEMPLATE(NAME_, OPERATOR_)                   \
    template <typename LeftExpr, typename RightExpr>                        \
    struct NAME_;                                                           \
                                                                            \
    template <template <class, size_t, size_t> typename LeftExpr,           \
              template <class, size_t, size_t> typename RightExpr,          \
              size_t LeftRows, size_t LeftColumns, size_t RightRows,        \
              size_t RightColumns, typename LeftType, typename RightType>   \
    struct NAME_<LeftExpr<LeftType, LeftRows, LeftColumns>,         \
                         RightExpr<RightType, RightRows, RightColumns>>     \
        : public _expression<                                               \
              NAME_<LeftExpr<LeftType, LeftRows, LeftColumns>,              \
                    RightExpr<RightType, RightRows, RightColumns>>>         \
    {                                                                       \
        static_assert(LeftRows == RightRows && LeftColumns == RightColumns, \
                      "Matrices must be the same size.");                   \
                                                                            \
        using value_type = decltype(LeftType{} * RightType{});              \
                                                                            \
        using left_type = LeftExpr<LeftType, LeftRows, LeftColumns>;        \
        using right_type = RightExpr<RightType, RightRows, RightColumns>;   \
                                                                            \
        const left_type &lhs;                                               \
        const right_type &rhs;                                              \
                                                                            \
        NAME_(const left_type &left, const right_type &right)               \
            : lhs(left), rhs(right)                                         \
        {                                                                   \
        }                                                                   \
                                                                            \
        value_type operator[](size_t index) const                           \
        {                                                                   \
            return lhs[index] OPERATOR_ rhs[index];                         \
        }                                                                   \
        value_type at(size_t row, size_t column)                            \
        {                                                                   \
            size_t idx = left_type::convertToFlatIndex(row, column);        \
            return lhs[idx] OPERATOR_ rhs[idx];                             \
        }                                                                   \
                                                                            \
        constexpr size_t size() const { return LeftRows * LeftColumns; }    \
    }
