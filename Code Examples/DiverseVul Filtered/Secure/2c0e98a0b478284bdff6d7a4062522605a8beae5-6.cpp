static uint16_t language_code(const char *str)
{
    return (((str[0] - 0x60) & 0x1F) << 10) +
           (((str[1] - 0x60) & 0x1F) <<  5) +
           (( str[2] - 0x60) & 0x1F);
}