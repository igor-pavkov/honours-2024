static int mov_create_dvd_sub_decoder_specific_info(MOVTrack *track,
                                                    AVStream *st)
{
    int i, width = 720, height = 480;
    int have_palette = 0, have_size = 0;
    uint32_t palette[16];
    char *cur = st->codecpar->extradata;

    while (cur && *cur) {
        if (strncmp("palette:", cur, 8) == 0) {
            int i, count;
            count = sscanf(cur + 8,
                "%06"PRIx32", %06"PRIx32", %06"PRIx32", %06"PRIx32", "
                "%06"PRIx32", %06"PRIx32", %06"PRIx32", %06"PRIx32", "
                "%06"PRIx32", %06"PRIx32", %06"PRIx32", %06"PRIx32", "
                "%06"PRIx32", %06"PRIx32", %06"PRIx32", %06"PRIx32"",
                &palette[ 0], &palette[ 1], &palette[ 2], &palette[ 3],
                &palette[ 4], &palette[ 5], &palette[ 6], &palette[ 7],
                &palette[ 8], &palette[ 9], &palette[10], &palette[11],
                &palette[12], &palette[13], &palette[14], &palette[15]);

            for (i = 0; i < count; i++) {
                palette[i] = rgb_to_yuv(palette[i]);
            }
            have_palette = 1;
        } else if (!strncmp("size:", cur, 5)) {
            sscanf(cur + 5, "%dx%d", &width, &height);
            have_size = 1;
        }
        if (have_palette && have_size)
            break;
        cur += strcspn(cur, "\n\r");
        cur += strspn(cur, "\n\r");
    }
    if (have_palette) {
        track->vos_data = av_malloc(16*4);
        if (!track->vos_data)
            return AVERROR(ENOMEM);
        for (i = 0; i < 16; i++) {
            AV_WB32(track->vos_data + i * 4, palette[i]);
        }
        track->vos_len = 16 * 4;
    }
    st->codecpar->width = width;
    st->codecpar->height = track->height = height;

    return 0;
}