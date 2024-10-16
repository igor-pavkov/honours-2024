Set *manager_get_units_requiring_mounts_for(Manager *m, const char *path) {
        char p[strlen(path)+1];

        assert(m);
        assert(path);

        strcpy(p, path);
        path_kill_slashes(p);

        return hashmap_get(m->units_requiring_mounts_for, streq(p, "/") ? "" : p);
}