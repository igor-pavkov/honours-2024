static int mxf_read_generic_descriptor(void *arg, AVIOContext *pb, int tag, int size, UID uid, int64_t klv_offset)
{
    MXFDescriptor *descriptor = arg;
    descriptor->pix_fmt = AV_PIX_FMT_NONE;
    switch(tag) {
    case 0x3F01:
        descriptor->sub_descriptors_count = avio_rb32(pb);
        descriptor->sub_descriptors_refs = av_calloc(descriptor->sub_descriptors_count, sizeof(UID));
        if (!descriptor->sub_descriptors_refs)
            return AVERROR(ENOMEM);
        avio_skip(pb, 4); /* useless size of objects, always 16 according to specs */
        avio_read(pb, (uint8_t *)descriptor->sub_descriptors_refs, descriptor->sub_descriptors_count * sizeof(UID));
        break;
    case 0x3004:
        avio_read(pb, descriptor->essence_container_ul, 16);
        break;
    case 0x3006:
        descriptor->linked_track_id = avio_rb32(pb);
        break;
    case 0x3201: /* PictureEssenceCoding */
        avio_read(pb, descriptor->essence_codec_ul, 16);
        break;
    case 0x3203:
        descriptor->width = avio_rb32(pb);
        break;
    case 0x3202:
        descriptor->height = avio_rb32(pb);
        break;
    case 0x320C:
        descriptor->frame_layout = avio_r8(pb);
        break;
    case 0x320E:
        descriptor->aspect_ratio.num = avio_rb32(pb);
        descriptor->aspect_ratio.den = avio_rb32(pb);
        break;
    case 0x3212:
        descriptor->field_dominance = avio_r8(pb);
        break;
    case 0x3301:
        descriptor->component_depth = avio_rb32(pb);
        break;
    case 0x3302:
        descriptor->horiz_subsampling = avio_rb32(pb);
        break;
    case 0x3308:
        descriptor->vert_subsampling = avio_rb32(pb);
        break;
    case 0x3D03:
        descriptor->sample_rate.num = avio_rb32(pb);
        descriptor->sample_rate.den = avio_rb32(pb);
        break;
    case 0x3D06: /* SoundEssenceCompression */
        avio_read(pb, descriptor->essence_codec_ul, 16);
        break;
    case 0x3D07:
        descriptor->channels = avio_rb32(pb);
        break;
    case 0x3D01:
        descriptor->bits_per_sample = avio_rb32(pb);
        break;
    case 0x3401:
        mxf_read_pixel_layout(pb, descriptor);
        break;
    default:
        /* Private uid used by SONY C0023S01.mxf */
        if (IS_KLV_KEY(uid, mxf_sony_mpeg4_extradata)) {
            if (descriptor->extradata)
                av_log(NULL, AV_LOG_WARNING, "Duplicate sony_mpeg4_extradata\n");
            av_free(descriptor->extradata);
            descriptor->extradata_size = 0;
            descriptor->extradata = av_malloc(size);
            if (!descriptor->extradata)
                return AVERROR(ENOMEM);
            descriptor->extradata_size = size;
            avio_read(pb, descriptor->extradata, size);
        }
        break;
    }
    return 0;
}