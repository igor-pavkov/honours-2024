static void vorbis_cleanup(AVFormatContext *s, int idx)
{
    struct ogg *ogg = s->priv_data;
    struct ogg_stream *os = ogg->streams + idx;
    struct oggvorbis_private *priv = os->private;
    int i;
    if (os->private)
        for (i = 0; i < 3; i++)
            av_freep(&priv->packet[i]);
}