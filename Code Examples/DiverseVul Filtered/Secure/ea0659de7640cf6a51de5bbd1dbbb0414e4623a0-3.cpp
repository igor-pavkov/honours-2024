void StreamTcpRegisterTests (void)
{
#ifdef UNITTESTS
    UtRegisterTest("StreamTcpTest01 -- TCP session allocation",
                   StreamTcpTest01);
    UtRegisterTest("StreamTcpTest02 -- TCP session deallocation",
                   StreamTcpTest02);
    UtRegisterTest("StreamTcpTest03 -- SYN missed MidStream session",
                   StreamTcpTest03);
    UtRegisterTest("StreamTcpTest04 -- SYN/ACK missed MidStream session",
                   StreamTcpTest04);
    UtRegisterTest("StreamTcpTest05 -- 3WHS missed MidStream session",
                   StreamTcpTest05);
    UtRegisterTest("StreamTcpTest06 -- FIN, RST message MidStream session",
                   StreamTcpTest06);
    UtRegisterTest("StreamTcpTest07 -- PAWS invalid timestamp",
                   StreamTcpTest07);
    UtRegisterTest("StreamTcpTest08 -- PAWS valid timestamp", StreamTcpTest08);
    UtRegisterTest("StreamTcpTest09 -- No Client Reassembly", StreamTcpTest09);
    UtRegisterTest("StreamTcpTest10 -- No missed packet Async stream",
                   StreamTcpTest10);
    UtRegisterTest("StreamTcpTest11 -- SYN missed Async stream",
                   StreamTcpTest11);
    UtRegisterTest("StreamTcpTest12 -- SYN/ACK missed Async stream",
                   StreamTcpTest12);
    UtRegisterTest("StreamTcpTest13 -- opposite stream packets for Async " "stream",
                   StreamTcpTest13);
    UtRegisterTest("StreamTcp4WHSTest01", StreamTcp4WHSTest01);
    UtRegisterTest("StreamTcp4WHSTest02", StreamTcp4WHSTest02);
    UtRegisterTest("StreamTcp4WHSTest03", StreamTcp4WHSTest03);
    UtRegisterTest("StreamTcpTest14 -- setup OS policy", StreamTcpTest14);
    UtRegisterTest("StreamTcpTest15 -- setup OS policy", StreamTcpTest15);
    UtRegisterTest("StreamTcpTest16 -- setup OS policy", StreamTcpTest16);
    UtRegisterTest("StreamTcpTest17 -- setup OS policy", StreamTcpTest17);
    UtRegisterTest("StreamTcpTest18 -- setup OS policy", StreamTcpTest18);
    UtRegisterTest("StreamTcpTest19 -- setup OS policy", StreamTcpTest19);
    UtRegisterTest("StreamTcpTest20 -- setup OS policy", StreamTcpTest20);
    UtRegisterTest("StreamTcpTest21 -- setup OS policy", StreamTcpTest21);
    UtRegisterTest("StreamTcpTest22 -- setup OS policy", StreamTcpTest22);
    UtRegisterTest("StreamTcpTest23 -- stream memory leaks", StreamTcpTest23);
    UtRegisterTest("StreamTcpTest24 -- stream memory leaks", StreamTcpTest24);
    UtRegisterTest("StreamTcpTest25 -- test ecn/cwr sessions",
                   StreamTcpTest25);
    UtRegisterTest("StreamTcpTest26 -- test ecn/cwr sessions",
                   StreamTcpTest26);
    UtRegisterTest("StreamTcpTest27 -- test ecn/cwr sessions",
                   StreamTcpTest27);
    UtRegisterTest("StreamTcpTest28 -- Memcap Test", StreamTcpTest28);

#if 0 /* VJ 2010/09/01 disabled since they blow up on Fedora and Fedora is
       * right about blowing up. The checksum functions are not used properly
       * in the tests. */
    UtRegisterTest("StreamTcpTest29 -- Badchecksum Reset Test", StreamTcpTest29, 1);
    UtRegisterTest("StreamTcpTest30 -- Badchecksum Overlap Test", StreamTcpTest30, 1);
    UtRegisterTest("StreamTcpTest31 -- MultipleSyns Test", StreamTcpTest31, 1);
    UtRegisterTest("StreamTcpTest32 -- Bogus CWR Test", StreamTcpTest32, 1);
    UtRegisterTest("StreamTcpTest33 -- RST-SYN Again Test", StreamTcpTest33, 1);
    UtRegisterTest("StreamTcpTest34 -- SYN-PUSH Test", StreamTcpTest34, 1);
    UtRegisterTest("StreamTcpTest35 -- SYN-URG Test", StreamTcpTest35, 1);
    UtRegisterTest("StreamTcpTest36 -- PUSH-URG Test", StreamTcpTest36, 1);
#endif
    UtRegisterTest("StreamTcpTest37 -- Out of order FIN Test",
                   StreamTcpTest37);

    UtRegisterTest("StreamTcpTest38 -- validate ACK", StreamTcpTest38);
    UtRegisterTest("StreamTcpTest39 -- update next_seq", StreamTcpTest39);

    UtRegisterTest("StreamTcpTest42 -- SYN/ACK queue", StreamTcpTest42);
    UtRegisterTest("StreamTcpTest43 -- SYN/ACK queue", StreamTcpTest43);
    UtRegisterTest("StreamTcpTest44 -- SYN/ACK queue", StreamTcpTest44);
    UtRegisterTest("StreamTcpTest45 -- SYN/ACK queue", StreamTcpTest45);

    /* set up the reassembly tests as well */
    StreamTcpReassembleRegisterTests();

    StreamTcpSackRegisterTests ();
#endif /* UNITTESTS */
}