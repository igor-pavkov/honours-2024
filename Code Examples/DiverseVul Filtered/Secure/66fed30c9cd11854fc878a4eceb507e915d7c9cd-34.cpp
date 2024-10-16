static void mirror_cancel(Job *job, bool force)
{
    MirrorBlockJob *s = container_of(job, MirrorBlockJob, common.job);
    BlockDriverState *target = blk_bs(s->target);

    if (force || !job_is_ready(job)) {
        bdrv_cancel_in_flight(target);
    }
}