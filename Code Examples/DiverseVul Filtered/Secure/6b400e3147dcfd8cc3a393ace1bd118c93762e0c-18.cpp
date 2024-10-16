void QPaintEngineEx::drawPoints(const QPointF *points, int pointCount)
{
    QPen pen = state()->pen;
    if (pen.capStyle() == Qt::FlatCap)
        pen.setCapStyle(Qt::SquareCap);

    if (pen.brush().isOpaque()) {
        while (pointCount > 0) {
            int count = qMin(pointCount, 16);
            qreal pts[64];
            int oset = -1;
            for (int i=0; i<count; ++i) {
                pts[++oset] = points[i].x();
                pts[++oset] = points[i].y();
                pts[++oset] = points[i].x() + 1/63.;
                pts[++oset] = points[i].y();
            }
            QVectorPath path(pts, count * 2, qpaintengineex_line_types_16, QVectorPath::LinesHint);
            stroke(path, pen);
            pointCount -= 16;
            points += 16;
        }
    } else {
        for (int i=0; i<pointCount; ++i) {
            qreal pts[] = { points[i].x(), points[i].y(), points[i].x() + qreal(1/63.), points[i].y() };
            QVectorPath path(pts, 2, nullptr);
            stroke(path, pen);
        }
    }
}