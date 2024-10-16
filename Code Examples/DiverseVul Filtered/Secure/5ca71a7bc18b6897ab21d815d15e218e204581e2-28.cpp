void *Type_NamedColor_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{

    cmsUInt32Number      vendorFlag;     // Bottom 16 bits for ICC use
    cmsUInt32Number      count;          // Count of named colors
    cmsUInt32Number      nDeviceCoords;  // Num of device coordinates
    char                 prefix[32];     // Prefix for each color name
    char                 suffix[32];     // Suffix for each color name
    cmsNAMEDCOLORLIST*  v;
    cmsUInt32Number i;


    *nItems = 0;
    if (!_cmsReadUInt32Number(io, &vendorFlag)) return NULL;
    if (!_cmsReadUInt32Number(io, &count)) return NULL;
    if (!_cmsReadUInt32Number(io, &nDeviceCoords)) return NULL;

    if (io -> Read(io, prefix, 32, 1) != 1) return NULL;
    if (io -> Read(io, suffix, 32, 1) != 1) return NULL;

    prefix[31] = suffix[31] = 0;

    v = cmsAllocNamedColorList(self ->ContextID, count, nDeviceCoords, prefix, suffix);
    if (v == NULL) {
        cmsSignalError(self->ContextID, cmsERROR_RANGE, "Too many named colors '%d'", count);
        return NULL;
    }

    if (nDeviceCoords > cmsMAXCHANNELS) {
        cmsSignalError(self->ContextID, cmsERROR_RANGE, "Too many device coordinates '%d'", nDeviceCoords);
        return 0;
    }
    for (i=0; i < count; i++) {

        cmsUInt16Number PCS[3];
        cmsUInt16Number Colorant[cmsMAXCHANNELS];
        char Root[33];

        memset(Colorant, 0, sizeof(Colorant));
        if (io -> Read(io, Root, 32, 1) != 1) return NULL;
        Root[32] = 0;  // To prevent exploits

        if (!_cmsReadUInt16Array(io, 3, PCS)) goto Error;
        if (!_cmsReadUInt16Array(io, nDeviceCoords, Colorant)) goto Error;

        if (!cmsAppendNamedColor(v, Root, PCS, Colorant)) goto Error;
    }

    *nItems = 1;
    return (void*) v ;

Error:
    cmsFreeNamedColorList(v);
    return NULL;

    cmsUNUSED_PARAMETER(SizeOfTag);
}