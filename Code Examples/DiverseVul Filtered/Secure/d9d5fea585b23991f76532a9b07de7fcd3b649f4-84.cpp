static void wsgi_signal_handler(int signum)
{
    apr_size_t nbytes = 1;

    if (signum == SIGXCPU)
        wsgi_cpu_time_limit_exceeded = 1;

    apr_file_write(wsgi_signal_pipe_out, "X", &nbytes);
    apr_file_flush(wsgi_signal_pipe_out);

    wsgi_daemon_shutdown++;
}