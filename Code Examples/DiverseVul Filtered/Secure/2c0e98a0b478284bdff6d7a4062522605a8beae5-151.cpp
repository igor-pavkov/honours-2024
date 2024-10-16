static int mov_write_loci_tag(AVFormatContext *s, AVIOContext *pb)
{
    int lang;
    int64_t pos = avio_tell(pb);
    double latitude, longitude, altitude;
    int32_t latitude_fix, longitude_fix, altitude_fix;
    AVDictionaryEntry *t = get_metadata_lang(s, "location", &lang);
    const char *ptr, *place = "";
    char *end;
    static const char *astronomical_body = "earth";
    if (!t)
        return 0;

    ptr = t->value;
    longitude = strtod(ptr, &end);
    if (end == ptr) {
        av_log(s, AV_LOG_WARNING, "malformed location metadata\n");
        return 0;
    }
    ptr = end;
    latitude = strtod(ptr, &end);
    if (end == ptr) {
        av_log(s, AV_LOG_WARNING, "malformed location metadata\n");
        return 0;
    }
    ptr = end;
    altitude = strtod(ptr, &end);
    /* If no altitude was present, the default 0 should be fine */
    if (*end == '/')
        place = end + 1;

    latitude_fix  = (int32_t) ((1 << 16) * latitude);
    longitude_fix = (int32_t) ((1 << 16) * longitude);
    altitude_fix  = (int32_t) ((1 << 16) * altitude);

    avio_wb32(pb, 0);         /* size */
    ffio_wfourcc(pb, "loci"); /* type */
    avio_wb32(pb, 0);         /* version + flags */
    avio_wb16(pb, lang);
    avio_write(pb, place, strlen(place) + 1);
    avio_w8(pb, 0);           /* role of place (0 == shooting location, 1 == real location, 2 == fictional location) */
    avio_wb32(pb, latitude_fix);
    avio_wb32(pb, longitude_fix);
    avio_wb32(pb, altitude_fix);
    avio_write(pb, astronomical_body, strlen(astronomical_body) + 1);
    avio_w8(pb, 0);           /* additional notes, null terminated string */

    return update_size(pb, pos);
}