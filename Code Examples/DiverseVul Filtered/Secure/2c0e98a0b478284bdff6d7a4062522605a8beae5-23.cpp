static int mov_write_isml_manifest(AVIOContext *pb, MOVMuxContext *mov, AVFormatContext *s)
{
    int64_t pos = avio_tell(pb);
    int i;
    int64_t manifest_bit_rate = 0;
    AVCPBProperties *props = NULL;

    static const uint8_t uuid[] = {
        0xa5, 0xd4, 0x0b, 0x30, 0xe8, 0x14, 0x11, 0xdd,
        0xba, 0x2f, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66
    };

    avio_wb32(pb, 0);
    ffio_wfourcc(pb, "uuid");
    avio_write(pb, uuid, sizeof(uuid));
    avio_wb32(pb, 0);

    avio_printf(pb, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    avio_printf(pb, "<smil xmlns=\"http://www.w3.org/2001/SMIL20/Language\">\n");
    avio_printf(pb, "<head>\n");
    if (!(mov->fc->flags & AVFMT_FLAG_BITEXACT))
        avio_printf(pb, "<meta name=\"creator\" content=\"%s\" />\n",
                    LIBAVFORMAT_IDENT);
    avio_printf(pb, "</head>\n");
    avio_printf(pb, "<body>\n");
    avio_printf(pb, "<switch>\n");

    mov_setup_track_ids(mov, s);

    for (i = 0; i < mov->nb_streams; i++) {
        MOVTrack *track = &mov->tracks[i];
        const char *type;
        int track_id = track->track_id;

        AVStream *st = track->st;
        AVDictionaryEntry *lang = av_dict_get(st->metadata, "language", NULL,0);

        if (track->par->codec_type == AVMEDIA_TYPE_VIDEO) {
            type = "video";
        } else if (track->par->codec_type == AVMEDIA_TYPE_AUDIO) {
            type = "audio";
        } else {
            continue;
        }

        props = (AVCPBProperties*)av_stream_get_side_data(track->st, AV_PKT_DATA_CPB_PROPERTIES, NULL);

        if (track->par->bit_rate) {
            manifest_bit_rate = track->par->bit_rate;
        } else if (props) {
            manifest_bit_rate = props->max_bitrate;
        }

        avio_printf(pb, "<%s systemBitrate=\"%"PRId64"\">\n", type,
                    manifest_bit_rate);
        param_write_int(pb, "systemBitrate", manifest_bit_rate);
        param_write_int(pb, "trackID", track_id);
        param_write_string(pb, "systemLanguage", lang ? lang->value : "und");
        if (track->par->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (track->par->codec_id == AV_CODEC_ID_H264) {
                uint8_t *ptr;
                int size = track->par->extradata_size;
                if (!ff_avc_write_annexb_extradata(track->par->extradata, &ptr,
                                                   &size)) {
                    param_write_hex(pb, "CodecPrivateData",
                                    ptr ? ptr : track->par->extradata,
                                    size);
                    av_free(ptr);
                }
                param_write_string(pb, "FourCC", "H264");
            } else if (track->par->codec_id == AV_CODEC_ID_VC1) {
                param_write_string(pb, "FourCC", "WVC1");
                param_write_hex(pb, "CodecPrivateData", track->par->extradata,
                                track->par->extradata_size);
            }
            param_write_int(pb, "MaxWidth", track->par->width);
            param_write_int(pb, "MaxHeight", track->par->height);
            param_write_int(pb, "DisplayWidth", track->par->width);
            param_write_int(pb, "DisplayHeight", track->par->height);
        } else {
            if (track->par->codec_id == AV_CODEC_ID_AAC) {
                switch (track->par->profile)
                {
                    case FF_PROFILE_AAC_HE_V2:
                        param_write_string(pb, "FourCC", "AACP");
                        break;
                    case FF_PROFILE_AAC_HE:
                        param_write_string(pb, "FourCC", "AACH");
                        break;
                    default:
                        param_write_string(pb, "FourCC", "AACL");
                }
            } else if (track->par->codec_id == AV_CODEC_ID_WMAPRO) {
                param_write_string(pb, "FourCC", "WMAP");
            }
            param_write_hex(pb, "CodecPrivateData", track->par->extradata,
                            track->par->extradata_size);
            param_write_int(pb, "AudioTag", ff_codec_get_tag(ff_codec_wav_tags,
                                                             track->par->codec_id));
            param_write_int(pb, "Channels", track->par->channels);
            param_write_int(pb, "SamplingRate", track->par->sample_rate);
            param_write_int(pb, "BitsPerSample", 16);
            param_write_int(pb, "PacketSize", track->par->block_align ?
                                              track->par->block_align : 4);
        }
        avio_printf(pb, "</%s>\n", type);
    }
    avio_printf(pb, "</switch>\n");
    avio_printf(pb, "</body>\n");
    avio_printf(pb, "</smil>\n");

    return update_size(pb, pos);
}