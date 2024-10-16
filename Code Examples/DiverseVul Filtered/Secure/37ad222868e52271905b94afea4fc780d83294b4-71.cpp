static int _dlmap_remap(DLHandler * const dlhandler)
{
    size_t min_dlmap_size;
    off_t remaining;

    if (dlhandler->map_data != NULL) {
        if (dlhandler->cur_pos >= dlhandler->dlmap_pos &&
            dlhandler->cur_pos + dlhandler->chunk_size <=
            dlhandler->dlmap_pos + (off_t) dlhandler->dlmap_size) {
            if (dlhandler->cur_pos < dlhandler->dlmap_pos ||
                dlhandler->cur_pos - dlhandler->dlmap_pos >
                (off_t) dlhandler->dlmap_size) {
                addreply_noformat(451, "remap");
                return -1;
            }
            dlhandler->map_data =
                dlhandler->map + dlhandler->cur_pos - dlhandler->dlmap_pos;
            return 0;
        }
    }
    if (dlhandler->file_size - dlhandler->cur_pos < dlhandler->chunk_size) {
        dlhandler->chunk_size = dlhandler->file_size - dlhandler->cur_pos;
    }
    if (dlhandler->chunk_size <= 0) {
        return 1;
    }
    dlhandler->dlmap_pos = dlhandler->cur_pos;
    min_dlmap_size = dlhandler->chunk_size;
    if (dlhandler->dlmap_size < min_dlmap_size) {
        dlhandler->dlmap_size = min_dlmap_size;
    }
    dlhandler->dlmap_size = (dlhandler->dlmap_size + page_size - (size_t) 1U) &
        ~(page_size - (size_t) 1U);
    if (dlhandler->dlmap_size < page_size) {
        dlhandler->dlmap_size = page_size;
    }
    remaining = dlhandler->file_size - dlhandler->dlmap_pos;
    if ((off_t) dlhandler->dlmap_size > remaining) {
        dlhandler->dlmap_size = (off_t) remaining;
    }
    if (_dlmap_read(dlhandler) != 0) {
        error(451, MSG_DATA_READ_FAILED);
        return -1;
    }
    dlhandler->map_data = dlhandler->map;

    return 0;
}