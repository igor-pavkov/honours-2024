int unit_name_build(const char *prefix, const char *instance, const char *suffix, char **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        if (suffix[0] != '.')
                return -EINVAL;

        type = unit_type_from_string(suffix + 1);
        if (type < 0)
                return -EINVAL;

        return unit_name_build_from_type(prefix, instance, type, ret);
}