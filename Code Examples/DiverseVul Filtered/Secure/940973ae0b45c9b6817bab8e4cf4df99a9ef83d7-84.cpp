static bool ide_cmd_permitted(IDEState *s, uint32_t cmd)
{
    return cmd < ARRAY_SIZE(ide_cmd_table)
        && (ide_cmd_table[cmd].flags & (1u << s->drive_kind));
}