int ZEXPORT inflateGetDictionary(strm, dictionary, dictLength)
z_streamp strm;
Bytef *dictionary;
uInt *dictLength;
{
    struct inflate_state FAR *state;

    /* check state */
    if (inflateStateCheck(strm)) return Z_STREAM_ERROR;
    state = (struct inflate_state FAR *)strm->state;

    /* copy dictionary */
    if (state->whave && dictionary != Z_NULL) {
        zmemcpy(dictionary, state->window + state->wnext,
                state->whave - state->wnext);
        zmemcpy(dictionary + state->whave - state->wnext,
                state->window, state->wnext);
    }
    if (dictLength != Z_NULL)
        *dictLength = state->whave;
    return Z_OK;
}