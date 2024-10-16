void ImapModelOpenConnectionTest::testOpenConnectionShallBlock()
{
    model->rowCount(QModelIndex());
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(SOCK->writtenStuff().isEmpty());
    SOCK->fakeReading("* OK [capability imap4rev1] hi there\r\n");
    QVERIFY(completedSpy->isEmpty());
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE(SOCK->writtenStuff(), QByteArray("y0 LOGIN luzr sikrit\r\n"));
    QCOMPARE(authSpy->size(), 1);
    SOCK->fakeReading("y0 OK [CAPABILITY IMAP4rev1 compress=deflate id] logged in\r\n");
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
#if TROJITA_COMPRESS_DEFLATE
    QCOMPARE(SOCK->writtenStuff(), QByteArray("y1 COMPRESS DEFLATE\r\n"));
    SOCK->fakeReading("y1 NO I just don't want to\r\n");
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE(SOCK->writtenStuff(), QByteArray("y2 ID NIL\r\ny3 LIST \"\" \"%\"\r\n"));
    SOCK->fakeReading("* ID nil\r\ny3 OK listed, nothing like that in there\r\ny2 OK you courious peer\r\n");
#else
    QCOMPARE(SOCK->writtenStuff(), QByteArray("y1 ID NIL\r\ny2 LIST \"\" \"%\"\r\n"));
    SOCK->fakeReading("* ID nil\r\ny2 OK listed, nothing like that in there\r\ny1 OK you courious peer\r\n");
#endif
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE(completedSpy->size(), 1);
    QVERIFY(failedSpy->isEmpty());
    QCOMPARE(authSpy->size(), 1);
    QVERIFY(SOCK->writtenStuff().isEmpty());
    QVERIFY(startTlsUpgradeSpy->isEmpty());
}