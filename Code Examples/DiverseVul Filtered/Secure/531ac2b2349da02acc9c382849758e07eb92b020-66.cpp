int manager_open_serialization(Manager *m, FILE **_f) {
        const char *path;
        int fd = -1;
        FILE *f;

        assert(_f);

        path = MANAGER_IS_SYSTEM(m) ? "/run/systemd" : "/tmp";
        fd = open_tmpfile_unlinkable(path, O_RDWR|O_CLOEXEC);
        if (fd < 0)
                return -errno;

        log_debug("Serializing state to %s", path);

        f = fdopen(fd, "w+");
        if (!f) {
                safe_close(fd);
                return -errno;
        }

        *_f = f;

        return 0;
}