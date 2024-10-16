static void AppLayerProtoDetectProbingParserElementFree(AppLayerProtoDetectProbingParserElement *p)
{
    SCEnter();
    SCFree(p);
    SCReturn;
}