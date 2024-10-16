static void qpaintengineex_lineTo(qreal x, qreal y, void *data) {
    ((StrokeHandler *) data)->pts.add(x);
    ((StrokeHandler *) data)->pts.add(y);
    ((StrokeHandler *) data)->types.add(QPainterPath::LineToElement);
}