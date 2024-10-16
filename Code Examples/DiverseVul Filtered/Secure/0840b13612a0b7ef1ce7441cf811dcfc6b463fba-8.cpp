apr_status_t modsecurity_request_body_to_stream(modsec_rec *msr, const char *buffer, int buflen, char **error_msg) {
    char *stream_input_body = NULL;
    char *data = NULL;
    int first_pkt = 0;

    if(msr->stream_input_data == NULL)  {
        msr->stream_input_data = (char *)calloc(sizeof(char), msr->stream_input_length + 1);
        first_pkt = 1;
    }
    else    {

        data = (char *)malloc(msr->stream_input_length + 1 - buflen);

        if(data == NULL)
            return -1;

        memset(data, 0, msr->stream_input_length + 1 - buflen);
        memcpy(data, msr->stream_input_data, msr->stream_input_length - buflen);

        stream_input_body = (char *)realloc(msr->stream_input_data, msr->stream_input_length + 1);

        msr->stream_input_data = (char *)stream_input_body;
    }

    if (msr->stream_input_data == NULL) {
        if(data)    {
            free(data);
            data = NULL;
        }
        *error_msg = apr_psprintf(msr->mp, "Unable to allocate memory to hold request body on stream. Asked for %" APR_SIZE_T_FMT " bytes.",
                msr->stream_input_length + 1);
        return -1;
    }

    memset(msr->stream_input_data, 0, msr->stream_input_length+1);

    if(first_pkt)   {
        memcpy(msr->stream_input_data, buffer, msr->stream_input_length);
    } else {
        memcpy(msr->stream_input_data, data, msr->stream_input_length - buflen);
        memcpy(msr->stream_input_data+(msr->stream_input_length - buflen), buffer, buflen);
    }

    if(data)    {
        free(data);
        data = NULL;
    }

    return 1;
}