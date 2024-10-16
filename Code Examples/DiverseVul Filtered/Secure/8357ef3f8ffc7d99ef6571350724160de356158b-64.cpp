int AppLayerProtoDetectPPParseConfPorts(const char *ipproto_name,
                                         uint8_t ipproto,
                                         const char *alproto_name,
                                         AppProto alproto,
                                         uint16_t min_depth, uint16_t max_depth,
                                         ProbingParserFPtr ProbingParserTs,
                                         ProbingParserFPtr ProbingParserTc)
{
    SCEnter();

    char param[100];
    int r;
    ConfNode *node;
    ConfNode *port_node = NULL;
    int config = 0;

    r = snprintf(param, sizeof(param), "%s%s%s", "app-layer.protocols.",
                 alproto_name, ".detection-ports");
    if (r < 0) {
        SCLogError(SC_ERR_FATAL, "snprintf failure.");
        exit(EXIT_FAILURE);
    } else if (r > (int)sizeof(param)) {
        SCLogError(SC_ERR_FATAL, "buffer not big enough to write param.");
        exit(EXIT_FAILURE);
    }
    node = ConfGetNode(param);
    if (node == NULL) {
        SCLogDebug("Entry for %s not found.", param);
        r = snprintf(param, sizeof(param), "%s%s%s%s%s", "app-layer.protocols.",
                     alproto_name, ".", ipproto_name, ".detection-ports");
        if (r < 0) {
            SCLogError(SC_ERR_FATAL, "snprintf failure.");
            exit(EXIT_FAILURE);
        } else if (r > (int)sizeof(param)) {
            SCLogError(SC_ERR_FATAL, "buffer not big enough to write param.");
            exit(EXIT_FAILURE);
        }
        node = ConfGetNode(param);
        if (node == NULL)
            goto end;
    }

    /* detect by destination port of the flow (e.g. port 53 for DNS) */
    port_node = ConfNodeLookupChild(node, "dp");
    if (port_node == NULL)
        port_node = ConfNodeLookupChild(node, "toserver");

    if (port_node != NULL && port_node->val != NULL) {
        AppLayerProtoDetectPPRegister(ipproto,
                                      port_node->val,
                                      alproto,
                                      min_depth, max_depth,
                                      STREAM_TOSERVER, /* to indicate dp */
                                      ProbingParserTs, ProbingParserTc);
    }

    /* detect by source port of flow */
    port_node = ConfNodeLookupChild(node, "sp");
    if (port_node == NULL)
        port_node = ConfNodeLookupChild(node, "toclient");

    if (port_node != NULL && port_node->val != NULL) {
        AppLayerProtoDetectPPRegister(ipproto,
                                      port_node->val,
                                      alproto,
                                      min_depth, max_depth,
                                      STREAM_TOCLIENT, /* to indicate sp */
                                      ProbingParserTc, ProbingParserTs);

    }

    config = 1;
 end:
    SCReturnInt(config);
}