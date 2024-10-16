int set_encryption_filter(const char* input)
{
    if(input == NULL) return 1;

    if(strlen(input) < 3) return 1;

    if(strcasecmp(input, "opn") == 0)
        G.f_encrypt |= STD_OPN;

    if(strcasecmp(input, "wep") == 0)
        G.f_encrypt |= STD_WEP;

    if(strcasecmp(input, "wpa") == 0)
    {
        G.f_encrypt |= STD_WPA;
        G.f_encrypt |= STD_WPA2;
    }

    if(strcasecmp(input, "wpa1") == 0)
        G.f_encrypt |= STD_WPA;

    if(strcasecmp(input, "wpa2") == 0)
        G.f_encrypt |= STD_WPA2;

    return 0;
}