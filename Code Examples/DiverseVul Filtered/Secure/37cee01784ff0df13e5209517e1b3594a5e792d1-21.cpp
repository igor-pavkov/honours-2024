static void lan9118_tick(void *opaque)
{
    lan9118_state *s = (lan9118_state *)opaque;
    if (s->int_en & GPT_INT) {
        s->int_sts |= GPT_INT;
    }
    lan9118_update(s);
}