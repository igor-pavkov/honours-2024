void ImapModelOpenConnectionTest::testOkMissingImap4rev1()
{
    SOCK->fakeReading( "* OK [CAPABILITY pwn] foo\r\n" );
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE(SOCK->writtenStuff(), QByteArray("y0 LOGOUT\r\n"));
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(authSpy->isEmpty());
    QVERIFY(SOCK->writtenStuff().isEmpty());
    QVERIFY(startTlsUpgradeSpy->isEmpty());
}