static apr_status_t apr_unix_file_cleanup(void *thefile)
{
    apr_file_t *file = thefile;

    return apr_file_close(file);
}