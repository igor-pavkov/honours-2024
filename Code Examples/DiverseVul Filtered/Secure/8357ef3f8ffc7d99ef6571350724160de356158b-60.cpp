static void AppLayerProtoDetectFreeProbingParsers(AppLayerProtoDetectProbingParser *pp)
{
    SCEnter();

    AppLayerProtoDetectProbingParser *tmp_pp = NULL;

    if (pp == NULL)
        goto end;

    while (pp != NULL) {
        tmp_pp = pp->next;
        AppLayerProtoDetectProbingParserFree(pp);
        pp = tmp_pp;
    }

 end:
    SCReturn;
}