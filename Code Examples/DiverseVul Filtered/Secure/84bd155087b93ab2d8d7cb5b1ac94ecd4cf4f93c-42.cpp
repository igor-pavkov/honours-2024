static void init_ordered8_dither(int line)
{
    static int const dither8x8[] =
    {
        0x00, 0x80, 0x20, 0xa0, 0x08, 0x88, 0x28, 0xa8,
        0xc0, 0x40, 0xe0, 0x60, 0xc8, 0x48, 0xe8, 0x68,
        0x30, 0xb0, 0x10, 0x90, 0x38, 0xb8, 0x18, 0x98,
        0xf0, 0x70, 0xd0, 0x50, 0xf8, 0x78, 0xd8, 0x58,
        0x0c, 0x8c, 0x2c, 0xac, 0x04, 0x84, 0x24, 0xa4,
        0xcc, 0x4c, 0xec, 0x6c, 0xc4, 0x44, 0xe4, 0x64,
        0x3c, 0xbc, 0x1c, 0x9c, 0x34, 0xb4, 0x14, 0x94,
        0xfc, 0x7c, 0xdc, 0x5c, 0xf4, 0x74, 0xd4, 0x54,
    };

    ordered8_table = dither8x8 + (line % 8) * 8;
    ordered8_index = 0;
}