void QPaintEngineEx::drawPixmap(const QPointF &pos, const QPixmap &pm)
{
    drawPixmap(QRectF(pos, pm.size() / pm.devicePixelRatio()), pm, pm.rect());
}