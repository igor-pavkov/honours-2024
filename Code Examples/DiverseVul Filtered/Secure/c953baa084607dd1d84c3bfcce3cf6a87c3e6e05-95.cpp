static int get_current_encryption_info(MOVContext *c, MOVEncryptionIndex **encryption_index, MOVStreamContext **sc)
{
    MOVFragmentStreamInfo *frag_stream_info;
    AVStream *st;
    int i;

    frag_stream_info = get_current_frag_stream_info(&c->frag_index);
    if (frag_stream_info) {
        for (i = 0; i < c->fc->nb_streams; i++) {
            if (c->fc->streams[i]->id == frag_stream_info->id) {
              st = c->fc->streams[i];
              break;
            }
        }
        if (i == c->fc->nb_streams)
            return 0;
        *sc = st->priv_data;

        if (!frag_stream_info->encryption_index) {
            // If this stream isn't encrypted, don't create the index.
            if (!(*sc)->cenc.default_encrypted_sample)
                return 0;
            frag_stream_info->encryption_index = av_mallocz(sizeof(*frag_stream_info->encryption_index));
            if (!frag_stream_info->encryption_index)
                return AVERROR(ENOMEM);
        }
        *encryption_index = frag_stream_info->encryption_index;
        return 1;
    } else {
        // No current track fragment, using stream level encryption info.

        if (c->fc->nb_streams < 1)
            return 0;
        st = c->fc->streams[c->fc->nb_streams - 1];
        *sc = st->priv_data;

        if (!(*sc)->cenc.encryption_index) {
            // If this stream isn't encrypted, don't create the index.
            if (!(*sc)->cenc.default_encrypted_sample)
                return 0;
            (*sc)->cenc.encryption_index = av_mallocz(sizeof(*frag_stream_info->encryption_index));
            if (!(*sc)->cenc.encryption_index)
                return AVERROR(ENOMEM);
        }

        *encryption_index = (*sc)->cenc.encryption_index;
        return 1;
    }
}