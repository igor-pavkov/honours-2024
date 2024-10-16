static int dlmap_send(DLHandler * const dlhandler)
{
    int ret;
    double ts_start = 0.0;
    double required_sleep;
    off_t downloaded;

    if (dlhandler->bandwidth > 0UL && (ts_start = get_usec_time()) <= 0.0) {
        error(451, "gettimeofday()");
        return -1;
    }
    required_sleep = 0.0;
    for (;;) {
        ret = _dlmap_remap(dlhandler);
        if (ret < 0) {
            return -1;
        }
        if (ret == 1) {
            break;
        }
        if (dl_dowrite(dlhandler, dlhandler->map_data, dlhandler->chunk_size,
                       &downloaded) != 0) {
            return -1;
        }
        dlhandler->cur_pos += dlhandler->chunk_size;
#ifdef FTPWHO
        if (shm_data_cur != NULL) {
            shm_data_cur->download_current_size = dlhandler->cur_pos;
        }
#endif
        dlhandler->total_downloaded += downloaded;
        required_sleep = 0.0;
        if (dlhandler->bandwidth > 0UL) {
            dlhandler_throttle(dlhandler, downloaded, ts_start,
                               &required_sleep);
        }
        ret = dlhandler_handle_commands(dlhandler, required_sleep);
        if (ret != 0) {
            return ret;
        }
    }
    return 0;
}