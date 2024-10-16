QByteArray Cipher::initKeyExchange()
{
    QCA::Initializer init;
    m_tempKey = QCA::KeyGenerator().createDH(QCA::DLGroup(m_primeNum, QCA::BigInteger(2))).toDH();

    if (m_tempKey.isNull())
        return QByteArray();

    QByteArray publicKey = m_tempKey.toPublicKey().toDH().y().toArray().toByteArray();

    //remove leading 0
    if (publicKey.length() > 135 && publicKey.at(0) == '\0')
        publicKey = publicKey.mid(1);

    return publicKey.toBase64().append('A');
}