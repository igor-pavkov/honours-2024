void QPaintEngineEx::clip(const QPainterPath &path, Qt::ClipOperation op)
{
    if (path.isEmpty()) {
        QVectorPath vp(nullptr, 0);
        clip(vp, op);
    } else {
        clip(qtVectorPathForPath(path), op);
    }
}