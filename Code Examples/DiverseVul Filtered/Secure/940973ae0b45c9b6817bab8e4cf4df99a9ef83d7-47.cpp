static bool ide_error_needed(void *opaque)
{
    IDEBus *bus = opaque;

    return (bus->error_status != 0);
}