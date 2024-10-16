Bool TY_(IsNamechar)(uint c)
{
    uint map = MAP(c);
    return (map & namechar)!=0;
}