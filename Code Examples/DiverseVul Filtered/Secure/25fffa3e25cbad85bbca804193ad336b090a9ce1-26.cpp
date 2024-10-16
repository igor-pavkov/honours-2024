void ImapModelOpenConnectionTest::testOkStartTlsForbidden()
{
    cleanup(); init(true); // yuck, but I can't come up with anything better...

    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(SOCK->writtenStuff().isEmpty());
    SOCK->fakeReading( "* OK foo\r\n" );
    QVERIFY(completedSpy->isEmpty());
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE(SOCK->writtenStuff(), QByteArray("y0 CAPABILITY\r\n"));
    SOCK->fakeReading("* CAPABILITY imap4rev1\r\ny0 ok cap\r\n");
    QVERIFY(authSpy->isEmpty());
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE(failedSpy->size(), 1);
    QVERIFY(authSpy->isEmpty());
    QVERIFY(startTlsUpgradeSpy->isEmpty());
}