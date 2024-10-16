static AppLayerProtoDetectProbingParserElement *AppLayerProtoDetectProbingParserElementAlloc(void)
{
    SCEnter();

    AppLayerProtoDetectProbingParserElement *p = SCMalloc(sizeof(AppLayerProtoDetectProbingParserElement));
    if (unlikely(p == NULL)) {
        exit(EXIT_FAILURE);
    }
    memset(p, 0, sizeof(AppLayerProtoDetectProbingParserElement));

    SCReturnPtr(p, "AppLayerProtoDetectProbingParserElement");
}