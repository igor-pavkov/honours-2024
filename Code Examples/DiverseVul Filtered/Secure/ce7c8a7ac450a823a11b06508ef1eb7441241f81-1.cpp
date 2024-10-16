size_t _q_urldecode(char *str)
{
    if (str == NULL) {
        return 0;
    }

    char *pEncPt, *pBinPt = str;
    for (pEncPt = str; *pEncPt != '\0'; pEncPt++) {
        switch (*pEncPt) {
            case '+': {
                *pBinPt++ = ' ';
                break;
            }
            case '%': {
                if (*(pEncPt + 1) != '\0' && isxdigit(*(pEncPt + 1)) \
                    && *(pEncPt + 2) != '\0' && isxdigit(*(pEncPt + 2))) {
                    *pBinPt++ = _q_x2c(*(pEncPt + 1), *(pEncPt + 2));
                    pEncPt += 2;
                } else {
                    *pBinPt++ = *pEncPt;
                }
                break;
            }
            default: {
                *pBinPt++ = *pEncPt;
                break;
            }
        }
    }
    *pBinPt = '\0';

    return (pBinPt - str);
}