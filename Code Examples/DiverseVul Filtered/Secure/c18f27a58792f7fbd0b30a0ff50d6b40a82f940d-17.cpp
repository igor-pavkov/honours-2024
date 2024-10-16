Bool TY_(IsDigit)(uint c)
{
    uint map;

    map = MAP(c);

    return (map & digit)!=0;
}