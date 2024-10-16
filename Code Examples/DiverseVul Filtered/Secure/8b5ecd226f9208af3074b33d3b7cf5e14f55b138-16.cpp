QByteArray Cipher::b64ToByte(QByteArray text)
{
    QString base64 = "./0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QByteArray decoded;
    int k = -1;
    while (k < (text.length() - 1)) {
        int right = 0;
        int left = 0;
        int v = 0;
        int w = 0;
        int z = 0;

        for (int i = 0; i < 6; i++) {
            k++;
            v = base64.indexOf(text.at(k));
            right |= v << (i * 6);
        }

        for (int i = 0; i < 6; i++) {
            k++;
            v = base64.indexOf(text.at(k));
            left |= v << (i * 6);
        }

        for (int i = 0; i < 4; i++) {
            w = ((left & (0xFF << ((3 - i) * 8))));
            z = w >> ((3 - i) * 8);
            if (z < 0) { z = z + 256; }
            decoded.append(z);
        }

        for (int i = 0; i < 4; i++) {
            w = ((right & (0xFF << ((3 - i) * 8))));
            z = w >> ((3 - i) * 8);
            if (z < 0) { z = z + 256; }
            decoded.append(z);
        }
    }
    return decoded;
}