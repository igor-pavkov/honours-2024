void OpenConnectionTask::startTlsOrLoginNow()
{
    if (model->m_startTls || model->accessParser(parser).capabilities.contains(QLatin1String("LOGINDISABLED"))) {
        // Should run STARTTLS later and already have the capabilities
        Q_ASSERT(model->accessParser(parser).capabilitiesFresh);
        if (!model->accessParser(parser).capabilities.contains(QLatin1String("STARTTLS"))) {
            logout(tr("Server does not support STARTTLS"));
        } else {
            startTlsCmd = parser->startTls();
            model->changeConnectionState(parser, CONN_STATE_STARTTLS_ISSUED);
        }
    } else {
        // We're requested to authenticate even without STARTTLS
        Q_ASSERT(!model->accessParser(parser).capabilities.contains(QLatin1String("LOGINDISABLED")));
        model->changeConnectionState(parser, CONN_STATE_LOGIN);
        askForAuth();
    }
}