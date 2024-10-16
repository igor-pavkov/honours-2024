static off_t get_file_size(const char * const file)
{
    struct stat st;

    if (stat(file, &st) != 0) {
        return (off_t) -1;
    }
    return st.st_size;
}