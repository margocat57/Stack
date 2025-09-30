//!@file
#ifndef MISTAKES_CODE_H
#define MISTAKES_CODE_H

//! Mistakes at verification
enum stack_err_t {
    NO_MISTAKE              = 0,
    NULL_STACK_PTR          = 1 << 0,
    NULL_STACK_ELEM         = 1 << 1,
    CANARY_ST_NOT_IN_PLACES = 1 << 2,
    PTR_SMALLER_THAN_DATA   = 1 << 3,
    PTR_BIGGER_THAN_DATA    = 1 << 4,
    ALIGN_NOT_CORRECT       = 1 << 5,
    STACK_HASH_NOT_CORRECT  = 1 << 6,
    DATA_HASH_NOT_CORRECT   = 1 << 7,
    CANARY_DT_NOT_IN_PLACES = 1 << 8
};

//! Mistakes at function's work
enum func_param_err_t{
    NO_MISTAKE_FUNC         = 0,
    FUNC_PARAM_IS_NULL      = 1 << 0, //!< Parametr given to function is NULL
    VERIFY_FAILED           = 1 << 1, //!< Stack verification failed
    ALLOC_ERROR             = 1 << 2, //!< Allocation error
    INCORR_DIGIT_PARAMS     = 1 << 3
};


#endif //MISTAKES_CODE_H