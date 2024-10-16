static void AppLayerProtoDetectInsertNewProbingParser(AppLayerProtoDetectProbingParser **pp,
                                                             uint8_t ipproto,
                                                             uint16_t port,
                                                             AppProto alproto,
                                                             uint16_t min_depth, uint16_t max_depth,
                                                             uint8_t direction,
                                                             ProbingParserFPtr ProbingParser1,
                                                             ProbingParserFPtr ProbingParser2)
{
    SCEnter();

    /* get the top level ipproto pp */
    AppLayerProtoDetectProbingParser *curr_pp = *pp;
    while (curr_pp != NULL) {
        if (curr_pp->ipproto == ipproto)
            break;
        curr_pp = curr_pp->next;
    }
    if (curr_pp == NULL) {
        AppLayerProtoDetectProbingParser *new_pp = AppLayerProtoDetectProbingParserAlloc();
        new_pp->ipproto = ipproto;
        AppLayerProtoDetectProbingParserAppend(pp, new_pp);
        curr_pp = new_pp;
    }

    /* get the top level port pp */
    AppLayerProtoDetectProbingParserPort *curr_port = curr_pp->port;
    while (curr_port != NULL) {
        if (curr_port->port == port)
            break;
        curr_port = curr_port->next;
    }
    if (curr_port == NULL) {
        AppLayerProtoDetectProbingParserPort *new_port = AppLayerProtoDetectProbingParserPortAlloc();
        new_port->port = port;
        AppLayerProtoDetectProbingParserPortAppend(&curr_pp->port, new_port);
        curr_port = new_port;
        if (direction & STREAM_TOSERVER) {
            curr_port->dp_max_depth = max_depth;
        } else {
            curr_port->sp_max_depth = max_depth;
        }

        AppLayerProtoDetectProbingParserPort *zero_port;

        zero_port = curr_pp->port;
        while (zero_port != NULL && zero_port->port != 0) {
            zero_port = zero_port->next;
        }
        if (zero_port != NULL) {
            AppLayerProtoDetectProbingParserElement *zero_pe;

            zero_pe = zero_port->dp;
            for ( ; zero_pe != NULL; zero_pe = zero_pe->next) {
                if (curr_port->dp == NULL)
                    curr_port->dp_max_depth = zero_pe->max_depth;
                if (zero_pe->max_depth == 0)
                    curr_port->dp_max_depth = zero_pe->max_depth;
                if (curr_port->dp_max_depth != 0 &&
                    curr_port->dp_max_depth < zero_pe->max_depth) {
                    curr_port->dp_max_depth = zero_pe->max_depth;
                }

                AppLayerProtoDetectProbingParserElement *dup_pe =
                    AppLayerProtoDetectProbingParserElementDuplicate(zero_pe);
                AppLayerProtoDetectProbingParserElementAppend(&curr_port->dp, dup_pe);
                curr_port->alproto_mask |= dup_pe->alproto_mask;
            }

            zero_pe = zero_port->sp;
            for ( ; zero_pe != NULL; zero_pe = zero_pe->next) {
                if (curr_port->sp == NULL)
                    curr_port->sp_max_depth = zero_pe->max_depth;
                if (zero_pe->max_depth == 0)
                    curr_port->sp_max_depth = zero_pe->max_depth;
                if (curr_port->sp_max_depth != 0 &&
                    curr_port->sp_max_depth < zero_pe->max_depth) {
                    curr_port->sp_max_depth = zero_pe->max_depth;
                }

                AppLayerProtoDetectProbingParserElement *dup_pe =
                    AppLayerProtoDetectProbingParserElementDuplicate(zero_pe);
                AppLayerProtoDetectProbingParserElementAppend(&curr_port->sp, dup_pe);
                curr_port->alproto_mask |= dup_pe->alproto_mask;
            }
        } /* if (zero_port != NULL) */
    } /* if (curr_port == NULL) */

    /* insert the pe_pp */
    AppLayerProtoDetectProbingParserElement *curr_pe;
    if (direction & STREAM_TOSERVER)
        curr_pe = curr_port->dp;
    else
        curr_pe = curr_port->sp;
    while (curr_pe != NULL) {
        if (curr_pe->alproto == alproto) {
            SCLogError(SC_ERR_ALPARSER, "Duplicate pp registered - "
                       "ipproto - %"PRIu8" Port - %"PRIu16" "
                       "App Protocol - NULL, App Protocol(ID) - "
                       "%"PRIu16" min_depth - %"PRIu16" "
                       "max_dept - %"PRIu16".",
                       ipproto, port, alproto,
                       min_depth, max_depth);
            goto error;
        }
        curr_pe = curr_pe->next;
    }
    /* Get a new parser element */
    AppLayerProtoDetectProbingParserElement *new_pe =
        AppLayerProtoDetectProbingParserElementCreate(alproto,
                                                      curr_port->port,
                                                      min_depth, max_depth);
    if (new_pe == NULL)
        goto error;
    curr_pe = new_pe;
    AppLayerProtoDetectProbingParserElement **head_pe;
    if (direction & STREAM_TOSERVER) {
        curr_pe->ProbingParserTs = ProbingParser1;
        curr_pe->ProbingParserTc = ProbingParser2;
        if (curr_port->dp == NULL)
            curr_port->dp_max_depth = new_pe->max_depth;
        if (new_pe->max_depth == 0)
            curr_port->dp_max_depth = new_pe->max_depth;
        if (curr_port->dp_max_depth != 0 &&
            curr_port->dp_max_depth < new_pe->max_depth) {
            curr_port->dp_max_depth = new_pe->max_depth;
        }
        curr_port->alproto_mask |= new_pe->alproto_mask;
        head_pe = &curr_port->dp;
    } else {
        curr_pe->ProbingParserTs = ProbingParser2;
        curr_pe->ProbingParserTc = ProbingParser1;
        if (curr_port->sp == NULL)
            curr_port->sp_max_depth = new_pe->max_depth;
        if (new_pe->max_depth == 0)
            curr_port->sp_max_depth = new_pe->max_depth;
        if (curr_port->sp_max_depth != 0 &&
            curr_port->sp_max_depth < new_pe->max_depth) {
            curr_port->sp_max_depth = new_pe->max_depth;
        }
        curr_port->alproto_mask |= new_pe->alproto_mask;
        head_pe = &curr_port->sp;
    }
    AppLayerProtoDetectProbingParserElementAppend(head_pe, new_pe);

    if (curr_port->port == 0) {
        AppLayerProtoDetectProbingParserPort *temp_port = curr_pp->port;
        while (temp_port != NULL && temp_port->port != 0) {
            if (direction & STREAM_TOSERVER) {
                if (temp_port->dp == NULL)
                    temp_port->dp_max_depth = curr_pe->max_depth;
                if (curr_pe->max_depth == 0)
                    temp_port->dp_max_depth = curr_pe->max_depth;
                if (temp_port->dp_max_depth != 0 &&
                    temp_port->dp_max_depth < curr_pe->max_depth) {
                    temp_port->dp_max_depth = curr_pe->max_depth;
                }
                AppLayerProtoDetectProbingParserElementAppend(&temp_port->dp,
                                                              AppLayerProtoDetectProbingParserElementDuplicate(curr_pe));
                temp_port->alproto_mask |= curr_pe->alproto_mask;
            } else {
                if (temp_port->sp == NULL)
                    temp_port->sp_max_depth = curr_pe->max_depth;
                if (curr_pe->max_depth == 0)
                    temp_port->sp_max_depth = curr_pe->max_depth;
                if (temp_port->sp_max_depth != 0 &&
                    temp_port->sp_max_depth < curr_pe->max_depth) {
                    temp_port->sp_max_depth = curr_pe->max_depth;
                }
                AppLayerProtoDetectProbingParserElementAppend(&temp_port->sp,
                                                              AppLayerProtoDetectProbingParserElementDuplicate(curr_pe));
                temp_port->alproto_mask |= curr_pe->alproto_mask;
            }
            temp_port = temp_port->next;
        } /* while */
    } /* if */

 error:
    SCReturn;
}