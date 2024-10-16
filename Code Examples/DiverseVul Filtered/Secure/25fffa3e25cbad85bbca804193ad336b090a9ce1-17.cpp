void ImapModelOpenConnectionTest::init( bool startTlsRequired )
{
    Imap::Mailbox::AbstractCache* cache = new Imap::Mailbox::MemoryCache(this);
    factory = new Streams::FakeSocketFactory(Imap::CONN_STATE_CONNECTED_PRETLS_PRECAPS);
    factory->setStartTlsRequired( startTlsRequired );
    Imap::Mailbox::TaskFactoryPtr taskFactory( new Imap::Mailbox::TaskFactory() ); // yes, the real one
    model = new Imap::Mailbox::Model(this, cache, Imap::Mailbox::SocketFactoryPtr( factory ), std::move(taskFactory));
    connect(model, SIGNAL(authRequested()), this, SLOT(provideAuthDetails()), Qt::QueuedConnection);
    connect(model, SIGNAL(needsSslDecision(QList<QSslCertificate>,QList<QSslError>)),
            this, SLOT(acceptSsl(QList<QSslCertificate>,QList<QSslError>)), Qt::QueuedConnection);
    LibMailboxSync::setModelNetworkPolicy(model, Imap::Mailbox::NETWORK_ONLINE);
    QCoreApplication::processEvents();
    task = new Imap::Mailbox::OpenConnectionTask(model);
    completedSpy = new QSignalSpy(task, SIGNAL(completed(Imap::Mailbox::ImapTask*)));
    failedSpy = new QSignalSpy(task, SIGNAL(failed(QString)));
    authSpy = new QSignalSpy(model, SIGNAL(authRequested()));
    connErrorSpy = new QSignalSpy(model, SIGNAL(connectionError(QString)));
    startTlsUpgradeSpy = new QSignalSpy(model, SIGNAL(requireStartTlsInFuture()));
}