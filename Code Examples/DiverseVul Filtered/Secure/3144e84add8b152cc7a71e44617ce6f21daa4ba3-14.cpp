static void pp_add_stdmac(macros_t *macros)
{
    macros_t **mp;

    /* Find the end of the list and avoid duplicates */
    for (mp = stdmacros; *mp; mp++) {
        if (*mp == macros)
            return;             /* Nothing to do */
    }

    nasm_assert(mp < &stdmacros[ARRAY_SIZE(stdmacros)-1]);

    *mp = macros;
}