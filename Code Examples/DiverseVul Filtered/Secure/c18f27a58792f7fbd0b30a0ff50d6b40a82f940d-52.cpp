ctmbstr TY_(HTMLVersionNameFromCode)( uint vers, Bool ARG_UNUSED(isXhtml) )
{
    ctmbstr name = GetNameFromVers(vers);

    /* this test has moved to ReportMarkupVersion() in localize.c, for localization reasons */
    /*
    if (!name)
        name = "HTML Proprietary";
     */

    return name;
}