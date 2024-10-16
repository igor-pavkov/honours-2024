static void init_ordered2_dither(int line)
{
    static int const dither2x2[] =
    {
        0x00, 0x80,
        0xc0, 0x40,
    };

    ordered2_table = dither2x2 + (line % 2) * 2;
    ordered2_index = 0;
}