bool Cipher::neededFeaturesAvailable()
{
    QCA::Initializer init;

    if (QCA::isSupported("blowfish-ecb") && QCA::isSupported("blowfish-cbc") && QCA::isSupported("dh"))
        return true;

    return false;
}