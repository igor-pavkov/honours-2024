TEST(MemIo, seek_out_of_bounds_00)
{
    byte buf[1024];
    memset(buf, 0, sizeof(buf));

    MemIo io(buf, sizeof(buf));
    ASSERT_FALSE(io.eof());

    // Regression test for bug reported in https://github.com/Exiv2/exiv2/pull/945
    // The problem is that MemIo::seek() does not check that the new offset is
    // in bounds.
    byte tmp[16];
    ASSERT_EQ(io.seek(0x10000000, BasicIo::beg), 1);
    ASSERT_TRUE(io.eof());

    // The seek was invalid, so the offset didn't change and this read still works.
    ASSERT_EQ(io.read(tmp, sizeof(tmp)), sizeof(tmp));
}