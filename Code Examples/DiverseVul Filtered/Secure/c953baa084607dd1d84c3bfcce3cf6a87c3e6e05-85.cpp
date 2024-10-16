static void mov_read_chapters(AVFormatContext *s)
{
    MOVContext *mov = s->priv_data;
    MOVStreamContext *sc;
    int64_t cur_pos;
    int i, j;
    int chapter_track;

    for (j = 0; j < mov->nb_chapter_tracks; j++) {
        AVStream *st = NULL;
        FFStream *sti = NULL;
        chapter_track = mov->chapter_tracks[j];
        for (i = 0; i < s->nb_streams; i++)
            if (s->streams[i]->id == chapter_track) {
                st = s->streams[i];
                break;
            }
        if (!st) {
            av_log(s, AV_LOG_ERROR, "Referenced QT chapter track not found\n");
            continue;
        }
        sti = ffstream(st);

        sc = st->priv_data;
        cur_pos = avio_tell(sc->pb);

        if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            st->disposition |= AV_DISPOSITION_ATTACHED_PIC | AV_DISPOSITION_TIMED_THUMBNAILS;
            if (sti->nb_index_entries) {
                // Retrieve the first frame, if possible
                AVIndexEntry *sample = &sti->index_entries[0];
                if (avio_seek(sc->pb, sample->pos, SEEK_SET) != sample->pos) {
                    av_log(s, AV_LOG_ERROR, "Failed to retrieve first frame\n");
                    goto finish;
                }

                if (ff_add_attached_pic(s, st, sc->pb, NULL, sample->size) < 0)
                    goto finish;
            }
        } else {
            st->codecpar->codec_type = AVMEDIA_TYPE_DATA;
            st->codecpar->codec_id = AV_CODEC_ID_BIN_DATA;
            st->discard = AVDISCARD_ALL;
            for (int i = 0; i < sti->nb_index_entries; i++) {
                AVIndexEntry *sample = &sti->index_entries[i];
                int64_t end = i+1 < sti->nb_index_entries ? sti->index_entries[i+1].timestamp : st->duration;
                uint8_t *title;
                uint16_t ch;
                int len, title_len;

                if (end < sample->timestamp) {
                    av_log(s, AV_LOG_WARNING, "ignoring stream duration which is shorter than chapters\n");
                    end = AV_NOPTS_VALUE;
                }

                if (avio_seek(sc->pb, sample->pos, SEEK_SET) != sample->pos) {
                    av_log(s, AV_LOG_ERROR, "Chapter %d not found in file\n", i);
                    goto finish;
                }

                // the first two bytes are the length of the title
                len = avio_rb16(sc->pb);
                if (len > sample->size-2)
                    continue;
                title_len = 2*len + 1;
                if (!(title = av_mallocz(title_len)))
                    goto finish;

                // The samples could theoretically be in any encoding if there's an encd
                // atom following, but in practice are only utf-8 or utf-16, distinguished
                // instead by the presence of a BOM
                if (!len) {
                    title[0] = 0;
                } else {
                    ch = avio_rb16(sc->pb);
                    if (ch == 0xfeff)
                        avio_get_str16be(sc->pb, len, title, title_len);
                    else if (ch == 0xfffe)
                        avio_get_str16le(sc->pb, len, title, title_len);
                    else {
                        AV_WB16(title, ch);
                        if (len == 1 || len == 2)
                            title[len] = 0;
                        else
                            avio_get_str(sc->pb, INT_MAX, title + 2, len - 1);
                    }
                }

                avpriv_new_chapter(s, i, st->time_base, sample->timestamp, end, title);
                av_freep(&title);
            }
        }
finish:
        avio_seek(sc->pb, cur_pos, SEEK_SET);
    }
}