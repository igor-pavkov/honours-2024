void ImapModelOpenConnectionTest::testOkLogindisabledWithoutStarttls()
{
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(SOCK->writtenStuff().isEmpty());
    SOCK->fakeReading( "* OK [CAPABILITY IMAP4rev1 LoginDisabled] foo\r\n" );
    QVERIFY( completedSpy->isEmpty() );
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY( authSpy->isEmpty() );
    // The capabilities do not contain STARTTLS but LOGINDISABLED is in there
    QCOMPARE(failedSpy->size(), 1);
    QVERIFY(startTlsUpgradeSpy->isEmpty());
}