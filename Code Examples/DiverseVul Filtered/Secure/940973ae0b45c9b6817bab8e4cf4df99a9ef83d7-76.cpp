static bool cmd_flush_cache(IDEState *s, uint8_t cmd)
{
    ide_flush_cache(s);
    return false;
}