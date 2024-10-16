static int serialize_cgroup_mask(FILE *f, const char *key, CGroupMask mask) {
        _cleanup_free_ char *s = NULL;
        int r;

        assert(f);
        assert(key);

        if (mask == 0)
                return 0;

        r = cg_mask_to_string(mask, &s);
        if (r < 0)
                return log_error_errno(r, "Failed to format cgroup mask: %m");

        return serialize_item(f, key, s);
}