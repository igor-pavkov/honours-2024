cmsBool  Type_MPEclut_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsUInt8Number Dimensions8[16];  // 16 because the spec says 16 and not max number of channels
    cmsUInt32Number i;
    cmsStage* mpe = (cmsStage*) Ptr;
    _cmsStageCLutData* clut = (_cmsStageCLutData*) mpe ->Data;

    // Check for maximum number of channels supported by lcms
    if (mpe -> InputChannels > MAX_INPUT_DIMENSIONS) return FALSE;

    // Only floats are supported in MPE
    if (clut ->HasFloatValues == FALSE) return FALSE;

    if (!_cmsWriteUInt16Number(io, (cmsUInt16Number) mpe ->InputChannels)) return FALSE;
    if (!_cmsWriteUInt16Number(io, (cmsUInt16Number) mpe ->OutputChannels)) return FALSE;

    memset(Dimensions8, 0, sizeof(Dimensions8));

    for (i=0; i < mpe ->InputChannels; i++)
        Dimensions8[i] = (cmsUInt8Number) clut ->Params ->nSamples[i];

    if (!io ->Write(io, 16, Dimensions8)) return FALSE;

    for (i=0; i < clut ->nEntries; i++) {

        if (!_cmsWriteFloat32Number(io, clut ->Tab.TFloat[i])) return FALSE;
    }

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}