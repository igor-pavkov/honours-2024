QByteArray Cipher::byteToB64(QByteArray text)
{
    int left = 0;
    int right = 0;
    int k = -1;
    int v;
    QString base64 = "./0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QByteArray encoded;
    while (k < (text.length() - 1)) {
        k++;
        v = text.at(k); if (v < 0) v += 256;
        left = v << 24;
        k++;
        v = text.at(k); if (v < 0) v += 256;
        left += v << 16;
        k++;
        v = text.at(k); if (v < 0) v += 256;
        left += v << 8;
        k++;
        v = text.at(k); if (v < 0) v += 256;
        left += v;

        k++;
        v = text.at(k); if (v < 0) v += 256;
        right = v << 24;
        k++;
        v = text.at(k); if (v < 0) v += 256;
        right += v << 16;
        k++;
        v = text.at(k); if (v < 0) v += 256;
        right += v << 8;
        k++;
        v = text.at(k); if (v < 0) v += 256;
        right += v;

        for (int i = 0; i < 6; i++) {
            encoded.append(base64.at(right & 0x3F).toAscii());
            right = right >> 6;
        }

        //TODO make sure the .toascii doesn't break anything
        for (int i = 0; i < 6; i++) {
            encoded.append(base64.at(left & 0x3F).toAscii());
            left = left >> 6;
        }
    }
    return encoded;
}