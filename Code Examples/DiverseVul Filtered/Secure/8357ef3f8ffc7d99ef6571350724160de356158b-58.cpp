static void AppLayerProtoDetectPrintProbingParsers(AppLayerProtoDetectProbingParser *pp)
{
    SCEnter();

    AppLayerProtoDetectProbingParserPort *pp_port = NULL;
    AppLayerProtoDetectProbingParserElement *pp_pe = NULL;

    printf("\nProtocol Detection Configuration\n");

    for ( ; pp != NULL; pp = pp->next) {
        /* print ip protocol */
        if (pp->ipproto == IPPROTO_TCP)
            printf("IPProto: TCP\n");
        else if (pp->ipproto == IPPROTO_UDP)
            printf("IPProto: UDP\n");
        else
            printf("IPProto: %"PRIu8"\n", pp->ipproto);

        pp_port = pp->port;
        for ( ; pp_port != NULL; pp_port = pp_port->next) {
            if (pp_port->dp != NULL) {
                printf("    Port: %"PRIu16 "\n", pp_port->port);

                printf("        Destination port: (max-depth: %"PRIu16 ", "
                        "mask - %"PRIu32")\n",
                        pp_port->dp_max_depth,
                        pp_port->alproto_mask);
                pp_pe = pp_port->dp;
                for ( ; pp_pe != NULL; pp_pe = pp_pe->next) {

                    if (pp_pe->alproto == ALPROTO_HTTP)
                        printf("            alproto: ALPROTO_HTTP\n");
                    else if (pp_pe->alproto == ALPROTO_FTP)
                        printf("            alproto: ALPROTO_FTP\n");
                    else if (pp_pe->alproto == ALPROTO_FTPDATA)
                        printf("            alproto: ALPROTO_FTPDATA\n");
                    else if (pp_pe->alproto == ALPROTO_SMTP)
                        printf("            alproto: ALPROTO_SMTP\n");
                    else if (pp_pe->alproto == ALPROTO_TLS)
                        printf("            alproto: ALPROTO_TLS\n");
                    else if (pp_pe->alproto == ALPROTO_SSH)
                        printf("            alproto: ALPROTO_SSH\n");
                    else if (pp_pe->alproto == ALPROTO_IMAP)
                        printf("            alproto: ALPROTO_IMAP\n");
                    else if (pp_pe->alproto == ALPROTO_MSN)
                        printf("            alproto: ALPROTO_MSN\n");
                    else if (pp_pe->alproto == ALPROTO_JABBER)
                        printf("            alproto: ALPROTO_JABBER\n");
                    else if (pp_pe->alproto == ALPROTO_SMB)
                        printf("            alproto: ALPROTO_SMB\n");
                    else if (pp_pe->alproto == ALPROTO_SMB2)
                        printf("            alproto: ALPROTO_SMB2\n");
                    else if (pp_pe->alproto == ALPROTO_DCERPC)
                        printf("            alproto: ALPROTO_DCERPC\n");
                    else if (pp_pe->alproto == ALPROTO_IRC)
                        printf("            alproto: ALPROTO_IRC\n");
                    else if (pp_pe->alproto == ALPROTO_DNS)
                        printf("            alproto: ALPROTO_DNS\n");
                    else if (pp_pe->alproto == ALPROTO_MODBUS)
                        printf("            alproto: ALPROTO_MODBUS\n");
                    else if (pp_pe->alproto == ALPROTO_ENIP)
                        printf("            alproto: ALPROTO_ENIP\n");
                    else if (pp_pe->alproto == ALPROTO_NFS)
                        printf("            alproto: ALPROTO_NFS\n");
                    else if (pp_pe->alproto == ALPROTO_NTP)
                        printf("            alproto: ALPROTO_NTP\n");
                    else if (pp_pe->alproto == ALPROTO_TFTP)
                        printf("            alproto: ALPROTO_TFTP\n");
                    else if (pp_pe->alproto == ALPROTO_IKEV2)
                        printf("            alproto: ALPROTO_IKEV2\n");
                    else if (pp_pe->alproto == ALPROTO_KRB5)
                        printf("            alproto: ALPROTO_KRB5\n");
                    else if (pp_pe->alproto == ALPROTO_DHCP)
                        printf("            alproto: ALPROTO_DHCP\n");
                    else if (pp_pe->alproto == ALPROTO_TEMPLATE_RUST)
                        printf("            alproto: ALPROTO_TEMPLATE_RUST\n");
                    else if (pp_pe->alproto == ALPROTO_TEMPLATE)
                        printf("            alproto: ALPROTO_TEMPLATE\n");
                    else if (pp_pe->alproto == ALPROTO_DNP3)
                        printf("            alproto: ALPROTO_DNP3\n");
                    else
                        printf("impossible\n");

                    printf("            port: %"PRIu16 "\n", pp_pe->port);
                    printf("            mask: %"PRIu32 "\n", pp_pe->alproto_mask);
                    printf("            min_depth: %"PRIu32 "\n", pp_pe->min_depth);
                    printf("            max_depth: %"PRIu32 "\n", pp_pe->max_depth);

                    printf("\n");
                }
            }

            if (pp_port->sp == NULL) {
                continue;
            }

            printf("        Source port: (max-depth: %"PRIu16 ", "
                   "mask - %"PRIu32")\n",
                   pp_port->sp_max_depth,
                   pp_port->alproto_mask);
            pp_pe = pp_port->sp;
            for ( ; pp_pe != NULL; pp_pe = pp_pe->next) {

                if (pp_pe->alproto == ALPROTO_HTTP)
                    printf("            alproto: ALPROTO_HTTP\n");
                else if (pp_pe->alproto == ALPROTO_FTP)
                    printf("            alproto: ALPROTO_FTP\n");
                else if (pp_pe->alproto == ALPROTO_FTPDATA)
                    printf("            alproto: ALPROTO_FTPDATA\n");
                else if (pp_pe->alproto == ALPROTO_SMTP)
                    printf("            alproto: ALPROTO_SMTP\n");
                else if (pp_pe->alproto == ALPROTO_TLS)
                    printf("            alproto: ALPROTO_TLS\n");
                else if (pp_pe->alproto == ALPROTO_SSH)
                    printf("            alproto: ALPROTO_SSH\n");
                else if (pp_pe->alproto == ALPROTO_IMAP)
                    printf("            alproto: ALPROTO_IMAP\n");
                else if (pp_pe->alproto == ALPROTO_MSN)
                    printf("            alproto: ALPROTO_MSN\n");
                else if (pp_pe->alproto == ALPROTO_JABBER)
                    printf("            alproto: ALPROTO_JABBER\n");
                else if (pp_pe->alproto == ALPROTO_SMB)
                    printf("            alproto: ALPROTO_SMB\n");
                else if (pp_pe->alproto == ALPROTO_SMB2)
                    printf("            alproto: ALPROTO_SMB2\n");
                else if (pp_pe->alproto == ALPROTO_DCERPC)
                    printf("            alproto: ALPROTO_DCERPC\n");
                else if (pp_pe->alproto == ALPROTO_IRC)
                    printf("            alproto: ALPROTO_IRC\n");
                else if (pp_pe->alproto == ALPROTO_DNS)
                    printf("            alproto: ALPROTO_DNS\n");
                else if (pp_pe->alproto == ALPROTO_MODBUS)
                    printf("            alproto: ALPROTO_MODBUS\n");
                else if (pp_pe->alproto == ALPROTO_ENIP)
                    printf("            alproto: ALPROTO_ENIP\n");
                else if (pp_pe->alproto == ALPROTO_NFS)
                    printf("            alproto: ALPROTO_NFS\n");
                else if (pp_pe->alproto == ALPROTO_NTP)
                    printf("            alproto: ALPROTO_NTP\n");
                else if (pp_pe->alproto == ALPROTO_TFTP)
                    printf("            alproto: ALPROTO_TFTP\n");
                else if (pp_pe->alproto == ALPROTO_IKEV2)
                    printf("            alproto: ALPROTO_IKEV2\n");
                else if (pp_pe->alproto == ALPROTO_KRB5)
                    printf("            alproto: ALPROTO_KRB5\n");
                else if (pp_pe->alproto == ALPROTO_DHCP)
                    printf("            alproto: ALPROTO_DHCP\n");
                else if (pp_pe->alproto == ALPROTO_TEMPLATE_RUST)
                    printf("            alproto: ALPROTO_TEMPLATE_RUST\n");
                else if (pp_pe->alproto == ALPROTO_TEMPLATE)
                    printf("            alproto: ALPROTO_TEMPLATE\n");
                else if (pp_pe->alproto == ALPROTO_DNP3)
                    printf("            alproto: ALPROTO_DNP3\n");
                else
                    printf("impossible\n");

                printf("            port: %"PRIu16 "\n", pp_pe->port);
                printf("            mask: %"PRIu32 "\n", pp_pe->alproto_mask);
                printf("            min_depth: %"PRIu32 "\n", pp_pe->min_depth);
                printf("            max_depth: %"PRIu32 "\n", pp_pe->max_depth);

                printf("\n");
            }
        }
    }

    SCReturn;
}