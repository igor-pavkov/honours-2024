void QPaintEngineEx::drawRects(const QRectF *rects, int rectCount)
{
    for (int i=0; i<rectCount; ++i) {
        const QRectF &r = rects[i];
        qreal right = r.x() + r.width();
        qreal bottom = r.y() + r.height();
        qreal pts[] = { r.x(), r.y(),
                        right, r.y(),
                        right, bottom,
                        r.x(), bottom,
                        r.x(), r.y() };
        QVectorPath vp(pts, 5, nullptr, QVectorPath::RectangleHint);
        draw(vp);
    }
}