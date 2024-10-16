static AppLayerProtoDetectProbingParser *AppLayerProtoDetectProbingParserAlloc(void)
{
    SCEnter();

    AppLayerProtoDetectProbingParser *p = SCMalloc(sizeof(AppLayerProtoDetectProbingParser));
    if (unlikely(p == NULL)) {
        exit(EXIT_FAILURE);
    }
    memset(p, 0, sizeof(AppLayerProtoDetectProbingParser));

    SCReturnPtr(p, "AppLayerProtoDetectProbingParser");
}