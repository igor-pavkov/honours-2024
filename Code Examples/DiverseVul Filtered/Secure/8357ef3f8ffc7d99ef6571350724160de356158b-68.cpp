static int AppLayerProtoDetectTest15(void)
{
    AppLayerProtoDetectUnittestCtxBackup();
    AppLayerProtoDetectSetup();

    int result = 0;

    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "80",
                                  ALPROTO_HTTP,
                                  5, 8,
                                  STREAM_TOSERVER,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "80",
                                  ALPROTO_SMB,
                                  5, 6,
                                  STREAM_TOSERVER,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "80",
                                  ALPROTO_FTP,
                                  7, 10,
                                  STREAM_TOSERVER,
                                  ProbingParserDummyForTesting, NULL);

    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "81",
                                  ALPROTO_DCERPC,
                                  9, 10,
                                  STREAM_TOSERVER,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "81",
                                  ALPROTO_FTP,
                                  7, 15,
                                  STREAM_TOSERVER,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "0",
                                  ALPROTO_SMTP,
                                  12, 0,
                                  STREAM_TOSERVER,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "0",
                                  ALPROTO_TLS,
                                  12, 18,
                                  STREAM_TOSERVER,
                                  ProbingParserDummyForTesting, NULL);

    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "85",
                                  ALPROTO_DCERPC,
                                  9, 10,
                                  STREAM_TOSERVER,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "85",
                                  ALPROTO_FTP,
                                  7, 15,
                                  STREAM_TOSERVER,
                                  ProbingParserDummyForTesting, NULL);
    result = 1;

    AppLayerProtoDetectPPRegister(IPPROTO_UDP,
                                  "85",
                                  ALPROTO_IMAP,
                                  12, 23,
                                  STREAM_TOSERVER,
                                  ProbingParserDummyForTesting, NULL);

    /* toclient */
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "0",
                                  ALPROTO_JABBER,
                                  12, 23,
                                  STREAM_TOCLIENT,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "0",
                                  ALPROTO_IRC,
                                  12, 14,
                                  STREAM_TOCLIENT,
                                  ProbingParserDummyForTesting, NULL);

    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "85",
                                  ALPROTO_DCERPC,
                                  9, 10,
                                  STREAM_TOCLIENT,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "81",
                                  ALPROTO_FTP,
                                  7, 15,
                                  STREAM_TOCLIENT,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "0",
                                  ALPROTO_TLS,
                                  12, 18,
                                  STREAM_TOCLIENT,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "80",
                                  ALPROTO_HTTP,
                                  5, 8,
                                  STREAM_TOCLIENT,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "81",
                                  ALPROTO_DCERPC,
                                  9, 10,
                                  STREAM_TOCLIENT,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "90",
                                  ALPROTO_FTP,
                                  7, 15,
                                  STREAM_TOCLIENT,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "80",
                                  ALPROTO_SMB,
                                  5, 6,
                                  STREAM_TOCLIENT,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_UDP,
                                  "85",
                                  ALPROTO_IMAP,
                                  12, 23,
                                  STREAM_TOCLIENT,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "0",
                                  ALPROTO_SMTP,
                                  12, 17,
                                  STREAM_TOCLIENT,
                                  ProbingParserDummyForTesting, NULL);
    AppLayerProtoDetectPPRegister(IPPROTO_TCP,
                                  "80",
                                  ALPROTO_FTP,
                                  7, 10,
                                  STREAM_TOCLIENT,
                                  ProbingParserDummyForTesting, NULL);

    AppLayerProtoDetectPPTestDataElement element_ts_80[] = {
        { "http", ALPROTO_HTTP, 80, 1 << ALPROTO_HTTP, 5, 8 },
          { "smb", ALPROTO_SMB, 80, 1 << ALPROTO_SMB, 5, 6 },
          { "ftp", ALPROTO_FTP, 80, 1 << ALPROTO_FTP, 7, 10 },
          { "smtp", ALPROTO_SMTP, 0, 1 << ALPROTO_SMTP, 12, 0 },
          { "tls", ALPROTO_TLS, 0, 1 << ALPROTO_TLS, 12, 18 },
          { "irc", ALPROTO_IRC, 0, 1 << ALPROTO_IRC, 12, 25 },
          { "jabber", ALPROTO_JABBER, 0, 1 << ALPROTO_JABBER, 12, 23 },
        };
    AppLayerProtoDetectPPTestDataElement element_tc_80[] = {
        { "http", ALPROTO_HTTP, 80, 1 << ALPROTO_HTTP, 5, 8 },
          { "smb", ALPROTO_SMB, 80, 1 << ALPROTO_SMB, 5, 6 },
          { "ftp", ALPROTO_FTP, 80, 1 << ALPROTO_FTP, 7, 10 },
          { "jabber", ALPROTO_JABBER, 0, 1 << ALPROTO_JABBER, 12, 23 },
          { "irc", ALPROTO_IRC, 0, 1 << ALPROTO_IRC, 12, 14 },
          { "tls", ALPROTO_TLS, 0, 1 << ALPROTO_TLS, 12, 18 },
          { "smtp", ALPROTO_SMTP, 0, 1 << ALPROTO_SMTP, 12, 17 }
        };

    AppLayerProtoDetectPPTestDataElement element_ts_81[] = {
        { "dcerpc", ALPROTO_DCERPC, 81, 1 << ALPROTO_DCERPC, 9, 10 },
          { "ftp", ALPROTO_FTP, 81, 1 << ALPROTO_FTP, 7, 15 },
          { "smtp", ALPROTO_SMTP, 0, 1 << ALPROTO_SMTP, 12, 0 },
          { "tls", ALPROTO_TLS, 0, 1 << ALPROTO_TLS, 12, 18 },
          { "irc", ALPROTO_IRC, 0, 1 << ALPROTO_IRC, 12, 25 },
          { "jabber", ALPROTO_JABBER, 0, 1 << ALPROTO_JABBER, 12, 23 },
        };
    AppLayerProtoDetectPPTestDataElement element_tc_81[] = {
        { "ftp", ALPROTO_FTP, 81, 1 << ALPROTO_FTP, 7, 15 },
          { "dcerpc", ALPROTO_DCERPC, 81, 1 << ALPROTO_DCERPC, 9, 10 },
          { "jabber", ALPROTO_JABBER, 0, 1 << ALPROTO_JABBER, 12, 23 },
          { "irc", ALPROTO_IRC, 0, 1 << ALPROTO_IRC, 12, 14 },
          { "tls", ALPROTO_TLS, 0, 1 << ALPROTO_TLS, 12, 18 },
          { "smtp", ALPROTO_SMTP, 0, 1 << ALPROTO_SMTP, 12, 17 }
        };

    AppLayerProtoDetectPPTestDataElement element_ts_85[] = {
        { "dcerpc", ALPROTO_DCERPC, 85, 1 << ALPROTO_DCERPC, 9, 10 },
          { "ftp", ALPROTO_FTP, 85, 1 << ALPROTO_FTP, 7, 15 },
          { "smtp", ALPROTO_SMTP, 0, 1 << ALPROTO_SMTP, 12, 0 },
          { "tls", ALPROTO_TLS, 0, 1 << ALPROTO_TLS, 12, 18 },
          { "irc", ALPROTO_IRC, 0, 1 << ALPROTO_IRC, 12, 25 },
          { "jabber", ALPROTO_JABBER, 0, 1 << ALPROTO_JABBER, 12, 23 },
        };
    AppLayerProtoDetectPPTestDataElement element_tc_85[] = {
        { "dcerpc", ALPROTO_DCERPC, 85, 1 << ALPROTO_DCERPC, 9, 10 },
          { "jabber", ALPROTO_JABBER, 0, 1 << ALPROTO_JABBER, 12, 23 },
          { "irc", ALPROTO_IRC, 0, 1 << ALPROTO_IRC, 12, 14 },
          { "tls", ALPROTO_TLS, 0, 1 << ALPROTO_TLS, 12, 18 },
          { "smtp", ALPROTO_SMTP, 0, 1 << ALPROTO_SMTP, 12, 17 }
        };

    AppLayerProtoDetectPPTestDataElement element_ts_90[] = {
        { "smtp", ALPROTO_SMTP, 0, 1 << ALPROTO_SMTP, 12, 0 },
          { "tls", ALPROTO_TLS, 0, 1 << ALPROTO_TLS, 12, 18 },
          { "irc", ALPROTO_IRC, 0, 1 << ALPROTO_IRC, 12, 25 },
          { "jabber", ALPROTO_JABBER, 0, 1 << ALPROTO_JABBER, 12, 23 },
        };
    AppLayerProtoDetectPPTestDataElement element_tc_90[] = {
        { "ftp", ALPROTO_FTP, 90, 1 << ALPROTO_FTP, 7, 15 },
          { "jabber", ALPROTO_JABBER, 0, 1 << ALPROTO_JABBER, 12, 23 },
          { "irc", ALPROTO_IRC, 0, 1 << ALPROTO_IRC, 12, 14 },
          { "tls", ALPROTO_TLS, 0, 1 << ALPROTO_TLS, 12, 18 },
          { "smtp", ALPROTO_SMTP, 0, 1 << ALPROTO_SMTP, 12, 17 }
        };

    AppLayerProtoDetectPPTestDataElement element_ts_0[] = {
        { "smtp", ALPROTO_SMTP, 0, 1 << ALPROTO_SMTP, 12, 0 },
          { "tls", ALPROTO_TLS, 0, 1 << ALPROTO_TLS, 12, 18 },
          { "irc", ALPROTO_IRC, 0, 1 << ALPROTO_IRC, 12, 25 },
          { "jabber", ALPROTO_JABBER, 0, 1 << ALPROTO_JABBER, 12, 23 },
        };
    AppLayerProtoDetectPPTestDataElement element_tc_0[] = {
        { "jabber", ALPROTO_JABBER, 0, 1 << ALPROTO_JABBER, 12, 23 },
          { "irc", ALPROTO_IRC, 0, 1 << ALPROTO_IRC, 12, 14 },
          { "tls", ALPROTO_TLS, 0, 1 << ALPROTO_TLS, 12, 18 },
          { "smtp", ALPROTO_SMTP, 0, 1 << ALPROTO_SMTP, 12, 17 }
        };


    AppLayerProtoDetectPPTestDataElement element_ts_85_udp[] = {
        { "imap", ALPROTO_IMAP, 85, 1 << ALPROTO_IMAP, 12, 23 },
        };
    AppLayerProtoDetectPPTestDataElement element_tc_85_udp[] = {
        { "imap", ALPROTO_IMAP, 85, 1 << ALPROTO_IMAP, 12, 23 },
        };

    AppLayerProtoDetectPPTestDataPort ports_tcp[] = {
        { 80,
          ((1 << ALPROTO_HTTP) | (1 << ALPROTO_SMB) | (1 << ALPROTO_FTP) |
           (1 << ALPROTO_SMTP) | (1 << ALPROTO_TLS) | (1 << ALPROTO_IRC) | (1 << ALPROTO_JABBER)),
          ((1 << ALPROTO_HTTP) | (1 << ALPROTO_SMB) | (1 << ALPROTO_FTP) |
           (1 << ALPROTO_JABBER) | (1 << ALPROTO_IRC) | (1 << ALPROTO_TLS) | (1 << ALPROTO_SMTP)),
          23,
          element_ts_80, element_tc_80,
          sizeof(element_ts_80) / sizeof(AppLayerProtoDetectPPTestDataElement),
          sizeof(element_tc_80) / sizeof(AppLayerProtoDetectPPTestDataElement),
        },
        { 81,
          ((1 << ALPROTO_DCERPC) | (1 << ALPROTO_FTP) |
           (1 << ALPROTO_SMTP) | (1 << ALPROTO_TLS) | (1 << ALPROTO_IRC) | (1 << ALPROTO_JABBER)),
          ((1 << ALPROTO_FTP) | (1 << ALPROTO_DCERPC) |
           (1 << ALPROTO_JABBER) | (1 << ALPROTO_IRC) | (1 << ALPROTO_TLS) | (1 << ALPROTO_SMTP)),
          23,
          element_ts_81, element_tc_81,
          sizeof(element_ts_81) / sizeof(AppLayerProtoDetectPPTestDataElement),
          sizeof(element_tc_81) / sizeof(AppLayerProtoDetectPPTestDataElement),
        },
        { 85,
          ((1 << ALPROTO_DCERPC) | (1 << ALPROTO_FTP) |
           (1 << ALPROTO_SMTP) | (1 << ALPROTO_TLS) | (1 << ALPROTO_IRC) | (1 << ALPROTO_JABBER)),
          ((1 << ALPROTO_DCERPC) |
           (1 << ALPROTO_JABBER) | (1 << ALPROTO_IRC) | (1 << ALPROTO_TLS) | (1 << ALPROTO_SMTP)),
          23,
          element_ts_85, element_tc_85,
          sizeof(element_ts_85) / sizeof(AppLayerProtoDetectPPTestDataElement),
          sizeof(element_tc_85) / sizeof(AppLayerProtoDetectPPTestDataElement)
        },
        { 90,
          ((1 << ALPROTO_SMTP) | (1 << ALPROTO_TLS) | (1 << ALPROTO_IRC) | (1 << ALPROTO_JABBER)),
          ((1 << ALPROTO_FTP) |
           (1 << ALPROTO_JABBER) | (1 << ALPROTO_IRC) | (1 << ALPROTO_TLS) | (1 << ALPROTO_SMTP)),
          23,
          element_ts_90, element_tc_90,
          sizeof(element_ts_90) / sizeof(AppLayerProtoDetectPPTestDataElement),
          sizeof(element_tc_90) / sizeof(AppLayerProtoDetectPPTestDataElement)
        },
        { 0,
          ((1 << ALPROTO_SMTP) | (1 << ALPROTO_TLS) | (1 << ALPROTO_IRC) | (1 << ALPROTO_JABBER)),
          ((1 << ALPROTO_JABBER) | (1 << ALPROTO_IRC) | (1 << ALPROTO_TLS) | (1 << ALPROTO_SMTP)),
          23,
          element_ts_0, element_tc_0,
          sizeof(element_ts_0) / sizeof(AppLayerProtoDetectPPTestDataElement),
          sizeof(element_tc_0) / sizeof(AppLayerProtoDetectPPTestDataElement)
        }
    };

    AppLayerProtoDetectPPTestDataPort ports_udp[] = {
        { 85,
            (1 << ALPROTO_IMAP),
            (1 << ALPROTO_IMAP),
            23,
            element_ts_85_udp, element_tc_85_udp,
            sizeof(element_ts_85_udp) / sizeof(AppLayerProtoDetectPPTestDataElement),
            sizeof(element_tc_85_udp) / sizeof(AppLayerProtoDetectPPTestDataElement),
            },
        };

    AppLayerProtoDetectPPTestDataIPProto ip_proto[] = {
        { IPPROTO_TCP,
          ports_tcp,
          sizeof(ports_tcp) / sizeof(AppLayerProtoDetectPPTestDataPort),
        },
        { IPPROTO_UDP,
          ports_udp,
          sizeof(ports_udp) / sizeof(AppLayerProtoDetectPPTestDataPort),
        },
    };


    if (AppLayerProtoDetectPPTestData(alpd_ctx.ctx_pp, ip_proto,
                                      sizeof(ip_proto) / sizeof(AppLayerProtoDetectPPTestDataIPProto)) == 0) {
        goto end;
    }
    result = 1;

 end:
    AppLayerProtoDetectDeSetup();
    AppLayerProtoDetectUnittestCtxRestore();
    return result;
}