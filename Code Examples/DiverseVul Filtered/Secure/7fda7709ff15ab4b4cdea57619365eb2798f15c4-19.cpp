void janet_lib_array(JanetTable *env) {
    JanetRegExt array_cfuns[] = {
        JANET_CORE_REG("array/new", cfun_array_new),
        JANET_CORE_REG("array/new-filled", cfun_array_new_filled),
        JANET_CORE_REG("array/fill", cfun_array_fill),
        JANET_CORE_REG("array/pop", cfun_array_pop),
        JANET_CORE_REG("array/peek", cfun_array_peek),
        JANET_CORE_REG("array/push", cfun_array_push),
        JANET_CORE_REG("array/ensure", cfun_array_ensure),
        JANET_CORE_REG("array/slice", cfun_array_slice),
        JANET_CORE_REG("array/concat", cfun_array_concat),
        JANET_CORE_REG("array/insert", cfun_array_insert),
        JANET_CORE_REG("array/remove", cfun_array_remove),
        JANET_CORE_REG("array/trim", cfun_array_trim),
        JANET_CORE_REG("array/clear", cfun_array_clear),
        JANET_REG_END
    };
    janet_core_cfuns_ext(env, NULL, array_cfuns);
}