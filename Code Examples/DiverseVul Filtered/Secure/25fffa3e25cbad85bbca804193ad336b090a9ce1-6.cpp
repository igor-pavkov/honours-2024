void ImapModelOpenConnectionTest::testPreauthWithStartTlsWanted()
{
    cleanup(); init(true); // yuck, but I can't come up with anything better...

    cEmpty();
    cServer("* PREAUTH hi there\r\n");
    QCOMPARE(failedSpy->size(), 1);
    QVERIFY(completedSpy->isEmpty());
    QVERIFY(authSpy->isEmpty());
    QVERIFY(startTlsUpgradeSpy->isEmpty());
}