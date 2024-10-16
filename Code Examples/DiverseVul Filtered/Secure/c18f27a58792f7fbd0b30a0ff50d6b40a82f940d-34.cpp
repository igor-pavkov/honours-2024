static Bool IsDigitHex(uint c)
{
    uint map;

    map = MAP(c);

    return (map & digithex)!=0;
}