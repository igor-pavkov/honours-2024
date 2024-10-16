void ImapModelOpenConnectionTest::testPreauth()
{
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(SOCK->writtenStuff().isEmpty());
    SOCK->fakeReading( "* PREAUTH foo\r\n" );
    QVERIFY( completedSpy->isEmpty() );
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE( SOCK->writtenStuff(), QByteArray("y0 CAPABILITY\r\n") );
    QVERIFY( completedSpy->isEmpty() );
    SOCK->fakeReading( "* CAPABILITY IMAP4rev1\r\ny0 OK capability completed\r\n" );
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE( completedSpy->size(), 1 );
    QVERIFY(failedSpy->isEmpty());
    QVERIFY( authSpy->isEmpty() );
    QVERIFY(startTlsUpgradeSpy->isEmpty());
}