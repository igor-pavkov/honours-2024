static MXFWrappingScheme mxf_get_wrapping_by_body_sid(AVFormatContext *s, int body_sid)
{
    for (int i = 0; i < s->nb_streams; i++) {
        MXFTrack *track = s->streams[i]->priv_data;
        if (track && track->body_sid == body_sid && track->wrapping != UnknownWrapped)
            return track->wrapping;
    }
    return UnknownWrapped;
}