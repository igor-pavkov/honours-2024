static void write_metadata(AVFormatContext *s, unsigned int ts)
{
    AVIOContext *pb = s->pb;
    FLVContext *flv = s->priv_data;
    int metadata_count = 0;
    int64_t metadata_size_pos, data_size, metadata_count_pos;
    AVDictionaryEntry *tag = NULL;

    /* write meta_tag */
    avio_w8(pb, 18);            // tag type META
    metadata_size_pos = avio_tell(pb);
    avio_wb24(pb, 0);           // size of data part (sum of all parts below)
    avio_wb24(pb, ts);          // timestamp
    avio_wb32(pb, 0);           // reserved

    /* now data of data_size size */

    /* first event name as a string */
    avio_w8(pb, AMF_DATA_TYPE_STRING);
    put_amf_string(pb, "onMetaData"); // 12 bytes

    /* mixed array (hash) with size and string/type/data tuples */
    avio_w8(pb, AMF_DATA_TYPE_MIXEDARRAY);
    metadata_count_pos = avio_tell(pb);
    metadata_count = 4 * !!flv->video_enc +
                     5 * !!flv->audio_enc +
                     1 * !!flv->data_enc  +
                     2; // +2 for duration and file size

    avio_wb32(pb, metadata_count);

    put_amf_string(pb, "duration");
    flv->duration_offset = avio_tell(pb);

    // fill in the guessed duration, it'll be corrected later if incorrect
    put_amf_double(pb, s->duration / AV_TIME_BASE);

    if (flv->video_enc) {
        put_amf_string(pb, "width");
        put_amf_double(pb, flv->video_enc->width);

        put_amf_string(pb, "height");
        put_amf_double(pb, flv->video_enc->height);

        put_amf_string(pb, "videodatarate");
        put_amf_double(pb, flv->video_enc->bit_rate / 1024.0);

        if (flv->framerate != 0.0) {
            put_amf_string(pb, "framerate");
            put_amf_double(pb, flv->framerate);
            metadata_count++;
        }

        put_amf_string(pb, "videocodecid");
        put_amf_double(pb, flv->video_enc->codec_tag);
    }

    if (flv->audio_enc) {
        put_amf_string(pb, "audiodatarate");
        put_amf_double(pb, flv->audio_enc->bit_rate / 1024.0);

        put_amf_string(pb, "audiosamplerate");
        put_amf_double(pb, flv->audio_enc->sample_rate);

        put_amf_string(pb, "audiosamplesize");
        put_amf_double(pb, flv->audio_enc->codec_id == AV_CODEC_ID_PCM_U8 ? 8 : 16);

        put_amf_string(pb, "stereo");
        put_amf_bool(pb, flv->audio_enc->channels == 2);

        put_amf_string(pb, "audiocodecid");
        put_amf_double(pb, flv->audio_enc->codec_tag);
    }

    if (flv->data_enc) {
        put_amf_string(pb, "datastream");
        put_amf_double(pb, 0.0);
    }

    while ((tag = av_dict_get(s->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        if(   !strcmp(tag->key, "width")
            ||!strcmp(tag->key, "height")
            ||!strcmp(tag->key, "videodatarate")
            ||!strcmp(tag->key, "framerate")
            ||!strcmp(tag->key, "videocodecid")
            ||!strcmp(tag->key, "audiodatarate")
            ||!strcmp(tag->key, "audiosamplerate")
            ||!strcmp(tag->key, "audiosamplesize")
            ||!strcmp(tag->key, "stereo")
            ||!strcmp(tag->key, "audiocodecid")
            ||!strcmp(tag->key, "duration")
            ||!strcmp(tag->key, "onMetaData")
            ||!strcmp(tag->key, "datasize")
            ||!strcmp(tag->key, "lasttimestamp")
            ||!strcmp(tag->key, "totalframes")
            ||!strcmp(tag->key, "hasAudio")
            ||!strcmp(tag->key, "hasVideo")
            ||!strcmp(tag->key, "hasCuePoints")
            ||!strcmp(tag->key, "hasMetadata")
            ||!strcmp(tag->key, "hasKeyframes")
        ){
            av_log(s, AV_LOG_DEBUG, "Ignoring metadata for %s\n", tag->key);
            continue;
        }
        put_amf_string(pb, tag->key);
        avio_w8(pb, AMF_DATA_TYPE_STRING);
        put_amf_string(pb, tag->value);
        metadata_count++;
    }

    put_amf_string(pb, "filesize");
    flv->filesize_offset = avio_tell(pb);
    put_amf_double(pb, 0); // delayed write

    put_amf_string(pb, "");
    avio_w8(pb, AMF_END_OF_OBJECT);

    /* write total size of tag */
    data_size = avio_tell(pb) - metadata_size_pos - 10;

    avio_seek(pb, metadata_count_pos, SEEK_SET);
    avio_wb32(pb, metadata_count);

    avio_seek(pb, metadata_size_pos, SEEK_SET);
    avio_wb24(pb, data_size);
    avio_skip(pb, data_size + 10 - 3);
    avio_wb32(pb, data_size + 11);
}