static void coroutine_fn mirror_wait_for_all_io(MirrorBlockJob *s)
{
    while (s->in_flight > 0) {
        mirror_wait_for_free_in_flight_slot(s);
    }
}