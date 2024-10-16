void QPaintEngineEx::drawImage(const QPointF &pos, const QImage &image)
{
    drawImage(QRectF(pos, image.size() / image.devicePixelRatio()), image, image.rect());
}