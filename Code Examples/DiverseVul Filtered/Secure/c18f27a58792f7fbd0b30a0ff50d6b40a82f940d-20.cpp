Bool TY_(IsNewline)(uint c)
{
    uint map = MAP(c);
    return (map & newline)!=0;
}