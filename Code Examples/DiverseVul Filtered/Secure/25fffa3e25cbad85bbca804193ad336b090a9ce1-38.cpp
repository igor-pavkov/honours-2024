void OpenConnectionTask::authCredentialsNowAvailable()
{
    if (model->accessParser(parser).connState == CONN_STATE_LOGIN && loginCmd.isEmpty()) {
        if (model->m_hasImapPassword) {
            loginCmd = parser->login(model->m_imapUser, model->m_imapPassword);
            model->accessParser(parser).capabilitiesFresh = false;
        } else {
            logout(tr("No credentials available"));
        }
    }
}