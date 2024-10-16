const char *manager_get_runtime_prefix(Manager *m) {
        assert(m);

        return MANAGER_IS_SYSTEM(m) ?
               "/run" :
               getenv("XDG_RUNTIME_DIR");
}