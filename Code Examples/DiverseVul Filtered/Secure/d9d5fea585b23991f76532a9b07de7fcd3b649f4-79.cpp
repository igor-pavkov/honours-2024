static void wsgi_hook_child_init(server_rec *s, apr_pool_t *p)
{
    if (wsgi_python_required) {
        /*
         * Initialise Python if required to be done in
         * the child process. Note that it will not be
         * initialised if mod_python loaded and it has
         * already been done.
         */

        if (wsgi_python_after_fork)
            wsgi_python_init(p);

        /*
         * Now perform additional initialisation steps
         * always done in child process.
         */

        wsgi_python_child_init(p);
    }
}