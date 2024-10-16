void ImapModelOpenConnectionTest::testLoginDelaysOtherTasks()
{
    using namespace Imap::Mailbox;
    MemoryCache *cache = dynamic_cast<MemoryCache *>(model->cache());
    Q_ASSERT(cache);
    cache->setChildMailboxes(QString(),
                             QList<MailboxMetadata>() << MailboxMetadata(QString::fromUtf8("a"), QString(), QStringList())
                             << MailboxMetadata(QString::fromUtf8("b"), QString(), QStringList())
                             );
    m_enableAutoLogin = false;
    // The cache is not queried synchronously
    QCOMPARE(model->rowCount(QModelIndex()), 1);
    QCoreApplication::processEvents();
    // The cache should have been consulted by now
    QCOMPARE(model->rowCount(QModelIndex()), 3);
    QCoreApplication::processEvents();
    QVERIFY(SOCK->writtenStuff().isEmpty());
    SOCK->fakeReading("* OK [capability imap4rev1] hi there\r\n");
    QVERIFY(completedSpy->isEmpty());
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    // The login must not be sent (as per m_enableAutoLogin being false)
    QVERIFY(SOCK->writtenStuff().isEmpty());
    QModelIndex mailboxA = model->index(1, 0, QModelIndex());
    QVERIFY(mailboxA.isValid());
    QCOMPARE(mailboxA.data(RoleMailboxName).toString(), QString::fromUtf8("a"));
    QModelIndex mailboxB = model->index(2, 0, QModelIndex());
    QVERIFY(mailboxB.isValid());
    QCOMPARE(mailboxB.data(RoleMailboxName).toString(), QString::fromUtf8("b"));
    QModelIndex msgListA = mailboxA.child(0, 0);
    Q_ASSERT(msgListA.isValid());
    QModelIndex msgListB = mailboxB.child(0, 0);
    Q_ASSERT(msgListB.isValid());

    // Request syncing the mailboxes
    QCOMPARE(model->rowCount(msgListA), 0);
    QCOMPARE(model->rowCount(msgListB), 0);
    for (int i = 0; i < 10; ++i)
        QCoreApplication::processEvents();
    QVERIFY(SOCK->writtenStuff().isEmpty());

    // Unblock the login
    m_enableAutoLogin = true;
    provideAuthDetails();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE(SOCK->writtenStuff(), QByteArray("y0 LOGIN luzr sikrit\r\n"));
    SOCK->fakeReading("y0 OK [CAPABILITY IMAP4rev1] logged in\r\n");
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    // FIXME: selecting the "b" shall definitely wait until "a" is completed
    QCOMPARE(SOCK->writtenStuff(), QByteArray("y1 LIST \"\" \"%\"\r\ny2 SELECT a\r\ny3 SELECT b\r\n"));
    SOCK->fakeReading("y1 OK listed, nothing like that in there\r\n");
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCOMPARE(completedSpy->size(), 1);
    QVERIFY(failedSpy->isEmpty());
    QCOMPARE(authSpy->size(), 1);
    QVERIFY(SOCK->writtenStuff().isEmpty());
    QVERIFY(startTlsUpgradeSpy->isEmpty());
}