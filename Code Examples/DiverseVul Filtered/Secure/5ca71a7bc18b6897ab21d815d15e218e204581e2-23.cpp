cmsStage* ReadCLUT(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number Offset, int InputChannels, int OutputChannels)
{
    cmsUInt8Number  gridPoints8[cmsMAXCHANNELS]; // Number of grid points in each dimension.
    cmsUInt32Number GridPoints[cmsMAXCHANNELS], i;
    cmsUInt8Number  Precision;
    cmsStage* CLUT;
    _cmsStageCLutData* Data;

    if (!io -> Seek(io, Offset)) return NULL;
    if (io -> Read(io, gridPoints8, cmsMAXCHANNELS, 1) != 1) return NULL;


    for (i=0; i < cmsMAXCHANNELS; i++) {

        if (gridPoints8[i] == 1) return NULL; // Impossible value, 0 for no CLUT and then 2 at least
        GridPoints[i] = gridPoints8[i];
    }

    if (!_cmsReadUInt8Number(io, &Precision)) return NULL;

    if (!_cmsReadUInt8Number(io, NULL)) return NULL;
    if (!_cmsReadUInt8Number(io, NULL)) return NULL;
    if (!_cmsReadUInt8Number(io, NULL)) return NULL;

    CLUT = cmsStageAllocCLut16bitGranular(self ->ContextID, GridPoints, InputChannels, OutputChannels, NULL);
    if (CLUT == NULL) return NULL;

    Data = (_cmsStageCLutData*) CLUT ->Data;

    // Precision can be 1 or 2 bytes
    if (Precision == 1) {

        cmsUInt8Number  v;

        for (i=0; i < Data ->nEntries; i++) {

            if (io ->Read(io, &v, sizeof(cmsUInt8Number), 1) != 1) return NULL;
            Data ->Tab.T[i] = FROM_8_TO_16(v);
        }

    }
    else
        if (Precision == 2) {

            if (!_cmsReadUInt16Array(io, Data->nEntries, Data ->Tab.T)) {
                cmsStageFree(CLUT);
                return NULL;
            }
        }
        else {
            cmsStageFree(CLUT);
            cmsSignalError(self ->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unknown precision of '%d'", Precision);
            return NULL;
        }

        return CLUT;
}