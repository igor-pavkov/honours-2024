int unit_name_mangle_with_suffix(const char *name, const char *operation, UnitNameMangle flags, const char *suffix, char **ret) {
        _cleanup_free_ char *s = NULL;
        bool mangled, suggest_escape = true;
        int r;

        assert(name);
        assert(suffix);
        assert(ret);

        if (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                return -EINVAL;

        if (!unit_suffix_is_valid(suffix))
                return -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        if (unit_name_is_valid(name, UNIT_NAME_ANY))
                goto good;

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        if (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                if (flags & UNIT_NAME_MANGLE_GLOB)
                        goto good;
                log_full(flags & UNIT_NAME_MANGLE_WARN ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        if (is_device_path(name)) {
                r = unit_name_from_path(name, ".device", ret);
                if (r >= 0)
                        return 1;
                if (r != -EINVAL)
                        return r;
        }

        if (path_is_absolute(name)) {
                r = unit_name_from_path(name, ".mount", ret);
                if (r >= 0)
                        return 1;
                if (r != -EINVAL)
                        return r;
        }

        s = new(char, strlen(name) * 4 + strlen(suffix) + 1);
        if (!s)
                return -ENOMEM;

        mangled = do_escape_mangle(name, flags & UNIT_NAME_MANGLE_GLOB, s);
        if (mangled)
                log_full(flags & UNIT_NAME_MANGLE_WARN ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        if ((!(flags & UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        if (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                return -EINVAL;

        *ret = TAKE_PTR(s);
        return 1;

good:
        s = strdup(name);
        if (!s)
                return -ENOMEM;

        *ret = TAKE_PTR(s);
        return 0;
}