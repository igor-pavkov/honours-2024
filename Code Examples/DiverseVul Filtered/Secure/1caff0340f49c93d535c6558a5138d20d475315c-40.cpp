static bool e1000_tso_state_needed(void *opaque)
{
    E1000State *s = opaque;

    return chkflag(TSO);
}