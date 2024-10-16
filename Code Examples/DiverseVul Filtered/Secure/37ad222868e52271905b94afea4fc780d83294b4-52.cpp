static int ul_quota_update(const char * const file_name,
                           const int files_count, const off_t bytes)
{
    Quota quota;
    off_t file_size = (off_t) -1;
    int overflow;
    int ret = 0;

    if (files_count == 0 && bytes == (off_t) 0) {
        return 0;
    }
    if (quota_update(&quota, files_count, (long long) bytes, &overflow) != 0) {
        return 0;
    }
    if (overflow != 0) {
        ret = 1;
        if (file_name != NULL) {
            file_size = get_file_size(file_name);
        }
        if (file_size >= (off_t) 0 && unlink(file_name) == 0) {
            (void) quota_update(&quota, -1, (long long) -file_size, NULL);
        }
    }
    displayquota(&quota);

    return ret;
}