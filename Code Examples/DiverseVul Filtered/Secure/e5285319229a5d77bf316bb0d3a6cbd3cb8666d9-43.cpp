static void convert_32s4u_C1R(const OPJ_INT32* pSrc, OPJ_BYTE* pDst,
                              OPJ_SIZE_T length)
{
    OPJ_SIZE_T i;
    for (i = 0; i < (length & ~(OPJ_SIZE_T)1U); i += 2U) {
        OPJ_UINT32 src0 = (OPJ_UINT32)pSrc[i + 0];
        OPJ_UINT32 src1 = (OPJ_UINT32)pSrc[i + 1];

        *pDst++ = (OPJ_BYTE)((src0 << 4) | src1);
    }

    if (length & 1U) {
        OPJ_UINT32 src0 = (OPJ_UINT32)pSrc[i + 0];
        *pDst++ = (OPJ_BYTE)((src0 << 4));
    }
}