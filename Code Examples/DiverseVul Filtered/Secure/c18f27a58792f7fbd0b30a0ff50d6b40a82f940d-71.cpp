Bool TY_(IsHTMLSpace)(uint c)
{
    return c == 0x020 || c == 0x009 || c == 0x00a || c == 0x00c || c == 0x00d;
}