static int dlmap_init(DLHandler * const dlhandler, const int clientfd,
                      void * const tls_clientfd, const int xferfd,
                      const char * const name, const int f,
                      void * const tls_fd, const off_t restartat,
                      const int ascii_mode, const unsigned long bandwidth)
{
    if (ascii_mode > 0) {
#ifdef WITHOUT_ASCII
        addreply_noformat(450, MSG_ASCII_MODE_UNSUPPORTED);
        return -1;
#else
        addreply_noformat(0, MSG_ASCII_MODE_WARNING);
#endif
    }
    if (dlhandler_init(dlhandler, clientfd, tls_clientfd, xferfd, name, f,
                       tls_fd, restartat, ascii_mode, bandwidth) != 0) {
        return -1;
    }
    dlhandler->min_chunk_size = DL_MIN_CHUNK_SIZE;
    if (ascii_mode > 0) {
        dlhandler->default_chunk_size = dlhandler->max_chunk_size =
            DL_DEFAULT_CHUNK_SIZE_ASCII;
    } else {
        dlhandler->max_chunk_size = DL_MAX_CHUNK_SIZE;
        if (bandwidth <= 0UL) {
            dlhandler->default_chunk_size = dlhandler->max_chunk_size;
        } else {
            dlhandler->default_chunk_size = DL_DEFAULT_CHUNK_SIZE;
        }
    }
    dlhandler->chunk_size = dlhandler->default_chunk_size;
    dlhandler->dlmap_size =
        (DL_DLMAP_SIZE + page_size - (size_t) 1U) & ~(page_size - (size_t) 1U);
    dlhandler->cur_pos = restartat;
    dlhandler->dlmap_pos = (off_t) 0;
    dlhandler->dlmap_fdpos = (off_t) -1;
    dlhandler->sizeof_map = (size_t) 0U;
    dlhandler->map_data = NULL;
    dlhandler->sizeof_map = dlhandler->dlmap_size;
    dlhandler->map = malloc(dlhandler->sizeof_map);
    if (dlhandler->map == NULL) {
        die_mem();
    }
    return 0;
}