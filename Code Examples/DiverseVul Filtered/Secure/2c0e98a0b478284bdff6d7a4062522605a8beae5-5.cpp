static int mov_setup_track_ids(MOVMuxContext *mov, AVFormatContext *s)
{
    int i;

    if (mov->track_ids_ok)
        return 0;

    if (mov->use_stream_ids_as_track_ids) {
        int next_generated_track_id = 0;
        for (i = 0; i < s->nb_streams; i++) {
            if (s->streams[i]->id > next_generated_track_id)
                next_generated_track_id = s->streams[i]->id;
        }

        for (i = 0; i < mov->nb_streams; i++) {
            if (mov->tracks[i].entry <= 0 && !(mov->flags & FF_MOV_FLAG_FRAGMENT))
                continue;

            mov->tracks[i].track_id = i >= s->nb_streams ? ++next_generated_track_id : s->streams[i]->id;
        }
    } else {
        for (i = 0; i < mov->nb_streams; i++) {
            if (mov->tracks[i].entry <= 0 && !(mov->flags & FF_MOV_FLAG_FRAGMENT))
                continue;

            mov->tracks[i].track_id = i + 1;
        }
    }

    mov->track_ids_ok = 1;

    return 0;
}