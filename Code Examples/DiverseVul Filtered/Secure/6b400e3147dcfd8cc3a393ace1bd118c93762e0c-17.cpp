QRectF QVectorPath::controlPointRect() const
{
    if (m_hints & ControlPointRect)
        return QRectF(QPointF(m_cp_rect.x1, m_cp_rect.y1), QPointF(m_cp_rect.x2, m_cp_rect.y2));

    if (m_count == 0) {
        m_cp_rect.x1 = m_cp_rect.x2 = m_cp_rect.y1 = m_cp_rect.y2 = 0;
        m_hints |= ControlPointRect;
        return QRectF(QPointF(m_cp_rect.x1, m_cp_rect.y1), QPointF(m_cp_rect.x2, m_cp_rect.y2));
    }
    Q_ASSERT(m_points && m_count > 0);

    const qreal *pts = m_points;
    m_cp_rect.x1 = m_cp_rect.x2 = *pts;
    ++pts;
    m_cp_rect.y1 = m_cp_rect.y2 = *pts;
    ++pts;

    const qreal *epts = m_points + (m_count << 1);
    while (pts < epts) {
        qreal x = *pts;
        if (x < m_cp_rect.x1) m_cp_rect.x1 = x;
        else if (x > m_cp_rect.x2) m_cp_rect.x2 = x;
        ++pts;

        qreal y = *pts;
        if (y < m_cp_rect.y1) m_cp_rect.y1 = y;
        else if (y > m_cp_rect.y2) m_cp_rect.y2 = y;
        ++pts;
    }

    m_hints |= ControlPointRect;
    return QRectF(QPointF(m_cp_rect.x1, m_cp_rect.y1), QPointF(m_cp_rect.x2, m_cp_rect.y2));
}