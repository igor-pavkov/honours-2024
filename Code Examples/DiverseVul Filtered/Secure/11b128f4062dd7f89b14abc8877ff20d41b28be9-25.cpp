static int qcow2_load_vmstate(BlockDriverState *bs, uint8_t *buf,
                              int64_t pos, int size)
{
    BDRVQcowState *s = bs->opaque;
    int growable = bs->growable;
    bool zero_beyond_eof = bs->zero_beyond_eof;
    int ret;

    BLKDBG_EVENT(bs->file, BLKDBG_VMSTATE_LOAD);
    bs->growable = 1;
    bs->zero_beyond_eof = false;
    ret = bdrv_pread(bs, qcow2_vm_state_offset(s) + pos, buf, size);
    bs->growable = growable;
    bs->zero_beyond_eof = zero_beyond_eof;

    return ret;
}