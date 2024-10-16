static void mirror_abort(Job *job)
{
    int ret = mirror_exit_common(job);
    assert(ret == 0);
}