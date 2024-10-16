static void wsgi_signal_handler(int signum)
{
    apr_size_t nbytes = 1;

    if (signum == AP_SIG_GRACEFUL) {
        apr_file_write(wsgi_signal_pipe_out, "G", &nbytes);
        apr_file_flush(wsgi_signal_pipe_out);
    }
    else if (signum == SIGXCPU) {
        if (!wsgi_graceful_timeout)
            wsgi_daemon_shutdown++;

        apr_file_write(wsgi_signal_pipe_out, "C", &nbytes);
        apr_file_flush(wsgi_signal_pipe_out);
    }
    else {
        wsgi_daemon_shutdown++;

        apr_file_write(wsgi_signal_pipe_out, "S", &nbytes);
        apr_file_flush(wsgi_signal_pipe_out);
    }
}