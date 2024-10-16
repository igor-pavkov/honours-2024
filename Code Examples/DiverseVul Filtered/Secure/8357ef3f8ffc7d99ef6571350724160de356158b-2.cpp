static void AppLayerProtoDetectPPGetIpprotos(AppProto alproto,
                                             uint8_t *ipprotos)
{
    SCEnter();

    const AppLayerProtoDetectProbingParser *pp;
    const AppLayerProtoDetectProbingParserPort *pp_port;
    const AppLayerProtoDetectProbingParserElement *pp_pe;

    for (pp = alpd_ctx.ctx_pp; pp != NULL; pp = pp->next) {
        for (pp_port = pp->port; pp_port != NULL; pp_port = pp_port->next) {
            for (pp_pe = pp_port->dp; pp_pe != NULL; pp_pe = pp_pe->next) {
                if (alproto == pp_pe->alproto)
                    ipprotos[pp->ipproto / 8] |= 1 << (pp->ipproto % 8);
            }
            for (pp_pe = pp_port->sp; pp_pe != NULL; pp_pe = pp_pe->next) {
                if (alproto == pp_pe->alproto)
                    ipprotos[pp->ipproto / 8] |= 1 << (pp->ipproto % 8);
            }
        }
    }

    SCReturn;
}