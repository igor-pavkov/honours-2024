QByteArray Cipher::parseInitKeyX(QByteArray key)
{
    QCA::Initializer init;
    bool isCBC = false;

    if (key.endsWith(" CBC"))
    {
        isCBC = true;
        key.chop(4);
    }

    if (key.length() != 181)
        return QByteArray();

    QCA::SecureArray remoteKey = QByteArray::fromBase64(key.left(180));
    QCA::DLGroup group(m_primeNum, QCA::BigInteger(2));
    QCA::DHPrivateKey privateKey = QCA::KeyGenerator().createDH(group).toDH();

    if (privateKey.isNull())
        return QByteArray();

    QByteArray publicKey = privateKey.y().toArray().toByteArray();

    //remove leading 0
    if (publicKey.length() > 135 && publicKey.at(0) == '\0')
        publicKey = publicKey.mid(1);

    QCA::DHPublicKey remotePub(group, remoteKey);

    if (remotePub.isNull())
        return QByteArray();

    QByteArray sharedKey = privateKey.deriveKey(remotePub).toByteArray();
    sharedKey = QCA::Hash("sha256").hash(sharedKey).toByteArray().toBase64();

    //remove trailing = because mircryption and fish think it's a swell idea.
    while (sharedKey.endsWith('=')) sharedKey.chop(1);

    if (isCBC)
        sharedKey.prepend("cbc:");

    bool success = setKey(sharedKey);

    if (!success)
        return QByteArray();

    return publicKey.toBase64().append('A');
}