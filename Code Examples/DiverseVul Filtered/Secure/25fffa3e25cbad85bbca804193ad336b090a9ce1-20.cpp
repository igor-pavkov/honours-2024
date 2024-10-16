void ImapModelOpenConnectionTest::acceptSsl(const QList<QSslCertificate> &certificateChain, const QList<QSslError> &sslErrors)
{
    model->setSslPolicy(certificateChain, sslErrors, true);
}