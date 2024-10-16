static int compute_moov_size(AVFormatContext *s)
{
    int i, moov_size, moov_size2;
    MOVMuxContext *mov = s->priv_data;

    moov_size = get_moov_size(s);
    if (moov_size < 0)
        return moov_size;

    for (i = 0; i < mov->nb_streams; i++)
        mov->tracks[i].data_offset += moov_size;

    moov_size2 = get_moov_size(s);
    if (moov_size2 < 0)
        return moov_size2;

    /* if the size changed, we just switched from stco to co64 and need to
     * update the offsets */
    if (moov_size2 != moov_size)
        for (i = 0; i < mov->nb_streams; i++)
            mov->tracks[i].data_offset += moov_size2 - moov_size;

    return moov_size2;
}