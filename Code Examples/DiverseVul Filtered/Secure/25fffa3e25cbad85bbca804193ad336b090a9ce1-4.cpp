void OpenConnectionTask::onComplete()
{
    // Optionally issue the ID command
    if (model->accessParser(parser).capabilities.contains(QLatin1String("ID"))) {
        Imap::Mailbox::ImapTask *task = model->m_taskFactory->createIdTask(model, this);
        task->perform();
    }
    // Optionally enable QRESYNC
    if (model->accessParser(parser).capabilities.contains(QLatin1String("QRESYNC")) &&
            model->accessParser(parser).capabilities.contains(QLatin1String("ENABLE"))) {
        Imap::Mailbox::ImapTask *task = model->m_taskFactory->createEnableTask(model, this,
                                                                               QList<QByteArray>() << QByteArray("QRESYNC"));
        task->perform();
    }

    // But do terminate this task
    _completed();
}