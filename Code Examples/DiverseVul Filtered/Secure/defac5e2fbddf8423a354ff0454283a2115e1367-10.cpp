static const FDCtrlCommand *get_command(uint8_t cmd)
{
    int idx;

    idx = command_to_handler[cmd];
    FLOPPY_DPRINTF("%s command\n", handlers[idx].name);
    return &handlers[idx];
}