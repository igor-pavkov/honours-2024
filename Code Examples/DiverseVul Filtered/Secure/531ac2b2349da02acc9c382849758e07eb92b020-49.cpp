static int manager_run_generators(Manager *m) {
        _cleanup_strv_free_ char **paths = NULL;
        const char *argv[5];
        char **path;
        int r;

        assert(m);

        if (m->test_run)
                return 0;

        paths = generator_binary_paths(m->unit_file_scope);
        if (!paths)
                return log_oom();

        /* Optimize by skipping the whole process by not creating output directories
         * if no generators are found. */
        STRV_FOREACH(path, paths) {
                if (access(*path, F_OK) >= 0)
                        goto found;
                if (errno != ENOENT)
                        log_warning_errno(errno, "Failed to open generator directory %s: %m", *path);
        }

        return 0;

 found:
        r = lookup_paths_mkdir_generator(&m->lookup_paths);
        if (r < 0)
                goto finish;

        argv[0] = NULL; /* Leave this empty, execute_directory() will fill something in */
        argv[1] = m->lookup_paths.generator;
        argv[2] = m->lookup_paths.generator_early;
        argv[3] = m->lookup_paths.generator_late;
        argv[4] = NULL;

        RUN_WITH_UMASK(0022)
                execute_directories((const char* const*) paths, DEFAULT_TIMEOUT_USEC, (char**) argv);

finish:
        lookup_paths_trim_generator(&m->lookup_paths);
        return r;
}