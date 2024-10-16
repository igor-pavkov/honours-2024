static AVDictionaryEntry *get_metadata_lang(AVFormatContext *s,
                                            const char *tag, int *lang)
{
    int l, len, len2;
    AVDictionaryEntry *t, *t2 = NULL;
    char tag2[16];

    *lang = 0;

    if (!(t = av_dict_get(s->metadata, tag, NULL, 0)))
        return NULL;

    len = strlen(t->key);
    snprintf(tag2, sizeof(tag2), "%s-", tag);
    while ((t2 = av_dict_get(s->metadata, tag2, t2, AV_DICT_IGNORE_SUFFIX))) {
        len2 = strlen(t2->key);
        if (len2 == len + 4 && !strcmp(t->value, t2->value)
            && (l = ff_mov_iso639_to_lang(&t2->key[len2 - 3], 1)) >= 0) {
            *lang = l;
            return t;
        }
    }
    return t;
}