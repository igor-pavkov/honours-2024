bool OpenConnectionTask::handleSocketEncryptedResponse(const Responses::SocketEncryptedResponse *const resp)
{
    switch (model->accessParser(parser).connState) {
    case CONN_STATE_SSL_HANDSHAKE:
        model->changeConnectionState(parser, CONN_STATE_SSL_VERIFYING);
        m_sslChain = resp->sslChain;
        m_sslErrors = resp->sslErrors;
        model->processSslErrors(this);
        return true;
    case CONN_STATE_STARTTLS_HANDSHAKE:
        model->changeConnectionState(parser, CONN_STATE_STARTTLS_VERIFYING);
        m_sslChain = resp->sslChain;
        m_sslErrors = resp->sslErrors;
        model->processSslErrors(this);
        return true;
    default:
        qDebug() << model->accessParser(parser).connState;
        return false;
    }
}