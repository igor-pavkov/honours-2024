static void test_media_insert(void)
{
    uint8_t dir;

    /* Insert media in drive. DSKCHK should not be reset until a step pulse
     * is sent. */
    qmp_discard_response("{'execute':'blockdev-change-medium', 'arguments':{"
                         " 'id':'floppy0', 'filename': %s, 'format': 'raw' }}",
                         test_image);

    dir = inb(FLOPPY_BASE + reg_dir);
    assert_bit_set(dir, DSKCHG);
    dir = inb(FLOPPY_BASE + reg_dir);
    assert_bit_set(dir, DSKCHG);

    send_seek(0);
    dir = inb(FLOPPY_BASE + reg_dir);
    assert_bit_set(dir, DSKCHG);
    dir = inb(FLOPPY_BASE + reg_dir);
    assert_bit_set(dir, DSKCHG);

    /* Step to next track should clear DSKCHG bit. */
    send_seek(1);
    dir = inb(FLOPPY_BASE + reg_dir);
    assert_bit_clear(dir, DSKCHG);
    dir = inb(FLOPPY_BASE + reg_dir);
    assert_bit_clear(dir, DSKCHG);
}