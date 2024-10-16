static AppLayerProtoDetectProbingParserPort *AppLayerProtoDetectProbingParserPortAlloc(void)
{
    SCEnter();

    AppLayerProtoDetectProbingParserPort *p = SCMalloc(sizeof(AppLayerProtoDetectProbingParserPort));
    if (unlikely(p == NULL)) {
        exit(EXIT_FAILURE);
    }
    memset(p, 0, sizeof(AppLayerProtoDetectProbingParserPort));

    SCReturnPtr(p, "AppLayerProtoDetectProbingParserPort");
}