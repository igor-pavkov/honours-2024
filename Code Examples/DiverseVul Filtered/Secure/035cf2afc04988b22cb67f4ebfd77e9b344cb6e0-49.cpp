static int calculateColumnPosition(UChar32* buf32, int len) {
    int width = mk_wcswidth(reinterpret_cast<const int*>(buf32), len);
    if (width == -1)
        return len;
    else
        return width;
}