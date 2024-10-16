void ImapModelOpenConnectionTest::testInitialGarbage()
{
    QFETCH(QByteArray, greetings);

    model->rowCount(QModelIndex());
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QVERIFY(SOCK->writtenStuff().isEmpty());
    SOCK->fakeReading(greetings);
    for (int i = 0; i < 10; ++i)
        QCoreApplication::processEvents();
    //qDebug() << QList<QVariantList>(*connErrorSpy);
    QCOMPARE(connErrorSpy->size(), 1);
    QCOMPARE(failedSpy->size(), 1);
    QVERIFY(completedSpy->isEmpty());
    QVERIFY(startTlsUpgradeSpy->isEmpty());
}