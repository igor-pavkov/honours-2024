QByteArray Cipher::decrypt(QByteArray cipherText)
{
    QByteArray pfx = "";
    bool error = false; // used to flag non cbc, seems like good practice not to parse w/o regard for set encryption type

    //if we get cbc
    if (cipherText.mid(0, 5) == "+OK *")
    {
        //if we have cbc
        if (m_cbc)
            cipherText = cipherText.mid(5);
        //if we don't
        else
        {
            cipherText = cipherText.mid(5);
            pfx = "ERROR_NONECB: ";
            error = true;
        }
    }
    //if we get ecb
    else if (cipherText.mid(0, 4) == "+OK " || cipherText.mid(0, 5) == "mcps ")
    {
        //if we had cbc
        if (m_cbc)
        {
            cipherText = (cipherText.mid(0, 4) == "+OK ") ? cipherText.mid(4) : cipherText.mid(5);
            pfx = "ERROR_NONCBC: ";
            error = true;
        }
        //if we don't
        else
        {
            if (cipherText.mid(0, 4) == "+OK ")
                cipherText = cipherText.mid(4);
            else
                cipherText = cipherText.mid(5);
        }
    }
    //all other cases we fail
    else
        return cipherText;

    QByteArray temp;
    // (if cbc and no error we parse cbc) || (if ecb and error we parse cbc)
    if ((m_cbc && !error) || (!m_cbc && error))
    {
        cipherText = cipherText;
        temp = blowfishCBC(cipherText, false);

        if (temp == cipherText)
        {
            // kDebug("Decryption from CBC Failed");
            return cipherText+' '+'\n';
        }
        else
            cipherText = temp;
    }
    else
    {
        temp = blowfishECB(cipherText, false);

        if (temp == cipherText)
        {
            // kDebug("Decryption from ECB Failed");
            return cipherText+' '+'\n';
        }
        else
            cipherText = temp;
    }
    // TODO FIXME the proper fix for this is to show encryption differently e.g. [nick] instead of <nick>
    // don't hate me for the mircryption reference there.
    if (cipherText.at(0) == 1)
        pfx = "\x0";
    cipherText = pfx+cipherText+' '+'\n'; // FIXME(??) why is there an added space here?
    return cipherText;
}