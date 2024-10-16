void QPaintEngineEx::drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode)
{
    Q_ASSUME(pointCount >= 2);
    QVectorPath path((const qreal *) points, pointCount, nullptr, QVectorPath::polygonFlags(mode));

    if (mode == PolylineMode)
        stroke(path, state()->pen);
    else
        draw(path);
}