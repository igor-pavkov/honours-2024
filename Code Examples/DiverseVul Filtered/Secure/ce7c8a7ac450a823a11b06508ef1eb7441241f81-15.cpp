int _q_unlink(const char *pathname)
{
#ifdef _WIN32
    return _unlink(pathname);
#endif
    return unlink(pathname);
}