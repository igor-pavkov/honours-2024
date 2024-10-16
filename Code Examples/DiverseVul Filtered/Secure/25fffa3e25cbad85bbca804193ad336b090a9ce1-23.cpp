void ImapModelOpenConnectionTest::provideAuthDetails()
{
    if (m_enableAutoLogin) {
        model->setImapUser(QLatin1String("luzr"));
        model->setImapPassword(QLatin1String("sikrit"));
    }
}