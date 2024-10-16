static int strip_return_port (char *host)
{
        char *ptr1;
        char *ptr2;
        int port;

        ptr1 = strrchr (host, ':');
        if (ptr1 == NULL)
                return 0;

        /* Check for IPv6 style literals */
        ptr2 = strchr (ptr1, ']');
        if (ptr2 != NULL)
                return 0;

        *ptr1++ = '\0';
        if (sscanf (ptr1, "%d", &port) != 1)    /* one conversion required */
                return 0;
        return port;
}