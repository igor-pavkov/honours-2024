void QPaintEngineEx::fillRect(const QRectF &r, const QBrush &brush)
{
    qreal pts[] = { r.x(), r.y(), r.x() + r.width(), r.y(),
                    r.x() + r.width(), r.y() + r.height(), r.x(), r.y() + r.height() };
    QVectorPath vp(pts, 4, nullptr, QVectorPath::RectangleHint);
    fill(vp, brush);
}