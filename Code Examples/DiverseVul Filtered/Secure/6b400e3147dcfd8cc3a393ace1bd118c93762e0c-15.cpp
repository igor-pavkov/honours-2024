const QVectorPath &qtVectorPathForPath(const QPainterPath &path)
{
    Q_ASSERT(path.d_func());
    return path.d_func()->vectorPath();
}