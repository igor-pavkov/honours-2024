static int bus_socket_inotify_setup(sd_bus *b) {
        _cleanup_free_ int *new_watches = NULL;
        _cleanup_free_ char *absolute = NULL;
        size_t n_allocated = 0, n = 0, done = 0, i;
        unsigned max_follow = 32;
        const char *p;
        int wd, r;

        assert(b);
        assert(b->watch_bind);
        assert(b->sockaddr.sa.sa_family == AF_UNIX);
        assert(b->sockaddr.un.sun_path[0] != 0);

        /* Sets up an inotify fd in case watch_bind is enabled: wait until the configured AF_UNIX file system socket
         * appears before connecting to it. The implemented is pretty simplistic: we just subscribe to relevant changes
         * to all prefix components of the path, and every time we get an event for that we try to reconnect again,
         * without actually caring what precisely the event we got told us. If we still can't connect we re-subscribe
         * to all relevant changes of anything in the path, so that our watches include any possibly newly created path
         * components. */

        if (b->inotify_fd < 0) {
                b->inotify_fd = inotify_init1(IN_NONBLOCK|IN_CLOEXEC);
                if (b->inotify_fd < 0)
                        return -errno;

                b->inotify_fd = fd_move_above_stdio(b->inotify_fd);
        }

        /* Make sure the path is NUL terminated */
        p = strndupa(b->sockaddr.un.sun_path, sizeof(b->sockaddr.un.sun_path));

        /* Make sure the path is absolute */
        r = path_make_absolute_cwd(p, &absolute);
        if (r < 0)
                goto fail;

        /* Watch all parent directories, and don't mind any prefix that doesn't exist yet. For the innermost directory
         * that exists we want to know when files are created or moved into it. For all parents of it we just care if
         * they are removed or renamed. */

        if (!GREEDY_REALLOC(new_watches, n_allocated, n + 1)) {
                r = -ENOMEM;
                goto fail;
        }

        /* Start with the top-level directory, which is a bit simpler than the rest, since it can't be a symlink, and
         * always exists */
        wd = inotify_add_watch(b->inotify_fd, "/", IN_CREATE|IN_MOVED_TO);
        if (wd < 0) {
                r = log_debug_errno(errno, "Failed to add inotify watch on /: %m");
                goto fail;
        } else
                new_watches[n++] = wd;

        for (;;) {
                _cleanup_free_ char *component = NULL, *prefix = NULL, *destination = NULL;
                size_t n_slashes, n_component;
                char *c = NULL;

                n_slashes = strspn(absolute + done, "/");
                n_component = n_slashes + strcspn(absolute + done + n_slashes, "/");

                if (n_component == 0) /* The end */
                        break;

                component = strndup(absolute + done, n_component);
                if (!component) {
                        r = -ENOMEM;
                        goto fail;
                }

                /* A trailing slash? That's a directory, and not a socket then */
                if (path_equal(component, "/")) {
                        r = -EISDIR;
                        goto fail;
                }

                /* A single dot? Let's eat this up */
                if (path_equal(component, "/.")) {
                        done += n_component;
                        continue;
                }

                prefix = strndup(absolute, done + n_component);
                if (!prefix) {
                        r = -ENOMEM;
                        goto fail;
                }

                if (!GREEDY_REALLOC(new_watches, n_allocated, n + 1)) {
                        r = -ENOMEM;
                        goto fail;
                }

                wd = inotify_add_watch(b->inotify_fd, prefix, IN_DELETE_SELF|IN_MOVE_SELF|IN_ATTRIB|IN_CREATE|IN_MOVED_TO|IN_DONT_FOLLOW);
                log_debug("Added inotify watch for %s on bus %s: %i", prefix, strna(b->description), wd);

                if (wd < 0) {
                        if (IN_SET(errno, ENOENT, ELOOP))
                                break; /* This component doesn't exist yet, or the path contains a cyclic symlink right now */

                        r = log_debug_errno(errno, "Failed to add inotify watch on %s: %m", empty_to_root(prefix));
                        goto fail;
                } else
                        new_watches[n++] = wd;

                /* Check if this is possibly a symlink. If so, let's follow it and watch it too. */
                r = readlink_malloc(prefix, &destination);
                if (r == -EINVAL) { /* not a symlink */
                        done += n_component;
                        continue;
                }
                if (r < 0)
                        goto fail;

                if (isempty(destination)) { /* Empty symlink target? Yuck! */
                        r = -EINVAL;
                        goto fail;
                }

                if (max_follow <= 0) { /* Let's make sure we don't follow symlinks forever */
                        r = -ELOOP;
                        goto fail;
                }

                if (path_is_absolute(destination)) {
                        /* For absolute symlinks we build the new path and start anew */
                        c = strjoin(destination, absolute + done + n_component);
                        done = 0;
                } else {
                        _cleanup_free_ char *t = NULL;

                        /* For relative symlinks we replace the last component, and try again */
                        t = strndup(absolute, done);
                        if (!t)
                                return -ENOMEM;

                        c = strjoin(t, "/", destination, absolute + done + n_component);
                }
                if (!c) {
                        r = -ENOMEM;
                        goto fail;
                }

                free(absolute);
                absolute = c;

                max_follow--;
        }

        /* And now, let's remove all watches from the previous iteration we don't need anymore */
        for (i = 0; i < b->n_inotify_watches; i++) {
                bool found = false;
                size_t j;

                for (j = 0; j < n; j++)
                        if (new_watches[j] == b->inotify_watches[i]) {
                                found = true;
                                break;
                        }

                if (found)
                        continue;

                (void) inotify_rm_watch(b->inotify_fd, b->inotify_watches[i]);
        }

        free_and_replace(b->inotify_watches, new_watches);
        b->n_inotify_watches = n;

        return 0;

fail:
        bus_close_inotify_fd(b);
        return r;
}