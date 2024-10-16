static int parallels_read(BlockDriverState *bs, int64_t sector_num,
                    uint8_t *buf, int nb_sectors)
{
    while (nb_sectors > 0) {
        int64_t position = seek_to_sector(bs, sector_num);
        if (position >= 0) {
            if (bdrv_pread(bs->file, position, buf, 512) != 512)
                return -1;
        } else {
            memset(buf, 0, 512);
        }
        nb_sectors--;
        sector_num++;
        buf += 512;
    }
    return 0;
}