void ImapModelOpenConnectionTest::testOkWithCapability()
{
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(SOCK->writtenStuff().isEmpty());
    SOCK->fakeReading( "* OK [CAPABILITY IMAP4rev1] foo\r\n" );
    QVERIFY( completedSpy->isEmpty() );
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE( authSpy->size(), 1 );
    QCOMPARE( SOCK->writtenStuff(), QByteArray("y0 LOGIN luzr sikrit\r\n") );
    SOCK->fakeReading( "y0 OK [CAPABILITY IMAP4rev1] logged in\r\n");
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE( completedSpy->size(), 1 );
    QVERIFY(failedSpy->isEmpty());
    QCOMPARE( authSpy->size(), 1 );
    QVERIFY(startTlsUpgradeSpy->isEmpty());
}