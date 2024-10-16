static tmbchar LastChar( tmbstr str )
{
    if ( str && *str )
    {
        int n = TY_(tmbstrlen)(str);
        return str[n-1];
    }
    return 0;
}