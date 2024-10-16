static void cmd_get_configuration(IDEState *s, uint8_t *buf)
{
    uint32_t len;
    uint8_t index = 0;
    int max_len;

    /* only feature 0 is supported */
    if (buf[2] != 0 || buf[3] != 0) {
        ide_atapi_cmd_error(s, ILLEGAL_REQUEST,
                            ASC_INV_FIELD_IN_CMD_PACKET);
        return;
    }

    /* XXX: could result in alignment problems in some architectures */
    max_len = lduw_be_p(buf + 7);

    /*
     * XXX: avoid overflow for io_buffer if max_len is bigger than
     *      the size of that buffer (dimensioned to max number of
     *      sectors to transfer at once)
     *
     *      Only a problem if the feature/profiles grow.
     */
    if (max_len > BDRV_SECTOR_SIZE) {
        /* XXX: assume 1 sector */
        max_len = BDRV_SECTOR_SIZE;
    }

    memset(buf, 0, max_len);
    /*
     * the number of sectors from the media tells us which profile
     * to use as current.  0 means there is no media
     */
    if (media_is_dvd(s)) {
        stw_be_p(buf + 6, MMC_PROFILE_DVD_ROM);
    } else if (media_is_cd(s)) {
        stw_be_p(buf + 6, MMC_PROFILE_CD_ROM);
    }

    buf[10] = 0x02 | 0x01; /* persistent and current */
    len = 12; /* headers: 8 + 4 */
    len += ide_atapi_set_profile(buf, &index, MMC_PROFILE_DVD_ROM);
    len += ide_atapi_set_profile(buf, &index, MMC_PROFILE_CD_ROM);
    stl_be_p(buf, len - 4); /* data length */

    ide_atapi_cmd_reply(s, len, max_len);
}