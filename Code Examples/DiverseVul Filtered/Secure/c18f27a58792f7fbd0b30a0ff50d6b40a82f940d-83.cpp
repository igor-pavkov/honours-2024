static Bool IsValidAttrName( ctmbstr attr )
{
    uint i, c = attr[0];

    /* first character should be a letter */
    if (!TY_(IsLetter)(c))
        return no;

    /* remaining characters should be namechars */
    for( i = 1; i < TY_(tmbstrlen)(attr); i++)
    {
        c = attr[i];

        if (TY_(IsNamechar)(c))
            continue;

        return no;
    }

    return yes;
}