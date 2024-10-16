void _EXIT(const int status)
{
    delete_atomic_file();
#ifdef FTPWHO
    ftpwho_exit();
#endif
    _exit(status);
}