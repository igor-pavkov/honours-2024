static struct passwd *fakegetpwnam(const char * const name)
{
    static struct passwd pwd;

    (void) name;
    pwd.pw_name = pwd.pw_gecos = pwd.pw_shell = "ftp";
    pwd.pw_passwd = "*";
    pwd.pw_uid = (uid_t) 42U;
    pwd.pw_gid = (gid_t) 42U;
    pwd.pw_dir = WIN32_ANON_DIR;

    return &pwd;
}