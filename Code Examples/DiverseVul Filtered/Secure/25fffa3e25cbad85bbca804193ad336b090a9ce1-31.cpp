void OpenConnectionTask::sslConnectionPolicyDecided(bool ok)
{
    switch (model->accessParser(parser).connState) {
    case CONN_STATE_SSL_VERIFYING:
        if (ok) {
            model->changeConnectionState(parser, CONN_STATE_CONNECTED_PRETLS_PRECAPS);
        } else {
            logout(tr("The security state of the SSL connection got rejected"));
        }
        break;
    case CONN_STATE_STARTTLS_VERIFYING:
        if (ok) {
            model->changeConnectionState(parser, CONN_STATE_ESTABLISHED_PRECAPS);
            model->accessParser(parser).capabilitiesFresh = false;
            capabilityCmd = parser->capability();
        } else {
            logout(tr("The security state of the connection after a STARTTLS operation got rejected"));
        }
        break;
    default:
        Q_ASSERT(false);
    }
    parser->unfreezeAfterEncryption();
}