static inline int cloop_read_block(BlockDriverState *bs, int block_num)
{
    BDRVCloopState *s = bs->opaque;

    if (s->current_block != block_num) {
        int ret;
        uint32_t bytes = s->offsets[block_num + 1] - s->offsets[block_num];

        ret = bdrv_pread(bs->file, s->offsets[block_num], s->compressed_block,
                         bytes);
        if (ret != bytes) {
            return -1;
        }

        s->zstream.next_in = s->compressed_block;
        s->zstream.avail_in = bytes;
        s->zstream.next_out = s->uncompressed_block;
        s->zstream.avail_out = s->block_size;
        ret = inflateReset(&s->zstream);
        if (ret != Z_OK) {
            return -1;
        }
        ret = inflate(&s->zstream, Z_FINISH);
        if (ret != Z_STREAM_END || s->zstream.total_out != s->block_size) {
            return -1;
        }

        s->current_block = block_num;
    }
    return 0;
}