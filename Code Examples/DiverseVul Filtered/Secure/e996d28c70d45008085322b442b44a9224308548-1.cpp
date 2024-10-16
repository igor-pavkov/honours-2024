static void define_macros_late(void)
{
    char temp[128];

    /*
     * In case if output format is defined by alias
     * we have to put shortname of the alias itself here
     * otherwise ABI backward compatibility gets broken.
     */
    snprintf(temp, sizeof(temp), "__OUTPUT_FORMAT__=%s",
             ofmt_alias ? ofmt_alias->shortname : ofmt->shortname);
    preproc->pre_define(temp);
}