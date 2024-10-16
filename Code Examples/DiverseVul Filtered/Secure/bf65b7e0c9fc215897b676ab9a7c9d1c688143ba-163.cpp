static int unit_export_log_extra_fields(Unit *u, const ExecContext *c) {
        _cleanup_close_ int fd = -1;
        struct iovec *iovec;
        const char *p;
        char *pattern;
        le64_t *sizes;
        ssize_t n;
        size_t i;
        int r;

        if (u->exported_log_extra_fields)
                return 0;

        if (c->n_log_extra_fields <= 0)
                return 0;

        sizes = newa(le64_t, c->n_log_extra_fields);
        iovec = newa(struct iovec, c->n_log_extra_fields * 2);

        for (i = 0; i < c->n_log_extra_fields; i++) {
                sizes[i] = htole64(c->log_extra_fields[i].iov_len);

                iovec[i*2] = IOVEC_MAKE(sizes + i, sizeof(le64_t));
                iovec[i*2+1] = c->log_extra_fields[i];
        }

        p = strjoina("/run/systemd/units/log-extra-fields:", u->id);
        pattern = strjoina(p, ".XXXXXX");

        fd = mkostemp_safe(pattern);
        if (fd < 0)
                return log_unit_debug_errno(u, fd, "Failed to create extra fields file %s: %m", p);

        n = writev(fd, iovec, c->n_log_extra_fields*2);
        if (n < 0) {
                r = log_unit_debug_errno(u, errno, "Failed to write extra fields: %m");
                goto fail;
        }

        (void) fchmod(fd, 0644);

        if (rename(pattern, p) < 0) {
                r = log_unit_debug_errno(u, errno, "Failed to rename extra fields file: %m");
                goto fail;
        }

        u->exported_log_extra_fields = true;
        return 0;

fail:
        (void) unlink(pattern);
        return r;
}