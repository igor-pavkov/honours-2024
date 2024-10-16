Bool TY_(IsWhite)(uint c)
{
    uint map = MAP(c);

    return (map & white)!=0;
}