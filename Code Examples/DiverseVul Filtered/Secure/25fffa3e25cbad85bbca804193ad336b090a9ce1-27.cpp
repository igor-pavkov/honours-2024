void OpenConnectionTask::askForAuth()
{
    if (model->m_hasImapPassword) {
        Q_ASSERT(loginCmd.isEmpty());
        loginCmd = parser->login(model->m_imapUser, model->m_imapPassword);
        model->accessParser(parser).capabilitiesFresh = false;
    } else {
        EMIT_LATER_NOARG(model, authRequested);
    }
}