void* Type_Chromaticity_Dup(struct _cms_typehandler_struct* self, const void *Ptr, cmsUInt32Number n)
{
    return _cmsDupMem(self ->ContextID, Ptr, sizeof(cmsCIExyYTRIPLE));

    cmsUNUSED_PARAMETER(n);
}