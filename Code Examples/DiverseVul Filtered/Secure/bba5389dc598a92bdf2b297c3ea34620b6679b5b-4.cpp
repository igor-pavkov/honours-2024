static int read_channel_identities (WavpackContext *wpc, WavpackMetadata *wpmd)
{
    if (!wpc->channel_identities) {
        wpc->channel_identities = (unsigned char *)malloc (wpmd->byte_length + 1);
        memcpy (wpc->channel_identities, wpmd->data, wpmd->byte_length);
        wpc->channel_identities [wpmd->byte_length] = 0;
    }

    return TRUE;
}