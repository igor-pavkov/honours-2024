static int mxf_read_material_package(void *arg, AVIOContext *pb, int tag, int size, UID uid, int64_t klv_offset)
{
    MXFPackage *package = arg;
    switch(tag) {
    case 0x4403:
        package->tracks_count = avio_rb32(pb);
        package->tracks_refs = av_calloc(package->tracks_count, sizeof(UID));
        if (!package->tracks_refs)
            return AVERROR(ENOMEM);
        avio_skip(pb, 4); /* useless size of objects, always 16 according to specs */
        avio_read(pb, (uint8_t *)package->tracks_refs, package->tracks_count * sizeof(UID));
        break;
    }
    return 0;
}