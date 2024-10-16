void QPaintEngineEx::drawLines(const QLineF *lines, int lineCount)
{
    int elementCount = lineCount << 1;
    while (elementCount > 0) {
        int count = qMin(elementCount, 32);

        QVectorPath path((const qreal *) lines, count, qpaintengineex_line_types_16,
                         QVectorPath::LinesHint);
        stroke(path, state()->pen);

        elementCount -= 32;
        lines += 16;
    }
}