static int mov_write_amr_tag(AVIOContext *pb, MOVTrack *track)
{
    avio_wb32(pb, 0x11); /* size */
    if (track->mode == MODE_MOV) ffio_wfourcc(pb, "samr");
    else                         ffio_wfourcc(pb, "damr");
    ffio_wfourcc(pb, "FFMP");
    avio_w8(pb, 0); /* decoder version */

    avio_wb16(pb, 0x81FF); /* Mode set (all modes for AMR_NB) */
    avio_w8(pb, 0x00); /* Mode change period (no restriction) */
    avio_w8(pb, 0x01); /* Frames per sample */
    return 0x11;
}