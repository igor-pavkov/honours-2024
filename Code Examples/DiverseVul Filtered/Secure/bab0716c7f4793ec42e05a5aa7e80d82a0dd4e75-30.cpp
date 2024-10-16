static int mxf_read_essence_container_data(void *arg, AVIOContext *pb, int tag, int size, UID uid, int64_t klv_offset)
{
    MXFEssenceContainerData *essence_data = arg;
    switch(tag) {
        case 0x2701:
            /* linked package umid UMID */
            avio_read(pb, essence_data->package_ul, 16);
            avio_read(pb, essence_data->package_uid, 16);
            break;
        case 0x3f06:
            essence_data->index_sid = avio_rb32(pb);
            break;
        case 0x3f07:
            essence_data->body_sid = avio_rb32(pb);
            break;
    }
    return 0;
}