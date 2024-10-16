static int mov_read_iloc(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    int version, offset_size, length_size, base_offset_size, index_size;
    int item_count, extent_count;
    uint64_t base_offset, extent_offset, extent_length;
    uint8_t value;

    if (!c->is_still_picture_avif) {
        // * For non-avif, we simply ignore the iloc box.
        // * For animated avif, we don't care about the iloc box as all the
        //   necessary information can be found in the moov box.
        return 0;
    }

    if (c->fc->nb_streams) {
        av_log(c->fc, AV_LOG_INFO, "Duplicate iloc box found\n");
        return 0;
    }

    version = avio_r8(pb);
    avio_rb24(pb);  // flags.

    value = avio_r8(pb);
    offset_size = (value >> 4) & 0xF;
    length_size = value & 0xF;
    value = avio_r8(pb);
    base_offset_size = (value >> 4) & 0xF;
    index_size = !version ? 0 : (value & 0xF);
    if (index_size) {
        av_log(c->fc, AV_LOG_ERROR, "iloc: index_size != 0 not supported.\n");
        return AVERROR_PATCHWELCOME;
    }
    item_count = (version < 2) ? avio_rb16(pb) : avio_rb32(pb);

    c->avif_info = av_malloc_array(item_count, sizeof(*c->avif_info));
    if (!c->avif_info)
        return AVERROR(ENOMEM);
    c->avif_info_size = item_count;

    for (int i = 0; i < item_count; i++) {
        int item_id = (version < 2) ? avio_rb16(pb) : avio_rb32(pb);
        if (avio_feof(pb))
            return AVERROR_INVALIDDATA;
        c->avif_info[i].item_id = item_id;

        if (version > 0)
            avio_rb16(pb);  // construction_method.
        avio_rb16(pb);  // data_reference_index.
        if (rb_size(pb, &base_offset, base_offset_size) < 0)
            return AVERROR_INVALIDDATA;
        extent_count = avio_rb16(pb);
        if (extent_count > 1) {
            // For still AVIF images, we only support one extent item.
            av_log(c->fc, AV_LOG_ERROR, "iloc: extent_count > 1 not supported.\n");
            return AVERROR_PATCHWELCOME;
        }
        for (int j = 0; j < extent_count; j++) {
            if (rb_size(pb, &extent_offset, offset_size) < 0 ||
                rb_size(pb, &extent_length, length_size) < 0)
                return AVERROR_INVALIDDATA;
            c->avif_info[i].extent_length = extent_length;
            c->avif_info[i].extent_offset = base_offset + extent_offset;
        }
    }

    return atom.size;
}