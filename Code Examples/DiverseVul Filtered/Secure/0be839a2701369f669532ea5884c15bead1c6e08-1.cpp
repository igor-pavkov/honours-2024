static void reset_ram_globals(void)
{
    last_seen_block = NULL;
    last_sent_block = NULL;
    last_offset = 0;
    last_version = ram_list.version;
    ram_bulk_stage = true;
}