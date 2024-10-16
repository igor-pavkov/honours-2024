void server_process_native_file(
                Server *s,
                int fd,
                const struct ucred *ucred,
                const struct timeval *tv,
                const char *label, size_t label_len) {

        struct stat st;
        bool sealed;
        int r;

        /* Data is in the passed fd, since it didn't fit in a
         * datagram. */

        assert(s);
        assert(fd >= 0);

        /* If it's a memfd, check if it is sealed. If so, we can just
         * use map it and use it, and do not need to copy the data
         * out. */
        sealed = memfd_get_sealed(fd) > 0;

        if (!sealed && (!ucred || ucred->uid != 0)) {
                _cleanup_free_ char *k = NULL;
                const char *e;

                /* If this is not a sealed memfd, and the peer is unknown or
                 * unprivileged, then verify the path. */

                r = fd_get_path(fd, &k);
                if (r < 0) {
                        log_error_errno(r, "readlink(/proc/self/fd/%i) failed: %m", fd);
                        return;
                }

                e = PATH_STARTSWITH_SET(k, "/dev/shm/", "/tmp/", "/var/tmp/");
                if (!e) {
                        log_error("Received file outside of allowed directories. Refusing.");
                        return;
                }

                if (!filename_is_valid(e)) {
                        log_error("Received file in subdirectory of allowed directories. Refusing.");
                        return;
                }
        }

        if (fstat(fd, &st) < 0) {
                log_error_errno(errno, "Failed to stat passed file, ignoring: %m");
                return;
        }

        if (!S_ISREG(st.st_mode)) {
                log_error("File passed is not regular. Ignoring.");
                return;
        }

        if (st.st_size <= 0)
                return;

        if (st.st_size > ENTRY_SIZE_MAX) {
                log_error("File passed too large. Ignoring.");
                return;
        }

        if (sealed) {
                void *p;
                size_t ps;

                /* The file is sealed, we can just map it and use it. */

                ps = PAGE_ALIGN(st.st_size);
                p = mmap(NULL, ps, PROT_READ, MAP_PRIVATE, fd, 0);
                if (p == MAP_FAILED) {
                        log_error_errno(errno, "Failed to map memfd, ignoring: %m");
                        return;
                }

                server_process_native_message(s, p, st.st_size, ucred, tv, label, label_len);
                assert_se(munmap(p, ps) >= 0);
        } else {
                _cleanup_free_ void *p = NULL;
                struct statvfs vfs;
                ssize_t n;

                if (fstatvfs(fd, &vfs) < 0) {
                        log_error_errno(errno, "Failed to stat file system of passed file, ignoring: %m");
                        return;
                }

                /* Refuse operating on file systems that have
                 * mandatory locking enabled, see:
                 *
                 * https://github.com/systemd/systemd/issues/1822
                 */
                if (vfs.f_flag & ST_MANDLOCK) {
                        log_error("Received file descriptor from file system with mandatory locking enabled, refusing.");
                        return;
                }

                /* Make the fd non-blocking. On regular files this has
                 * the effect of bypassing mandatory locking. Of
                 * course, this should normally not be necessary given
                 * the check above, but let's better be safe than
                 * sorry, after all NFS is pretty confusing regarding
                 * file system flags, and we better don't trust it,
                 * and so is SMB. */
                r = fd_nonblock(fd, true);
                if (r < 0) {
                        log_error_errno(r, "Failed to make fd non-blocking, ignoring: %m");
                        return;
                }

                /* The file is not sealed, we can't map the file here, since
                 * clients might then truncate it and trigger a SIGBUS for
                 * us. So let's stupidly read it */

                p = malloc(st.st_size);
                if (!p) {
                        log_oom();
                        return;
                }

                n = pread(fd, p, st.st_size, 0);
                if (n < 0)
                        log_error_errno(errno, "Failed to read file, ignoring: %m");
                else if (n > 0)
                        server_process_native_message(s, p, n, ucred, tv, label, label_len);
        }
}