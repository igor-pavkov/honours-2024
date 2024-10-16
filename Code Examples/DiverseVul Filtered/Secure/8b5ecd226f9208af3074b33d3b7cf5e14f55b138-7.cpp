bool Cipher::setKey(QByteArray key)
{
    if (key.isEmpty()) {
        m_key.clear();
        return false;
    }

    if (key.mid(0, 4).toLower() == "ecb:")
    {
        m_cbc = false;
        m_key = key.mid(4);
    }
    //strip cbc: if included
    else if (key.mid(0, 4).toLower() == "cbc:")
    {
        m_cbc = true;
        m_key = key.mid(4);
    }
    else
    {
//    if(Preferences::self()->encryptionType())
//      m_cbc = true;
//    else
        m_cbc = false;
        m_key = key;
    }
    return true;
}