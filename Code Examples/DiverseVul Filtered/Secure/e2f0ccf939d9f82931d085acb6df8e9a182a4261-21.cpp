static bool checkreturn pb_dec_submessage(pb_istream_t *stream, const pb_field_iter_t *field)
{
    bool status = true;
    bool submsg_consumed = false;
    pb_istream_t substream;

    if (!pb_make_string_substream(stream, &substream))
        return false;
    
    if (field->submsg_desc == NULL)
        PB_RETURN_ERROR(stream, "invalid field descriptor");
    
    /* Submessages can have a separate message-level callback that is called
     * before decoding the message. Typically it is used to set callback fields
     * inside oneofs. */
    if (PB_LTYPE(field->type) == PB_LTYPE_SUBMSG_W_CB && field->pSize != NULL)
    {
        /* Message callback is stored right before pSize. */
        pb_callback_t *callback = (pb_callback_t*)field->pSize - 1;
        if (callback->funcs.decode)
        {
            status = callback->funcs.decode(&substream, field, &callback->arg);

            if (substream.bytes_left == 0)
            {
                submsg_consumed = true;
            }
        }
    }

    /* Now decode the submessage contents */
    if (status && !submsg_consumed)
    {
        unsigned int flags = 0;

        /* Static required/optional fields are already initialized by top-level
         * pb_decode(), no need to initialize them again. */
        if (PB_ATYPE(field->type) == PB_ATYPE_STATIC &&
            PB_HTYPE(field->type) != PB_HTYPE_REPEATED)
        {
            flags = PB_DECODE_NOINIT;
        }

        status = pb_decode_inner(&substream, field->submsg_desc, field->pData, flags);
    }
    
    if (!pb_close_string_substream(stream, &substream))
        return false;

    return status;
}