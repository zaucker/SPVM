#include "spvm_native.h"
#include "spvm_core_func.h"

// Complex
int32_t SPVM_NATIVE_SPVM__CORE__cadd(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_cadd(env, stack); };
/*
int32_t SPVM_NATIVE_SPVM__CORE__csub(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_csub(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__cmul(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_cmul(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__cdiv(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_cdiv(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__caddf(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_caddf(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__csubf(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_csubf(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__cmulf(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_cmulf(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__cdivf(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_cdivf(env, stack); };
*/

// Complex array
int32_t SPVM_NATIVE_SPVM__CORE__new_fcomplex(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_new_fcomplex(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__new_dcomplex(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_new_dcomplex(env, stack); };

int32_t SPVM_NATIVE_SPVM__CORE__print(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_print(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__warn(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_warn(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__time(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_time(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__isinff(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_isinff(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__isfinitef(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_isfinitef(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__isnanf(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_isnanf(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__INFINITYF(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_INFINITYF(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__NANF(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_NANF(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__INFINITY(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_INFINITY(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__NAN(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_NAN(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__isinf(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_isinf(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__isnan(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_isnan(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__isfinite(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_isfinite(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__sin(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_sin(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__cos(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_cos(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__tan(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_tan(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__asin(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_asin(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__acosh(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_acosh(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__atanh(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_atanh(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__asinh(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_asinh(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__acos(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_acos(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__atan(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_atan(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__exp(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_exp(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__exp2(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_exp2(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__log(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_log(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__log10(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_log10(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__sqrt(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_sqrt(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__cbrt(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_cbrt(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__ceil(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_ceil(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__remainder(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_remainder(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__floor(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_floor(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__nearbyint(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_nearbyint(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__atan2(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_atan2(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__pow(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_pow(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__round(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_round(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__abs(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_abs(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__labs(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_labs(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__fabs(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_fabs(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__erf(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_erf(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__erfc(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_erfc(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__lgamma(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_lgamma(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__tgamma(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_tgamma(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__sinh(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_sinh(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__cosh(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_cosh(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__tanh(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_tanh(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__hypot(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_hypot(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__expm1(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_expm1(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__log1p(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_log1p(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__FLT_MAX(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_FLT_MAX(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__FLT_MIN(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_FLT_MIN(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__DBL_MAX(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_DBL_MAX(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__DBL_MIN(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_DBL_MIN(env, stack); };
int32_t SPVM_NATIVE_SPVM__Math__PI(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_PI(env, stack); };
int32_t SPVM_NATIVE_SPVM__Math__E(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_E(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__INT8_MIN(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_INT8_MIN(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__INT8_MAX(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_INT8_MAX(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__INT16_MIN(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_INT16_MIN(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__INT16_MAX(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_INT16_MAX(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__INT32_MIN(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_INT32_MIN(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__INT32_MAX(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_INT32_MAX(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__INT64_MIN(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_INT64_MIN(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__INT64_MAX(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_INT64_MAX(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__PI(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_PI(env, stack); };
int32_t SPVM_NATIVE_SPVM__CORE__E(SPVM_ENV* env, SPVM_VALUE* stack) { return SPVM_CORE_FUNC_E(env, stack); };
