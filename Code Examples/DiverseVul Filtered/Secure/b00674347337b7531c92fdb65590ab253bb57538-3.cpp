int unit_name_build_from_type(const char *prefix, const char *instance, UnitType type, char **ret) {
        _cleanup_free_ char *s = NULL;
        const char *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        if (!unit_prefix_is_valid(prefix))
                return -EINVAL;

        ut = unit_type_to_string(type);

        if (instance) {
                if (!unit_instance_is_valid(instance))
                        return -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } else
                s = strjoin(prefix, ".", ut);
        if (!s)
                return -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        if (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                return -EINVAL;

        *ret = TAKE_PTR(s);
        return 0;
}