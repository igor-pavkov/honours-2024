static inline int get_byte(LZOContext *c)
{
    if (c->in < c->in_end)
        return *c->in++;
    c->error |= AV_LZO_INPUT_DEPLETED;
    return 1;
}