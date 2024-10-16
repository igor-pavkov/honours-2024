QDebug Q_GUI_EXPORT &operator<<(QDebug &s, const QVectorPath &path)
{
    QDebugStateSaver saver(s);
    QRectF rf = path.controlPointRect();
    s << "QVectorPath(size:" << path.elementCount()
      << " hints:" << Qt::hex << path.hints()
      << rf << ')';
    return s;
}