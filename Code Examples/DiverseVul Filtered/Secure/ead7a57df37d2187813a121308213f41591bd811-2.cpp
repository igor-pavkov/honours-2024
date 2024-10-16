static void ssd0323_cd(void *opaque, int n, int level)
{
    ssd0323_state *s = (ssd0323_state *)opaque;
    DPRINTF("%s mode\n", level ? "Data" : "Command");
    s->mode = level ? SSD0323_DATA : SSD0323_CMD;
}