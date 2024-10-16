int slice_build_parent_slice(const char *slice, char **ret) {
        _cleanup_free_ char *s = NULL;
        char *dash;
        int r;

        assert(slice);
        assert(ret);

        if (!slice_name_is_valid(slice))
                return -EINVAL;

        if (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                return 0;
        }

        s = strdup(slice);
        if (!s)
                return -ENOMEM;

        dash = strrchr(s, '-');
        if (dash)
                strcpy(dash, ".slice");
        else {
                r = free_and_strdup(&s, SPECIAL_ROOT_SLICE);
                if (r < 0)
                        return r;
        }

        *ret = TAKE_PTR(s);
        return 1;
}