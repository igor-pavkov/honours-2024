void QPaintEngineEx::drawEllipse(const QRectF &r)
{
    qreal pts[26]; // QPointF[13] without constructors...
    union {
        qreal *ptr;
        QPointF *points;
    } x;
    x.ptr = pts;

    int point_count = 0;
    x.points[0] = qt_curves_for_arc(r, 0, -360, x.points + 1, &point_count);
    if (point_count == 0)
        return;
    QVectorPath vp((qreal *) pts, point_count + 1, qpaintengineex_ellipse_types, QVectorPath::EllipseHint);
    draw(vp);
}