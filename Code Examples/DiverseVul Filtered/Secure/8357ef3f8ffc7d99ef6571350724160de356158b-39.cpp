void AppLayerProtoDetectPPRegister(uint8_t ipproto,
                                   const char *portstr,
                                   AppProto alproto,
                                   uint16_t min_depth, uint16_t max_depth,
                                   uint8_t direction,
                                   ProbingParserFPtr ProbingParser1,
                                   ProbingParserFPtr ProbingParser2)
{
    SCEnter();

    DetectPort *head = NULL;
    DetectPortParse(NULL,&head, portstr);
    DetectPort *temp_dp = head;
    while (temp_dp != NULL) {
        uint32_t port = temp_dp->port;
        if (port == 0 && temp_dp->port2 != 0)
            port++;
        for ( ; port <= temp_dp->port2; port++) {
            AppLayerProtoDetectInsertNewProbingParser(&alpd_ctx.ctx_pp,
                                                      ipproto,
                                                      port,
                                                      alproto,
                                                      min_depth, max_depth,
                                                      direction,
                                                      ProbingParser1,
                                                      ProbingParser2);
        }
        temp_dp = temp_dp->next;
    }
    DetectPortCleanupList(NULL,head);

    SCReturn;
}