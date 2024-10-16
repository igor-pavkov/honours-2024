void QPaintEngineEx::drawRoundedRect(const QRectF &rect, qreal xRadius, qreal yRadius,
                                     Qt::SizeMode mode)
{
    qreal x1 = rect.left();
    qreal x2 = rect.right();
    qreal y1 = rect.top();
    qreal y2 = rect.bottom();

    if (mode == Qt::RelativeSize) {
        xRadius = xRadius * rect.width() / 200.;
        yRadius = yRadius * rect.height() / 200.;
    }

    xRadius = qMin(xRadius, rect.width() / 2);
    yRadius = qMin(yRadius, rect.height() / 2);

    qreal pts[] = {
        x1 + xRadius, y1,                   // MoveTo
        x2 - xRadius, y1,                   // LineTo
        x2 - (1 - KAPPA) * xRadius, y1,     // CurveTo
        x2, y1 + (1 - KAPPA) * yRadius,
        x2, y1 + yRadius,
        x2, y2 - yRadius,                   // LineTo
        x2, y2 - (1 - KAPPA) * yRadius,     // CurveTo
        x2 - (1 - KAPPA) * xRadius, y2,
        x2 - xRadius, y2,
        x1 + xRadius, y2,                   // LineTo
        x1 + (1 - KAPPA) * xRadius, y2,           // CurveTo
        x1, y2 - (1 - KAPPA) * yRadius,
        x1, y2 - yRadius,
        x1, y1 + yRadius,                   // LineTo
        x1, y1 + (1 - KAPPA) * yRadius,           // CurveTo
        x1 + (1 - KAPPA) * xRadius, y1,
        x1 + xRadius, y1
    };

    QVectorPath path(pts, 17, qpaintengineex_roundedrect_types, QVectorPath::RoundedRectHint);
    draw(path);
}