Bool TY_(IsUpper)(uint c)
{
    uint map = MAP(c);

    return (map & uppercase)!=0;
}