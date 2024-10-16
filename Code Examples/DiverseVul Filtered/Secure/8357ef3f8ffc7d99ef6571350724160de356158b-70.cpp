static void AppLayerProtoDetectProbingParserFree(AppLayerProtoDetectProbingParser *p)
{
    SCEnter();

    AppLayerProtoDetectProbingParserPort *pt = p->port;
    while (pt != NULL) {
        AppLayerProtoDetectProbingParserPort *pt_next = pt->next;
        AppLayerProtoDetectProbingParserPortFree(pt);
        pt = pt_next;
    }

    SCFree(p);

    SCReturn;
}