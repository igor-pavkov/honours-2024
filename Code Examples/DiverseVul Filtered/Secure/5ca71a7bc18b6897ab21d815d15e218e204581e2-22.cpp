cmsBool Write8bitTables(cmsContext ContextID, cmsIOHANDLER* io, cmsUInt32Number n, _cmsStageToneCurvesData* Tables)
{
    int j;
    cmsUInt32Number i;
    cmsUInt8Number val;

    for (i=0; i < n; i++) {

        if (Tables) {

            // Usual case of identity curves
            if ((Tables ->TheCurves[i]->nEntries == 2) && 
                (Tables->TheCurves[i]->Table16[0] == 0) && 
                (Tables->TheCurves[i]->Table16[1] == 65535)) {

                    for (j=0; j < 256; j++) {
                        if (!_cmsWriteUInt8Number(io, (cmsUInt8Number) j)) return FALSE;
                    }
            }
            else 
                if (Tables ->TheCurves[i]->nEntries != 256) {
                    cmsSignalError(ContextID, cmsERROR_RANGE, "LUT8 needs 256 entries on prelinearization");
                    return FALSE;                
                }
                else
                    for (j=0; j < 256; j++) {

                        val = (cmsUInt8Number) FROM_16_TO_8(Tables->TheCurves[i]->Table16[j]);

                        if (!_cmsWriteUInt8Number(io, val)) return FALSE;
                    }
        }
    }
    return TRUE;
}