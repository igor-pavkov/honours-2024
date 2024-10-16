static inline bool is_valid_warning(int severity)
{
    /* Not a warning at all */
    if ((severity & ERR_MASK) != ERR_WARNING)
        return false;

    return WARN_IDX(severity) < ERR_WARN_ALL;
}