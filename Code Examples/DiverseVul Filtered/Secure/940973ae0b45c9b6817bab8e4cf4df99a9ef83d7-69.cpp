static bool ide_is_pio_out(IDEState *s)
{
    if (s->end_transfer_func == ide_sector_write ||
        s->end_transfer_func == ide_atapi_cmd) {
        return false;
    } else if (s->end_transfer_func == ide_sector_read ||
               s->end_transfer_func == ide_transfer_stop ||
               s->end_transfer_func == ide_atapi_cmd_reply_end ||
               s->end_transfer_func == ide_dummy_transfer_stop) {
        return true;
    }

    abort();
}