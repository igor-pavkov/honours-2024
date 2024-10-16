static void qpaintengineex_moveTo(qreal x, qreal y, void *data) {
    ((StrokeHandler *) data)->pts.add(x);
    ((StrokeHandler *) data)->pts.add(y);
    ((StrokeHandler *) data)->types.add(QPainterPath::MoveToElement);
}