static void AppLayerProtoDetectProbingParserPortFree(AppLayerProtoDetectProbingParserPort *p)
{
    SCEnter();

    AppLayerProtoDetectProbingParserElement *e;

    e = p->dp;
    while (e != NULL) {
        AppLayerProtoDetectProbingParserElement *e_next = e->next;
        AppLayerProtoDetectProbingParserElementFree(e);
        e = e_next;
    }

    e = p->sp;
    while (e != NULL) {
        AppLayerProtoDetectProbingParserElement *e_next = e->next;
        AppLayerProtoDetectProbingParserElementFree(e);
        e = e_next;
    }

    SCFree(p);

    SCReturn;
}