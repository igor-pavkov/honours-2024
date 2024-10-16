static AppLayerProtoDetectProbingParserPort *AppLayerProtoDetectGetProbingParsers(AppLayerProtoDetectProbingParser *pp,
                                                                                  uint8_t ipproto,
                                                                                  uint16_t port)
{
    AppLayerProtoDetectProbingParserPort *pp_port = NULL;

    while (pp != NULL) {
        if (pp->ipproto == ipproto)
            break;

        pp = pp->next;
    }

    if (pp == NULL)
        goto end;

    pp_port = pp->port;
    while (pp_port != NULL) {
        if (pp_port->port == port || pp_port->port == 0) {
            break;
        }
        pp_port = pp_port->next;
    }

 end:
    SCReturnPtr(pp_port, "AppLayerProtoDetectProbingParserPort *");
}