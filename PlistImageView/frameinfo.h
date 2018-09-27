#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include <QRect>
#include <QSize>
#include <QPoint>
#include <QString>

class FrameInfo
{
public:
    FrameInfo();

    QString frameName;
    QRect frame;
    QPoint offset;
    bool   rotated;
    QRect sourceColorRect;
    QSize sourceSize;

    void parseFrame(const QString& str);
    void parseOffset(const QString& str);
    void parseSourceColorRect(const QString& str);
    void parseSourceSize(const QString& str);

    QRect rframe() const;

    void dump();
};

#endif // FRAMEINFO_H
