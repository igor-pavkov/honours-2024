static void qpaintengineex_cubicTo(qreal c1x, qreal c1y, qreal c2x, qreal c2y, qreal ex, qreal ey, void *data) {
    ((StrokeHandler *) data)->pts.add(c1x);
    ((StrokeHandler *) data)->pts.add(c1y);
    ((StrokeHandler *) data)->types.add(QPainterPath::CurveToElement);

    ((StrokeHandler *) data)->pts.add(c2x);
    ((StrokeHandler *) data)->pts.add(c2y);
    ((StrokeHandler *) data)->types.add(QPainterPath::CurveToDataElement);

    ((StrokeHandler *) data)->pts.add(ex);
    ((StrokeHandler *) data)->pts.add(ey);
    ((StrokeHandler *) data)->types.add(QPainterPath::CurveToDataElement);
}