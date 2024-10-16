static void coroutine_fn mirror_pause(Job *job)
{
    MirrorBlockJob *s = container_of(job, MirrorBlockJob, common.job);

    mirror_wait_for_all_io(s);
}