bool Cipher::setType(const QString &type)
{
    //TODO check QCA::isSupported()
    m_type = type;
    return true;
}