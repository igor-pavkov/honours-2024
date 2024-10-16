static int StreamTcpTest23(void)
{
    StreamTcpThread stt;
    TcpSession ssn;
    Flow f;
    TCPHdr tcph;
    uint8_t packet[1460] = "";
    ThreadVars tv;
    PacketQueue pq;

    Packet *p = SCMalloc(SIZE_OF_PACKET);
    FAIL_IF(p == NULL);

    memset(&pq,0,sizeof(PacketQueue));
    memset(p, 0, SIZE_OF_PACKET);
    memset(&f, 0, sizeof (Flow));
    memset(&tcph, 0, sizeof (TCPHdr));
    memset(&tv, 0, sizeof (ThreadVars));

    StreamTcpUTInit(&stt.ra_ctx);
    StreamTcpUTSetupSession(&ssn);
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
    SET_ISN(&ssn.client, 3184324452UL);

    p->tcph->th_seq = htonl(3184324453UL);
    p->tcph->th_ack = htonl(3373419609UL);
    p->payload_len = 2;

    FAIL_IF(StreamTcpReassembleHandleSegment(&tv, stt.ra_ctx, &ssn, &ssn.client, p, &pq) == -1);

    p->tcph->th_seq = htonl(3184324455UL);
    p->tcph->th_ack = htonl(3373419621UL);
    p->payload_len = 2;

    FAIL_IF(StreamTcpReassembleHandleSegment(&tv, stt.ra_ctx, &ssn, &ssn.client, p, &pq) == -1);

    p->tcph->th_seq = htonl(3184324453UL);
    p->tcph->th_ack = htonl(3373419621UL);
    p->payload_len = 6;

    FAIL_IF(StreamTcpReassembleHandleSegment(&tv, stt.ra_ctx, &ssn, &ssn.client, p, &pq) == -1);

    TcpSegment *seg = RB_MAX(TCPSEG, &ssn.client.seg_tree);
    FAIL_IF_NULL(seg);
    FAIL_IF(TCP_SEG_LEN(seg) != 2);

    StreamTcpUTClearSession(&ssn);
    SCFree(p);
    FLOW_DESTROY(&f);
    StreamTcpUTDeinit(stt.ra_ctx);
    FAIL_IF(SC_ATOMIC_GET(st_memuse) > 0);
    PASS;
}