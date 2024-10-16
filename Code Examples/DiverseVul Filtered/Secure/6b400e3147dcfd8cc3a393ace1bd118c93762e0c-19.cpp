void QPaintEngineEx::drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s)
{
    QBrush brush(state()->pen.color(), pixmap);
    QTransform xform = QTransform::fromTranslate(r.x() - s.x(), r.y() - s.y());
    if (!qFuzzyCompare(pixmap.devicePixelRatio(), qreal(1.0)))
        xform.scale(1.0/pixmap.devicePixelRatio(), 1.0/pixmap.devicePixelRatio());
    brush.setTransform(xform);

    qreal pts[] = { r.x(), r.y(),
                    r.x() + r.width(), r.y(),
                    r.x() + r.width(), r.y() + r.height(),
                    r.x(), r.y() + r.height() };

    QVectorPath path(pts, 4, nullptr, QVectorPath::RectangleHint);
    fill(path, brush);
}