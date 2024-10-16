TEST(MemIo, seek_out_of_bounds_01)
{
    byte buf[1024];
    memset(buf, 0, sizeof(buf));

    MemIo io(buf, sizeof(buf));
    ASSERT_FALSE(io.eof());

    byte tmp[16];

    // Seek to the end of the file.
    ASSERT_EQ(io.seek(0, BasicIo::end), 0);
    ASSERT_EQ(io.read(tmp, sizeof(tmp)), 0);

    // Try to seek past the end of the file.
    ASSERT_EQ(io.seek(0x10000000, BasicIo::end), 1);
    ASSERT_TRUE(io.eof());
    ASSERT_EQ(io.read(tmp, sizeof(tmp)), 0);
}