static int mov_write_gama_tag(AVIOContext *pb, MOVTrack *track, double gamma)
{
    uint32_t gama = 0;
    if (gamma <= 0.0)
    {
        gamma = avpriv_get_gamma_from_trc(track->par->color_trc);
    }
    av_log(pb, AV_LOG_DEBUG, "gamma value %g\n", gamma);

    if (gamma > 1e-6) {
        gama = (uint32_t)lrint((double)(1<<16) * gamma);
        av_log(pb, AV_LOG_DEBUG, "writing gama value %d\n", gama);

        av_assert0(track->mode == MODE_MOV);
        avio_wb32(pb, 12);
        ffio_wfourcc(pb, "gama");
        avio_wb32(pb, gama);
        return 12;
    }
    else {
        av_log(pb, AV_LOG_WARNING, "gamma value unknown, unable to write gama atom\n");
    }
    return 0;
}