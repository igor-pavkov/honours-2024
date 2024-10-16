DCTStream::DCTStream(Stream *strA, int colorXformA, Dict *dict, int recursion) : FilterStream(strA)
{
    colorXform = colorXformA;
    if (dict != nullptr) {
        Object obj = dict->lookup("Width", recursion);
        err.width = (obj.isInt() && obj.getInt() <= JPEG_MAX_DIMENSION) ? obj.getInt() : 0;
        obj = dict->lookup("Height", recursion);
        err.height = (obj.isInt() && obj.getInt() <= JPEG_MAX_DIMENSION) ? obj.getInt() : 0;
    } else
        err.height = err.width = 0;
    init();
}