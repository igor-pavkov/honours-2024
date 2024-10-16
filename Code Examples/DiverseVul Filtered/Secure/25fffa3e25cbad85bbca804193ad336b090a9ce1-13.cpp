void ImapModelOpenConnectionTest::testOkLogindisabled()
{
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(SOCK->writtenStuff().isEmpty());
    SOCK->fakeReading( "* OK [CAPABILITY IMAP4rev1 starttls LoginDisabled] foo\r\n" );
    QVERIFY( completedSpy->isEmpty() );
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY( authSpy->isEmpty() );
    QCOMPARE( SOCK->writtenStuff(), QByteArray("y0 STARTTLS\r\n") );
    SOCK->fakeReading( "y0 OK will establish secure layer immediately\r\n");
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY( authSpy->isEmpty() );
    QCOMPARE( SOCK->writtenStuff(), QByteArray("[*** STARTTLS ***]y1 CAPABILITY\r\n") );
    QVERIFY( completedSpy->isEmpty() );
    QVERIFY( authSpy->isEmpty() );
    SOCK->fakeReading( "* CAPABILITY IMAP4rev1\r\ny1 OK capability completed\r\n" );
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE( SOCK->writtenStuff(), QByteArray("y2 LOGIN luzr sikrit\r\n") );
    QCOMPARE( authSpy->size(), 1 );
    SOCK->fakeReading( "y2 OK [CAPABILITY IMAP4rev1] logged in\r\n");
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE( completedSpy->size(), 1 );
    QVERIFY(failedSpy->isEmpty());
    QCOMPARE( authSpy->size(), 1 );
    QCOMPARE(startTlsUpgradeSpy->size(), 1);
}