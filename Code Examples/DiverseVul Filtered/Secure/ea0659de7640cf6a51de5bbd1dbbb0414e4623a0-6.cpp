static int StreamTcpTest24(void)
{
    StreamTcpThread stt;
    TcpSession ssn;
    Packet *p = SCMalloc(SIZE_OF_PACKET);
    FAIL_IF (p == NULL);
    Flow f;
    TCPHdr tcph;
    uint8_t packet[1460] = "";
    ThreadVars tv;
    PacketQueue pq;
    memset(&pq,0,sizeof(PacketQueue));

    StreamTcpUTInit(&stt.ra_ctx);
    StreamTcpUTSetupSession(&ssn);

    memset(p, 0, SIZE_OF_PACKET);
    memset(&f, 0, sizeof (Flow));
    memset(&tv, 0, sizeof (ThreadVars));
    memset(&tcph, 0, sizeof (TCPHdr));
    FLOW_INITIALIZE(&f);
    ssn.client.os_policy = OS_POLICY_BSD;
    f.protoctx = &ssn;
    p->src.family = AF_INET;
    p->dst.family = AF_INET;
    p->proto = IPPROTO_TCP;
    p->flow = &f;
    tcph.th_win = 5480;
    tcph.th_flags = TH_PUSH | TH_ACK;
    p->tcph = &tcph;
    p->flowflags = FLOW_PKT_TOSERVER;
    p->payload = packet;
    //ssn.client.ra_app_base_seq = ssn.client.ra_raw_base_seq = ssn.client.last_ack = 3184324453UL;
    SET_ISN(&ssn.client, 3184324453UL);

    p->tcph->th_seq = htonl(3184324455UL);
    p->tcph->th_ack = htonl(3373419621UL);
    p->payload_len = 4;

    FAIL_IF (StreamTcpReassembleHandleSegment(&tv, stt.ra_ctx, &ssn, &ssn.client, p, &pq) == -1);

    p->tcph->th_seq = htonl(3184324459UL);
    p->tcph->th_ack = htonl(3373419633UL);
    p->payload_len = 2;

    FAIL_IF(StreamTcpReassembleHandleSegment(&tv, stt.ra_ctx, &ssn, &ssn.client, p, &pq) == -1);

    p->tcph->th_seq = htonl(3184324459UL);
    p->tcph->th_ack = htonl(3373419657UL);
    p->payload_len = 4;

    FAIL_IF(StreamTcpReassembleHandleSegment(&tv, stt.ra_ctx, &ssn, &ssn.client, p, &pq) == -1);

    TcpSegment *seg = RB_MAX(TCPSEG, &ssn.client.seg_tree);
    FAIL_IF_NULL(seg);
    FAIL_IF(TCP_SEG_LEN(seg) != 4);

    StreamTcpUTClearSession(&ssn);
    SCFree(p);
    FLOW_DESTROY(&f);
    StreamTcpUTDeinit(stt.ra_ctx);
    FAIL_IF(SC_ATOMIC_GET(st_memuse) > 0);
    PASS;
}