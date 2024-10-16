static void CharToHex(unsigned char c, char * hexBuf)
{
    const char * hexchar = "0123456789ABCDEF";
    hexBuf[0] = hexchar[c >> 4];
    hexBuf[1] = hexchar[c & 0x0F];
}