QByteArray Cipher::decryptTopic(QByteArray cipherText)
{
    if (cipherText.mid(0, 4) == "+OK ") // FiSH style topic
        cipherText = cipherText.mid(4);
    else if (cipherText.left(5) == "«m«")
        cipherText = cipherText.mid(5, cipherText.length()-10);
    else
        return cipherText;

    QByteArray temp;
    //TODO currently no backwards sanity checks for topic, it seems to use different standards
    //if somebody figures them out they can enable it and add "ERROR_NONECB/CBC" warnings
    if (m_cbc)
        temp = blowfishCBC(cipherText.mid(1), false);
    else
        temp = blowfishECB(cipherText, false);

    if (temp == cipherText)
    {
        return cipherText;
    }
    else
        cipherText = temp;

    if (cipherText.mid(0, 2) == "@@")
        cipherText = cipherText.mid(2);

    return cipherText;
}