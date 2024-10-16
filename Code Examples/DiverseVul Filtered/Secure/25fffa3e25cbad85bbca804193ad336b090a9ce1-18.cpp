void ImapModelOpenConnectionTest::testCapabilityAfterLogin()
{
    cleanup(); init(true); // yuck, but I can't come up with anything better...

    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(SOCK->writtenStuff().isEmpty());
    SOCK->fakeReading( "* OK foo\r\n" );
    QVERIFY( completedSpy->isEmpty() );
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE(SOCK->writtenStuff(), QByteArray("y0 CAPABILITY\r\n"));
    SOCK->fakeReading("* CAPABILITY imap4rev1 starttls\r\ny0 ok cap\r\n");
    QVERIFY( authSpy->isEmpty() );
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE( SOCK->writtenStuff(), QByteArray("y1 STARTTLS\r\n") );
    SOCK->fakeReading( "y1 OK will establish secure layer immediately\r\n");
    QCoreApplication::processEvents();
    QVERIFY( authSpy->isEmpty() );
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE( SOCK->writtenStuff(), QByteArray("[*** STARTTLS ***]y2 CAPABILITY\r\n") );
    QVERIFY( completedSpy->isEmpty() );
    QVERIFY( authSpy->isEmpty() );
    SOCK->fakeReading( "* CAPABILITY IMAP4rev1\r\ny2 OK capability completed\r\n" );
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE( SOCK->writtenStuff(), QByteArray("y3 LOGIN luzr sikrit\r\n") );
    QCOMPARE( authSpy->size(), 1 );
    SOCK->fakeReading("* CAPABILITY imap4rev1\r\n" "y3 OK logged in\r\n");
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE( completedSpy->size(), 1 );
    QVERIFY(failedSpy->isEmpty());
    QCOMPARE( authSpy->size(), 1 );
    QVERIFY(SOCK->writtenStuff().isEmpty());
    QVERIFY(startTlsUpgradeSpy->isEmpty());
}