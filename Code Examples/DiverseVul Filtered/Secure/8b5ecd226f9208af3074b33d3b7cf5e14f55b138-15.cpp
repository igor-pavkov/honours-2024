bool Cipher::encrypt(QByteArray &cipherText)
{
    if (cipherText.left(3) == "+p ") //don't encode if...?
        cipherText = cipherText.mid(3);
    else
    {
        if (m_cbc) //encode in ecb or cbc decide how to determine later
        {
            QByteArray temp = blowfishCBC(cipherText, true);

            if (temp == cipherText)
            {
                // kDebug("CBC Encoding Failed");
                return false;
            }

            cipherText = "+OK *" + temp;
        }
        else
        {
            QByteArray temp = blowfishECB(cipherText, true);

            if (temp == cipherText)
            {
                // kDebug("ECB Encoding Failed");
                return false;
            }

            cipherText = "+OK " + temp;
        }
    }
    return true;
}