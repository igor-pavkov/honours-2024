void QPaintEngineEx::drawRects(const QRect *rects, int rectCount)
{
    for (int i=0; i<rectCount; ++i) {
        const QRect &r = rects[i];
        // ### Is there a one off here?
        qreal right = r.x() + r.width();
        qreal bottom = r.y() + r.height();
        qreal pts[] = { qreal(r.x()), qreal(r.y()),
                        right, qreal(r.y()),
                        right, bottom,
                        qreal(r.x()), bottom,
                        qreal(r.x()), qreal(r.y()) };
        QVectorPath vp(pts, 5, nullptr, QVectorPath::RectangleHint);
        draw(vp);
    }
}