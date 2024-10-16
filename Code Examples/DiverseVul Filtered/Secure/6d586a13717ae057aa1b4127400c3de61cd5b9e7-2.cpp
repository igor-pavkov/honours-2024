int bus_socket_exec(sd_bus *b) {
        int s[2], r;

        assert(b);
        assert(b->input_fd < 0);
        assert(b->output_fd < 0);
        assert(b->exec_path);
        assert(b->busexec_pid == 0);

        r = socketpair(AF_UNIX, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0, s);
        if (r < 0)
                return -errno;

        r = safe_fork_full("(sd-busexec)", s+1, 1, FORK_RESET_SIGNALS|FORK_CLOSE_ALL_FDS, &b->busexec_pid);
        if (r < 0) {
                safe_close_pair(s);
                return r;
        }
        if (r == 0) {
                /* Child */

                if (rearrange_stdio(s[1], s[1], STDERR_FILENO) < 0)
                        _exit(EXIT_FAILURE);

                (void) rlimit_nofile_safe();

                if (b->exec_argv)
                        execvp(b->exec_path, b->exec_argv);
                else {
                        const char *argv[] = { b->exec_path, NULL };
                        execvp(b->exec_path, (char**) argv);
                }

                _exit(EXIT_FAILURE);
        }

        safe_close(s[1]);
        b->output_fd = b->input_fd = fd_move_above_stdio(s[0]);

        bus_socket_setup(b);

        return bus_socket_start_auth(b);
}