const char *am_filepath_dirname(apr_pool_t *p, const char *path) 
{
    char *cp;

    /*
     * Try Unix and then Windows style. Borrowed from
     * apr_match_glob(), it seems it cannot be made more
     * portable.
     */
    if (((cp = strrchr(path, (int)'/')) == NULL) &&
        ((cp = strrchr(path, (int)'\\')) == NULL))
            return ".";
   
    return apr_pstrndup(p, path, cp - path);
}