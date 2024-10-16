static struct pkg_envra *get_script_name(const char *cmdline, char **executable)
{
// TODO: we don't verify that python executable is not modified
// or that python package is properly signed
// (see CheckFingerprint/CheckHash below)
    /* Try to find package for the script by looking at argv[1].
     * This will work only if the cmdline contains the whole path.
     * Example: python /usr/bin/system-control-network
     */
    struct pkg_envra *script_pkg = NULL;
    char *script_name = get_argv1_if_full_path(cmdline);
    if (script_name)
    {
        script_pkg = rpm_get_package_nvr(script_name, NULL);
        if (script_pkg)
        {
            /* There is a well-formed script name in argv[1],
             * and it does belong to some package.
             * Replace executable
             * with data pertaining to the script.
             */
            *executable = script_name;
        }
    }

    return script_pkg;
}