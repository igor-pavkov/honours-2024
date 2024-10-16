cmsBool Type_NamedColor_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsNAMEDCOLORLIST* NamedColorList = (cmsNAMEDCOLORLIST*) Ptr;
    char                prefix[33];     // Prefix for each color name
    char                suffix[33];     // Suffix for each color name
    int i, nColors;

    nColors = cmsNamedColorCount(NamedColorList);

    if (!_cmsWriteUInt32Number(io, 0)) return FALSE;
    if (!_cmsWriteUInt32Number(io, nColors)) return FALSE;
    if (!_cmsWriteUInt32Number(io, NamedColorList ->ColorantCount)) return FALSE;

    strncpy(prefix, (const char*) NamedColorList->Prefix, 32);
    strncpy(suffix, (const char*) NamedColorList->Suffix, 32);

    suffix[32] = prefix[32] = 0;

    if (!io ->Write(io, 32, prefix)) return FALSE;
    if (!io ->Write(io, 32, suffix)) return FALSE;

    for (i=0; i < nColors; i++) {

       cmsUInt16Number PCS[3];
       cmsUInt16Number Colorant[cmsMAXCHANNELS];
       char Root[33];

        if (!cmsNamedColorInfo(NamedColorList, i, Root, NULL, NULL, PCS, Colorant)) return 0;
        Root[32] = 0;
        if (!io ->Write(io, 32 , Root)) return FALSE;
        if (!_cmsWriteUInt16Array(io, 3, PCS)) return FALSE;
        if (!_cmsWriteUInt16Array(io, NamedColorList ->ColorantCount, Colorant)) return FALSE;
    }

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}