void ImapModelOpenConnectionTest::testPreauthWithCapability()
{
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(SOCK->writtenStuff().isEmpty());
    SOCK->fakeReading( "* PREAUTH [CAPABILITY IMAP4rev1] foo\r\n" );
    QVERIFY( completedSpy->isEmpty() );
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE( SOCK->writtenStuff(), QByteArray() );
    QCOMPARE( completedSpy->size(), 1 );
    QVERIFY(failedSpy->isEmpty());
    QVERIFY( authSpy->isEmpty() );
    QVERIFY(startTlsUpgradeSpy->isEmpty());
}