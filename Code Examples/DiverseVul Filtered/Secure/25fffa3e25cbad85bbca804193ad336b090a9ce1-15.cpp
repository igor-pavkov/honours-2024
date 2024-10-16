QString OpenConnectionTask::debugIdentification() const
{
    if (parser)
        return QString::fromUtf8("OpenConnectionTask: %1").arg(Imap::connectionStateToString(model->accessParser(parser).connState));
    else
        return QLatin1String("OpenConnectionTask: no parser");
}