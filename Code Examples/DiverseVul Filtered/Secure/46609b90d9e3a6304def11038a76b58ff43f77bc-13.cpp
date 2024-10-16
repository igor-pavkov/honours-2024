static void test_media_change(void)
{
    uint8_t dir;

    test_media_insert();

    /* Eject the floppy and check that DSKCHG is set. Reading it out doesn't
     * reset the bit. */
    qmp_discard_response("{'execute':'eject', 'arguments':{"
                         " 'id':'floppy0' }}");

    dir = inb(FLOPPY_BASE + reg_dir);
    assert_bit_set(dir, DSKCHG);
    dir = inb(FLOPPY_BASE + reg_dir);
    assert_bit_set(dir, DSKCHG);

    send_seek(0);
    dir = inb(FLOPPY_BASE + reg_dir);
    assert_bit_set(dir, DSKCHG);
    dir = inb(FLOPPY_BASE + reg_dir);
    assert_bit_set(dir, DSKCHG);

    send_seek(1);
    dir = inb(FLOPPY_BASE + reg_dir);
    assert_bit_set(dir, DSKCHG);
    dir = inb(FLOPPY_BASE + reg_dir);
    assert_bit_set(dir, DSKCHG);
}