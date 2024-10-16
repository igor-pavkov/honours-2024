static int compute_sidx_size(AVFormatContext *s)
{
    int i, sidx_size;
    MOVMuxContext *mov = s->priv_data;

    sidx_size = get_sidx_size(s);
    if (sidx_size < 0)
        return sidx_size;

    for (i = 0; i < mov->nb_streams; i++)
        mov->tracks[i].data_offset += sidx_size;

    return sidx_size;
}