long ZEXPORT inflateMark(strm)
z_streamp strm;
{
    struct inflate_state FAR *state;

    if (inflateStateCheck(strm))
        return -(1L << 16);
    state = (struct inflate_state FAR *)strm->state;
    return (long)(((unsigned long)((long)state->back)) << 16) +
        (state->mode == COPY ? state->length :
            (state->mode == MATCH ? state->was - state->length : 0));
}