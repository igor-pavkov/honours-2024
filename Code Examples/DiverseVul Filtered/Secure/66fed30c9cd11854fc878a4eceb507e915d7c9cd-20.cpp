static bool mirror_drained_poll(BlockJob *job)
{
    MirrorBlockJob *s = container_of(job, MirrorBlockJob, common);

    /* If the job isn't paused nor cancelled, we can't be sure that it won't
     * issue more requests. We make an exception if we've reached this point
     * from one of our own drain sections, to avoid a deadlock waiting for
     * ourselves.
     */
    if (!s->common.job.paused && !s->common.job.cancelled && !s->in_drain) {
        return true;
    }

    return !!s->in_flight;
}