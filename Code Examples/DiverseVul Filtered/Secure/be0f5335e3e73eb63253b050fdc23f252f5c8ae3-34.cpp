static int field_name_len(const char *field)
{
    const char *end = ap_strchr_c(field, ':');
    if (end == NULL || end - field > LOG_NAME_MAX_LEN)
        return LOG_NAME_MAX_LEN;
    return end - field;
}