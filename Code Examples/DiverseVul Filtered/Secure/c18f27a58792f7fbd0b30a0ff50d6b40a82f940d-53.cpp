static uint GetVersFromFPI(ctmbstr fpi)
{
    uint i;

    for (i = 0; W3C_Doctypes[i].name; ++i)
        if (W3C_Doctypes[i].fpi != NULL && TY_(tmbstrcasecmp)(W3C_Doctypes[i].fpi, fpi) == 0)
            return W3C_Doctypes[i].vers;

    return 0;
}