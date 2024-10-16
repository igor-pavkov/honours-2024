static bool validate_bcl(IDEState *s)
{
    /* TODO: Check IDENTIFY data word 125 for defacult BCL (currently 0) */
    if (s->atapi_dma || atapi_byte_count_limit(s)) {
        return true;
    }

    /* TODO: Move abort back into core.c and introduce proper error flow between
     *       ATAPI layer and IDE core layer */
    ide_abort_command(s);
    return false;
}