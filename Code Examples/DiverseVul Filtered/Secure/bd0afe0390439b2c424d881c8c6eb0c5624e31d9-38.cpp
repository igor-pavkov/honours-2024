TEST(MemIo, seek_out_of_bounds_03)
{
    byte buf[1024];
    memset(buf, 0, sizeof(buf));

    MemIo io(buf, sizeof(buf));
    ASSERT_FALSE(io.eof());

    byte tmp[16];

    // Try to seek past the beginning of the file.
    ASSERT_EQ(io.seek(-0x10000000, BasicIo::cur), 1);
    ASSERT_FALSE(io.eof());
    // The seek was invalid, so the offset didn't change and this read still works.
    ASSERT_EQ(io.read(tmp, sizeof(tmp)), sizeof(tmp));
}