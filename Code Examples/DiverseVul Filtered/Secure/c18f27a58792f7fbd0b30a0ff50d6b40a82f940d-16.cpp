Bool TY_(IsLetter)(uint c)
{
    uint map;

    map = MAP(c);

    return (map & letter)!=0;
}