static int analyze_security_one(sd_bus *bus, const char *name, Table* overview_table, AnalyzeSecurityFlags flags) {
        _cleanup_(security_info_free) struct security_info info = {
                .default_dependencies = true,
                .capability_bounding_set = UINT64_MAX,
                .restrict_namespaces = UINT64_MAX,
                ._umask = 0002,
        };
        int r;

        assert(bus);
        assert(name);

        r = acquire_security_info(bus, name, &info, flags);
        if (r == -EMEDIUMTYPE) /* Ignore this one because not loaded or Type is oneshot */
                return 0;
        if (r < 0)
                return r;

        r = assess(&info, overview_table, flags);
        if (r < 0)
                return r;

        return 0;
}