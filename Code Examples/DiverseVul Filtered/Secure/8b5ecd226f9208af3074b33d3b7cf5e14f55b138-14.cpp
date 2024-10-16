QByteArray Cipher::blowfishCBC(QByteArray cipherText, bool direction)
{
    QCA::Initializer init;
    QByteArray temp = cipherText;
    if (direction)
    {
        // make sure cipherText is an interval of 8 bits. We do this before so that we
        // know there's at least 8 bytes to en/decryption this ensures QCA doesn't fail
        while ((temp.length() % 8) != 0) temp.append('\0');

        QCA::InitializationVector iv(8);
        temp.prepend(iv.toByteArray()); // prefix with 8bits of IV for mircryptions *CUSTOM* cbc implementation
    }
    else
    {
        temp = QByteArray::fromBase64(temp);
        //supposedly nescessary if we get a truncated message also allows for decryption of 'crazy'
        //en/decoding clients that use STANDARDIZED PADDING TECHNIQUES
        while ((temp.length() % 8) != 0) temp.append('\0');
    }

    QCA::Direction dir = (direction) ? QCA::Encode : QCA::Decode;
    QCA::Cipher cipher(m_type, QCA::Cipher::CBC, QCA::Cipher::NoPadding, dir, m_key, QCA::InitializationVector(QByteArray("0")));
    QByteArray temp2 = cipher.update(QCA::MemoryRegion(temp)).toByteArray();
    temp2 += cipher.final().toByteArray();

    if (!cipher.ok())
        return cipherText;

    if (direction) //send in base64
        temp2 = temp2.toBase64();
    else //cut off the 8bits of IV
        temp2 = temp2.remove(0, 8);

    return temp2;
}