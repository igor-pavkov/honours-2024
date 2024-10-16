static int dlhandler_throttle(DLHandler * const dlhandler,
                              const off_t downloaded, const double ts_start,
                              double *required_sleep)
{
    double ts_now;
    double elapsed;
    off_t would_be_downloaded;
    double wanted_ts;
    off_t previous_chunk_size;

    if (dlhandler->bandwidth <= 0UL || downloaded <= (off_t) 0) {
        *required_sleep = 0.0;
        return 0;
    }
    if ((ts_now = get_usec_time()) <= 0.0) {
        ts_now = ts_start;
    }
    if (ts_start > ts_now) {
        ts_now = ts_start;
    }
    elapsed = ts_now - ts_start;
    would_be_downloaded = dlhandler->total_downloaded + dlhandler->chunk_size;
    if (dlhandler->bandwidth > 0UL) {
        wanted_ts = (double) would_be_downloaded /
            (double) dlhandler->bandwidth;
    } else {
        wanted_ts = elapsed;
    }
    *required_sleep = wanted_ts - elapsed;
    previous_chunk_size = dlhandler->chunk_size;
    if (dlhandler->total_downloaded <= dlhandler->chunk_size) {
        return 0;
    }
    if (*required_sleep < dlhandler->min_sleep) {
        dlhandler->chunk_size =
            (dlhandler->max_chunk_size + dlhandler->chunk_size) / 2;
    } else if (*required_sleep > dlhandler->max_sleep) {
        dlhandler->chunk_size =
            (dlhandler->min_chunk_size + dlhandler->chunk_size) / 2;
    } else {
        dlhandler->chunk_size = dlhandler->default_chunk_size;
    }
    if (dlhandler->chunk_size <= 0 || dlhandler->chunk_size > INT_MAX) {
        dlhandler->chunk_size = dlhandler->default_chunk_size;
    }
    if (previous_chunk_size != dlhandler->default_chunk_size) {
        would_be_downloaded =
            dlhandler->total_downloaded + dlhandler->chunk_size;
        if (dlhandler->bandwidth > 0UL) {
            wanted_ts = (double) would_be_downloaded /
                (double) dlhandler->bandwidth;
        } else {
            wanted_ts = elapsed;
        }
        *required_sleep = wanted_ts - elapsed;
    }
    return 0;
}