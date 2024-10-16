static void char2hex(char c, char *buf)
{
    buf[0] = num2hex((c >> 4) & 0xf);
    buf[1] = num2hex((c)&0xf);
}