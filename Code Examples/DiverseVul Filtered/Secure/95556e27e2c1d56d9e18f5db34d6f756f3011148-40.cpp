static int mov_write_dfla_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0);
    ffio_wfourcc(pb, "dfLa");
    avio_w8(pb, 0); /* version */
    avio_wb24(pb, 0); /* flags */

    /* Expect the encoder to pass a METADATA_BLOCK_TYPE_STREAMINFO. */
    if (track->par->extradata_size != FLAC_STREAMINFO_SIZE)
        return AVERROR_INVALIDDATA;

    /* TODO: Write other METADATA_BLOCK_TYPEs if the encoder makes them available. */
    avio_w8(pb, 1 << 7 | FLAC_METADATA_TYPE_STREAMINFO); /* LastMetadataBlockFlag << 7 | BlockType */
    avio_wb24(pb, track->par->extradata_size); /* Length */
    avio_write(pb, track->par->extradata, track->par->extradata_size); /* BlockData[Length] */

    return update_size(pb, pos);
}