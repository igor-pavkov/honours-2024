static void mirror_complete(Job *job, Error **errp)
{
    MirrorBlockJob *s = container_of(job, MirrorBlockJob, common.job);

    if (!s->synced) {
        error_setg(errp, "The active block job '%s' cannot be completed",
                   job->id);
        return;
    }

    /* block all operations on to_replace bs */
    if (s->replaces) {
        AioContext *replace_aio_context;

        s->to_replace = bdrv_find_node(s->replaces);
        if (!s->to_replace) {
            error_setg(errp, "Node name '%s' not found", s->replaces);
            return;
        }

        replace_aio_context = bdrv_get_aio_context(s->to_replace);
        aio_context_acquire(replace_aio_context);

        /* TODO Translate this into permission system. Current definition of
         * GRAPH_MOD would require to request it for the parents; they might
         * not even be BlockDriverStates, however, so a BdrvChild can't address
         * them. May need redefinition of GRAPH_MOD. */
        error_setg(&s->replace_blocker,
                   "block device is in use by block-job-complete");
        bdrv_op_block_all(s->to_replace, s->replace_blocker);
        bdrv_ref(s->to_replace);

        aio_context_release(replace_aio_context);
    }

    s->should_complete = true;

    /* If the job is paused, it will be re-entered when it is resumed */
    if (!job->paused) {
        job_enter(job);
    }
}