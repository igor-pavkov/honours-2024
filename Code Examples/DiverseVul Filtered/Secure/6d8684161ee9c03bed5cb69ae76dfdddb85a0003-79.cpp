int mingw_gethostname(char *name, int namelen)
{
    ensure_socket_initialization();
    return gethostname(name, namelen);
}