static int mov_read_clli(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVStreamContext *sc;

    if (c->fc->nb_streams < 1)
        return AVERROR_INVALIDDATA;

    sc = c->fc->streams[c->fc->nb_streams - 1]->priv_data;

    if (atom.size < 4) {
        av_log(c->fc, AV_LOG_ERROR, "Empty Content Light Level Info box\n");
        return AVERROR_INVALIDDATA;
    }

    if (sc->coll){
        av_log(c->fc, AV_LOG_WARNING, "Ignoring duplicate CLLI/COLL\n");
        return 0;
    }

    sc->coll = av_content_light_metadata_alloc(&sc->coll_size);
    if (!sc->coll)
        return AVERROR(ENOMEM);

    sc->coll->MaxCLL  = avio_rb16(pb);
    sc->coll->MaxFALL = avio_rb16(pb);

    return 0;
}