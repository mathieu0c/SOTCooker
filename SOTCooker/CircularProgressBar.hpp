#pragma once

#include <QWidget>
#include <QPainter>
#include <QPainterPath>

class CircularProgressBar : public QWidget {
    Q_OBJECT
private:
    qreal m_progress; // progress 0.0 to 1.0
public:
    CircularProgressBar(QWidget * p = 0) : QWidget(p), m_progress(0) {
        setMinimumSize(208, 208);
    }

public slots:
    void SetPercentage(double percentage) {//in [0-100]
        auto internal_percentage = percentage / 100.;
        if (m_progress == internal_percentage) return;
        m_progress = internal_percentage;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) {
        qreal pd = m_progress * 360.;
        qreal rd = 360. - pd;

        constexpr double kStrokeWidth{9};

        auto min_size{std::min(width(),height())-kStrokeWidth};
        const auto kHalfMinSize{min_size/2};
        QPointF center{(double)width()/2.,(double)height()/2.};

        const QPointF kStartingPoint{center.x(), center.y()-kHalfMinSize};
        const QRectF kArcRect{center.x()-kHalfMinSize, center.y()-kHalfMinSize, (double)min_size, (double)min_size};

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), Qt::white);
//        p.translate(4, 4);

        QPainterPath path, path2;

        const QColor kBaseProgressColor{25,180,255};
        const QColor kEndColor{255,69,0};

        //progress path
        path.moveTo(kStartingPoint);
        path.arcTo(kArcRect, 90, -pd);
        QConicalGradient gradient(kArcRect.center(),92.5);
        gradient.setColorAt(1.,kBaseProgressColor);
        gradient.setColorAt(0.,kEndColor);
        p.strokePath(path, QPen(gradient,kStrokeWidth));

        //base path
        QPen pen2{};
        path2.moveTo(kStartingPoint);
        pen2.setWidth(kStrokeWidth);
        pen2.setColor(QColor(215,215,215));
        pen2.setCapStyle(Qt::FlatCap);
        pen2.setDashPattern(QVector<qreal>{0.5, 1.105});
        path2.arcTo(kArcRect, 90, rd);
        pen2.setDashOffset(2.2);
        p.strokePath(path2, pen2);

        const auto kBaseFont{p.font()};
        QFont percent_font{"LEMONMILK",(int)(min_size/6),QFont::ExtraBold};
        p.setFont(percent_font);

        QString kPercentStr{QString::number(m_progress*100,'f',0)+" %"};
        p.setPen(kBaseProgressColor);
        p.drawText(kArcRect, Qt::AlignCenter, kPercentStr);

        p.setFont(kBaseFont);
    }
};
