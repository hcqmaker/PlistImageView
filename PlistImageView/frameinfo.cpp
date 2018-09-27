#include "frameinfo.h"
#include <QDebug>

FrameInfo::FrameInfo()
{

}
void FrameInfo::parseFrame(const QString& str)
{
    int x,y,w,h;
    sscanf(str.toLocal8Bit().data(), "{{%d,%d},{%d,%d}}", &x, &y, &w, &h);
    this->frame.setX(x);
    this->frame.setY(y);
    this->frame.setWidth(w);
    this->frame.setHeight(h);
}

void FrameInfo::parseOffset(const QString& str)
{
    int x,y;
    sscanf(str.toLocal8Bit().data(), "{%d,%d}", &x, &y);
    this->offset.setX(x);
    this->offset.setY(y);
}

void FrameInfo::parseSourceColorRect(const QString& str)
{
    int x,y,w,h;
    sscanf(str.toLocal8Bit().data(), "{{%d,%d},{%d,%d}}", &x, &y, &w, &h);
    this->sourceColorRect.setX(x);
    this->sourceColorRect.setY(y);
    this->sourceColorRect.setWidth(w);
    this->sourceColorRect.setHeight(h);
}

void FrameInfo::parseSourceSize(const QString& str)
{
    int w,h;
    sscanf(str.toLocal8Bit().data(), "{%d,%d}", &w, &h);
    this->sourceSize.setWidth(w);
    this->sourceSize.setHeight(h);
}

QRect FrameInfo::rframe() const
{
    QRect r = this->frame;
    if (this->rotated){
        qreal tmp = r.width();
        r.setWidth(r.height());
        r.setHeight(tmp);
        return r;
    }
    return r;
}

void FrameInfo::dump()
{
    qDebug()<<qPrintable(this->frameName)<<"==>";
}
