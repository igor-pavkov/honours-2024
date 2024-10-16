int json_read_object(const char *cp, const struct json_attr_t *attrs,
		     const char **end)
{
    int st;

    json_debug_trace((1, "json_read_object() sees '%s'\n", cp));
    st = json_internal_read_object(cp, attrs, NULL, 0, end);
    return st;
}