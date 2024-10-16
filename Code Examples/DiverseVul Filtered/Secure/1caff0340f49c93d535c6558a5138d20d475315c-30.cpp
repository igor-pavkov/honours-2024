static int e1000_tx_tso_post_load(void *opaque, int version_id)
{
    E1000State *s = opaque;
    s->received_tx_tso = true;
    return 0;
}