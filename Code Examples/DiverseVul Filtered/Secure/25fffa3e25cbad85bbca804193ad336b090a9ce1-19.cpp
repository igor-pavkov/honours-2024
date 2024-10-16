void ImapModelOpenConnectionTest::testInitialBye()
{
    model->rowCount(QModelIndex());
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(SOCK->writtenStuff().isEmpty());
    SOCK->fakeReading("* BYE Sorry, we're offline\r\n");
    for (int i = 0; i < 10; ++i)
        QCoreApplication::processEvents();
    QCOMPARE(failedSpy->size(), 1);
    QVERIFY(completedSpy->isEmpty());
    QVERIFY(connErrorSpy->isEmpty());
    QVERIFY(startTlsUpgradeSpy->isEmpty());
}