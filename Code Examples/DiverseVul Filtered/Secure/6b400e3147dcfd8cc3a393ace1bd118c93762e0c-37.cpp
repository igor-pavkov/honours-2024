void QPaintEngineEx::drawPath(const QPainterPath &path)
{
    if (!path.isEmpty())
        draw(qtVectorPathForPath(path));
}