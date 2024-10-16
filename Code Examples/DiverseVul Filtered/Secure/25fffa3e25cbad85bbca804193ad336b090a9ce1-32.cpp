QVariant OpenConnectionTask::taskData(const int role) const
{
    return role == RoleTaskCompactName ? QVariant(tr("Connecting to mail server")) : QVariant();
}