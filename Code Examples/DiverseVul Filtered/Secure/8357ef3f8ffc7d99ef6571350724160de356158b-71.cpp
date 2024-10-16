static int AppLayerProtoDetectPPTestData(AppLayerProtoDetectProbingParser *pp,
                                         AppLayerProtoDetectPPTestDataIPProto *ip_proto,
                                         int no_of_ip_proto)
{
    int result = 0;
    int i = -1, j = -1 , k = -1;
#ifdef DEBUG
    int dir = 0;
#endif
    for (i = 0; i < no_of_ip_proto; i++, pp = pp->next) {
        if (pp->ipproto != ip_proto[i].ipproto)
            goto end;

        AppLayerProtoDetectProbingParserPort *pp_port = pp->port;
        for (k = 0; k < ip_proto[i].no_of_port; k++, pp_port = pp_port->next) {
            if (pp_port->port != ip_proto[i].port[k].port)
                goto end;
            if (pp_port->alproto_mask != ip_proto[i].port[k].alproto_mask)
                goto end;
            if (pp_port->alproto_mask != ip_proto[i].port[k].alproto_mask)
                goto end;
            if (pp_port->dp_max_depth != ip_proto[i].port[k].dp_max_depth)
                goto end;
            if (pp_port->sp_max_depth != ip_proto[i].port[k].sp_max_depth)
                goto end;

            AppLayerProtoDetectProbingParserElement *pp_element = pp_port->dp;
#ifdef DEBUG
            dir = 0;
#endif
            for (j = 0 ; j < ip_proto[i].port[k].ts_no_of_element;
                 j++, pp_element = pp_element->next) {

                if (pp_element->alproto != ip_proto[i].port[k].toserver_element[j].alproto) {
                    goto end;
                }
                if (pp_element->port != ip_proto[i].port[k].toserver_element[j].port) {
                    goto end;
                }
                if (pp_element->alproto_mask != ip_proto[i].port[k].toserver_element[j].alproto_mask) {
                    goto end;
                }
                if (pp_element->min_depth != ip_proto[i].port[k].toserver_element[j].min_depth) {
                    goto end;
                }
                if (pp_element->max_depth != ip_proto[i].port[k].toserver_element[j].max_depth) {
                    goto end;
                }
            } /* for */
            if (pp_element != NULL)
                goto end;

            pp_element = pp_port->sp;
#ifdef DEBUG
            dir = 1;
#endif
            for (j = 0 ; j < ip_proto[i].port[k].tc_no_of_element; j++, pp_element = pp_element->next) {
                if (pp_element->alproto != ip_proto[i].port[k].toclient_element[j].alproto) {
                    goto end;
                }
                if (pp_element->port != ip_proto[i].port[k].toclient_element[j].port) {
                    goto end;
                }
                if (pp_element->alproto_mask != ip_proto[i].port[k].toclient_element[j].alproto_mask) {
                    goto end;
                }
                if (pp_element->min_depth != ip_proto[i].port[k].toclient_element[j].min_depth) {
                    goto end;
                }
                if (pp_element->max_depth != ip_proto[i].port[k].toclient_element[j].max_depth) {
                    goto end;
                }
            } /* for */
            if (pp_element != NULL)
                goto end;
        }
        if (pp_port != NULL)
            goto end;
    }
    if (pp != NULL)
        goto end;

    result = 1;
 end:
#ifdef DEBUG
    printf("i = %d, k = %d, j = %d(%s)\n", i, k, j, (dir == 0) ? "ts" : "tc");
#endif
    return result;
}