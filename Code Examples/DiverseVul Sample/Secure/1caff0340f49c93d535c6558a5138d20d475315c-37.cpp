static bool e1000_full_mac_needed(void *opaque)
{
    E1000State *s = opaque;

    return chkflag(MAC);
}