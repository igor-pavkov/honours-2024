static void coroutine_fn mirror_throttle(MirrorBlockJob *s)
{
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_REALTIME);

    if (now - s->last_pause_ns > BLOCK_JOB_SLICE_TIME) {
        s->last_pause_ns = now;
        job_sleep_ns(&s->common.job, 0);
    } else {
        job_pause_point(&s->common.job);
    }
}