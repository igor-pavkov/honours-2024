static int ipv4_from_asc(unsigned char *v4, const char *in)
{
    int a0, a1, a2, a3;
    if (sscanf(in, "%d.%d.%d.%d", &a0, &a1, &a2, &a3) != 4)
        return 0;
    if ((a0 < 0) || (a0 > 255) || (a1 < 0) || (a1 > 255)
        || (a2 < 0) || (a2 > 255) || (a3 < 0) || (a3 > 255))
        return 0;
    v4[0] = a0;
    v4[1] = a1;
    v4[2] = a2;
    v4[3] = a3;
    return 1;
}