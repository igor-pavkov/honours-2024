void OpenConnectionTask::logout(const QString &message)
{
    _failed(message);
    model->setNetworkPolicy(NETWORK_OFFLINE);
}