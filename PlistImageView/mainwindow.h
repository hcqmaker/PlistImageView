#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "frameinfo.h"
#include <QGraphicsScene>
#include <QModelIndex>
#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QDragEnterEvent>
#include <QDropEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    void showPlist(const QString& file);

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

public slots:
     void openFile();
     void saveFiles();
     void saveAllFiles();

     qreal updateZoom();
     void changeScaleValue(int val);
     void selectListItem(QModelIndex idx);

     void selectionChanged();

     void selectUpdate(int idx, bool bView, bool bList);
     //void focusItemChanged(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason);


     void showWarning(const QString& str);
private:
    Ui::MainWindow *_ui;
    QList<FrameInfo> _frames;
    QList<QGraphicsRectItem*> _frameItems;
    QGraphicsScene *_scene;
    QGraphicsPixmapItem *_itemImg;
    qreal _scale;
    int _tscale;

};

#endif // MAINWINDOW_H
