static int dlmap_exit(DLHandler * const dlhandler)
{
    if (dlhandler->map != NULL) {
        free(dlhandler->map);
        dlhandler->map = NULL;
        dlhandler->sizeof_map = (size_t) 0U;
        dlhandler->dlmap_size = (size_t) 0U;
    }
    return 0;
}