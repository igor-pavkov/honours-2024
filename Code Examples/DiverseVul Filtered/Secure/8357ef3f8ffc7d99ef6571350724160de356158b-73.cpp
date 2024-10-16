static void AppLayerProtoDetectPMFreeSignature(AppLayerProtoDetectPMSignature *sig)
{
    SCEnter();
    if (sig == NULL)
        SCReturn;
    if (sig->cd)
        DetectContentFree(sig->cd);
    SCFree(sig);
    SCReturn;
}