static uint16_t atapi_byte_count_limit(IDEState *s)
{
    uint16_t bcl;

    bcl = s->lcyl | (s->hcyl << 8);
    if (bcl == 0xffff) {
        return 0xfffe;
    }
    return bcl;
}