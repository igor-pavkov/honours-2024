static int ogm_chapter(AVFormatContext *as, uint8_t *key, uint8_t *val)
{
    int i, cnum, h, m, s, ms, keylen = strlen(key);
    AVChapter *chapter = NULL;

    if (keylen < 9 || sscanf(key, "CHAPTER%03d", &cnum) != 1)
        return 0;

    if (keylen <= 10) {
        if (sscanf(val, "%02d:%02d:%02d.%03d", &h, &m, &s, &ms) < 4)
            return 0;

        avpriv_new_chapter(as, cnum, (AVRational) { 1, 1000 },
                           ms + 1000 * (s + 60 * (m + 60 * h)),
                           AV_NOPTS_VALUE, NULL);
        av_free(val);
    } else if (!strcmp(key + keylen - 4, "NAME")) {
        for (i = 0; i < as->nb_chapters; i++)
            if (as->chapters[i]->id == cnum) {
                chapter = as->chapters[i];
                break;
            }
        if (!chapter)
            return 0;

        av_dict_set(&chapter->metadata, "title", val, AV_DICT_DONT_STRDUP_VAL);
    } else
        return 0;

    av_free(key);
    return 1;
}