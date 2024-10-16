void delete_atomic_file(void)
{
    const char *atomic_file;

    if ((atomic_file = get_atomic_file(NULL)) == NULL || *atomic_file == 0) {
        return;
    }
    (void) unlink(atomic_file);
    atomic_file = NULL;
}