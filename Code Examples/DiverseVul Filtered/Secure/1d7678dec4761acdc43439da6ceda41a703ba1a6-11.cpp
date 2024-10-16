int vhdx_update_headers(BlockDriverState *bs, BDRVVHDXState *s,
                        bool generate_data_write_guid, MSGUID *log_guid)
{
    int ret;

    ret = vhdx_update_header(bs, s, generate_data_write_guid, log_guid);
    if (ret < 0) {
        return ret;
    }
    ret = vhdx_update_header(bs, s, generate_data_write_guid, log_guid);
    return ret;
}