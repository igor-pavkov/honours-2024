void ImapModelOpenConnectionTest::cleanup()
{
    delete model;
    model = 0;
    task = 0;
    delete completedSpy;
    completedSpy = 0;
    delete failedSpy;
    failedSpy = 0;
    delete authSpy;
    authSpy = 0;
    delete startTlsUpgradeSpy;
    startTlsUpgradeSpy = 0;
    QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
}