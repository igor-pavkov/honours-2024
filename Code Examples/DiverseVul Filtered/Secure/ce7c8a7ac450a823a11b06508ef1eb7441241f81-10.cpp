off_t _q_filesize(const char *filepath)
{
    struct stat finfo;
    if (stat(filepath, &finfo) < 0) return -1;
    return finfo.st_size;
}