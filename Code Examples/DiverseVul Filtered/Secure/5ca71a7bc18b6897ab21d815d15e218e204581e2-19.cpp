cmsBool  Type_LUT16_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsUInt32Number nTabSize;
    cmsPipeline* NewLUT = (cmsPipeline*) Ptr;
    cmsStage* mpe;
    _cmsStageToneCurvesData* PreMPE = NULL, *PostMPE = NULL;
    _cmsStageMatrixData* MatMPE = NULL;
    _cmsStageCLutData* clut = NULL;
    int i, InputChannels, OutputChannels, clutPoints;

    // Disassemble the LUT into components.
    mpe = NewLUT -> Elements;
    if (mpe != NULL && mpe ->Type == cmsSigMatrixElemType) {

        MatMPE = (_cmsStageMatrixData*) mpe ->Data;
        mpe = mpe -> Next;
    }


    if (mpe != NULL && mpe ->Type == cmsSigCurveSetElemType) {
        PreMPE = (_cmsStageToneCurvesData*) mpe ->Data;
        mpe = mpe -> Next;
    }

    if (mpe != NULL && mpe ->Type == cmsSigCLutElemType) {
        clut  = (_cmsStageCLutData*) mpe -> Data;
        mpe = mpe ->Next;
    }

    if (mpe != NULL && mpe ->Type == cmsSigCurveSetElemType) {
        PostMPE = (_cmsStageToneCurvesData*) mpe ->Data;
        mpe = mpe -> Next;
    }

    // That should be all
    if (mpe != NULL) {
        cmsSignalError(mpe->ContextID, cmsERROR_UNKNOWN_EXTENSION, "LUT is not suitable to be saved as LUT16");
        return FALSE;
    }

    InputChannels  = cmsPipelineInputChannels(NewLUT);
    OutputChannels = cmsPipelineOutputChannels(NewLUT);

    if (clut == NULL)
        clutPoints = 0;
    else
        clutPoints    = clut->Params->nSamples[0];

    if (!_cmsWriteUInt8Number(io, (cmsUInt8Number) InputChannels)) return FALSE;
    if (!_cmsWriteUInt8Number(io, (cmsUInt8Number) OutputChannels)) return FALSE;
    if (!_cmsWriteUInt8Number(io, (cmsUInt8Number) clutPoints)) return FALSE;
    if (!_cmsWriteUInt8Number(io, 0)) return FALSE; // Padding


    if (MatMPE != NULL) {

        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[0])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[1])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[2])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[3])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[4])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[5])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[6])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[7])) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, MatMPE -> Double[8])) return FALSE;
    }
    else {

        if (!_cmsWrite15Fixed16Number(io, 1)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 1)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 0)) return FALSE;
        if (!_cmsWrite15Fixed16Number(io, 1)) return FALSE;
    }


    if (PreMPE != NULL) {
        if (!_cmsWriteUInt16Number(io, (cmsUInt16Number) PreMPE ->TheCurves[0]->nEntries)) return FALSE;
    } else {
            if (!_cmsWriteUInt16Number(io, 2)) return FALSE;
    }

    if (PostMPE != NULL) {
        if (!_cmsWriteUInt16Number(io, (cmsUInt16Number) PostMPE ->TheCurves[0]->nEntries)) return FALSE;
    } else {
        if (!_cmsWriteUInt16Number(io, 2)) return FALSE;

    }

    // The prelinearization table

    if (PreMPE != NULL) {
        if (!Write16bitTables(self ->ContextID, io, PreMPE)) return FALSE;
    }
    else {
        for (i=0; i < InputChannels; i++) {

            if (!_cmsWriteUInt16Number(io, 0)) return FALSE;
            if (!_cmsWriteUInt16Number(io, 0xffff)) return FALSE;
        }
    }

    nTabSize = uipow(OutputChannels, clutPoints, InputChannels);
    if (nTabSize == (cmsUInt32Number) -1) return FALSE;
    if (nTabSize > 0) {
        // The 3D CLUT.
        if (clut != NULL) {
            if (!_cmsWriteUInt16Array(io, nTabSize, clut->Tab.T)) return FALSE;
        }
    }

    // The postlinearization table
    if (PostMPE != NULL) {
        if (!Write16bitTables(self ->ContextID, io, PostMPE)) return FALSE;
    }
    else {
        for (i=0; i < OutputChannels; i++) {

            if (!_cmsWriteUInt16Number(io, 0)) return FALSE;
            if (!_cmsWriteUInt16Number(io, 0xffff)) return FALSE;
        }
    }

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
}