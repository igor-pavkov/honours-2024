void QPaintEngineEx::drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode)
{
    Q_ASSUME(pointCount >= 2);
    int count = pointCount<<1;
    QVarLengthArray<qreal> pts(count);

    for (int i=0; i<count; ++i)
        pts[i] = ((const int *) points)[i];

    QVectorPath path(pts.data(), pointCount, nullptr, QVectorPath::polygonFlags(mode));

    if (mode == PolylineMode)
        stroke(path, state()->pen);
    else
        draw(path);

}