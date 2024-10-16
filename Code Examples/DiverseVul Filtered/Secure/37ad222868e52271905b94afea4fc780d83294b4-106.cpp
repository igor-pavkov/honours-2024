static int ulhandler_throttle(ULHandler * const ulhandler,
                              const off_t uploaded, const double ts_start,
                              double *required_sleep)
{
    double ts_now;
    double elapsed;
    off_t would_be_uploaded;
    double wanted_ts;
    off_t previous_chunk_size;

    (void) uploaded;
    if (ulhandler->bandwidth <= 0UL) {
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
    would_be_uploaded = ulhandler->total_uploaded + ulhandler->chunk_size;
    if (ulhandler->bandwidth > 0UL) {
        wanted_ts = (double) would_be_uploaded / (double) ulhandler->bandwidth;
    } else {
        wanted_ts = elapsed;
    }
    *required_sleep = wanted_ts - elapsed;
    previous_chunk_size = ulhandler->chunk_size;
    if (ulhandler->total_uploaded > ulhandler->chunk_size) {
        if (*required_sleep < ulhandler->min_sleep) {
            ulhandler->chunk_size =
                (ulhandler->max_chunk_size + ulhandler->chunk_size) / 2;
        } else if (*required_sleep > ulhandler->max_sleep) {
            ulhandler->chunk_size =
                (ulhandler->min_chunk_size + ulhandler->chunk_size) / 2;
        } else {
            ulhandler->chunk_size = ulhandler->default_chunk_size;
        }
        if (ulhandler->chunk_size <= 0 ||
            ulhandler->chunk_size > (off_t) ulhandler->sizeof_buf) {
            ulhandler->chunk_size = ulhandler->default_chunk_size;
        }
        if (previous_chunk_size != ulhandler->default_chunk_size) {
            would_be_uploaded =
                ulhandler->total_uploaded + ulhandler->chunk_size;
            if (ulhandler->bandwidth > 0UL) {
                wanted_ts = (double) would_be_uploaded /
                    (double) ulhandler->bandwidth;
            } else {
                wanted_ts = elapsed;
            }
            *required_sleep = wanted_ts - elapsed;
        }
    }
    return 0;
}