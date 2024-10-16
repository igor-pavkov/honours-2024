Bool IsLower(uint c)
{
    uint map = MAP(c);

    return (map & lowercase)!=0;
}