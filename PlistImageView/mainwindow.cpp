#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QIODevice>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QGraphicsScene>
#include <QPixmap>
#include <QSplitter>
#include <QPen>
#include <QBrush>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _scale(1.0f),
    _itemImg(NULL)
{
    this->setAcceptDrops(true);
    _ui->setupUi(this);


    _ui->horizontalSlider->setEnabled(false);
    _scene = new QGraphicsScene(this);
    _ui->graphicsView->setScene(_scene);

    _scene->clear();
    this->updateZoom();


    connect(_ui->actionOpen,SIGNAL(triggered(bool)),this, SLOT(openFile()));
    connect(_ui->actionOpenFile,SIGNAL(triggered(bool)),this, SLOT(openFile()));
    connect(_ui->actionSaveFile,SIGNAL(triggered(bool)),this, SLOT(saveFiles()));
    connect(_ui->actionSave,SIGNAL(triggered(bool)),this, SLOT(saveFiles()));
    connect(_ui->actionSave_all,SIGNAL(triggered(bool)),this, SLOT(saveAllFiles()));
    connect(_ui->horizontalSlider,SIGNAL(valueChanged(int)),this, SLOT(changeScaleValue(int)));
    connect(_ui->listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(selectListItem(QModelIndex)));
    connect(_scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));


}

MainWindow::~MainWindow()
{
    delete _ui;
}

void MainWindow::showPlist(const QString& filename)
{
    QString pngfile = filename;
    QString plistfile = filename;
    if (filename.endsWith(".plist")){
        pngfile.replace(QString(".plist"), QString(".png"));
    }else{
        plistfile.replace(QString(".png"), QString(".plist"));
    }

    if (!QFile::exists(pngfile)){
        showWarning(tr("can't find file %1.").arg(pngfile));
        return;
    }

    if (!QFile::exists(plistfile)){
        showWarning(tr("can't find file %1.").arg(plistfile));
        return;
    }

//    qDebug("%s:%s",pngfile.toLocal8Bit().data(), plistfile.toLocal8Bit().data());

    QFile file(plistfile);
    if (!file.open(QIODevice::ReadOnly)){
       showWarning(tr("can't open file %1.").arg(plistfile));
       return;
    }
    QDomDocument doc("mydocument");
      if (!doc.setContent(&file)) {
          file.close();
          showWarning(tr("can't parse xml file %1.").arg(plistfile));
          return;
      }
      file.close();

      this->_frames.clear();

      // 解析plist获取需要的数据
      QDomElement plistElem = doc.documentElement();
      QDomElement dictElem = plistElem.firstChild().toElement();
      QDomElement framesKeyElem = dictElem.firstChild().toElement();
      QDomElement framesDictElem = framesKeyElem.nextSibling().toElement();
      QDomElement frameInfoKeyElem = framesDictElem.firstChild().toElement();
      while(!frameInfoKeyElem.isNull()){
          FrameInfo frameInfo;
          QString key = frameInfoKeyElem.text();
          frameInfo.frameName = key;

          //qDebug() << "key:" << qPrintable(key);

          QDomElement frameInfoDictElem = frameInfoKeyElem.nextSibling().toElement();
          QDomElement nextEl = frameInfoDictElem.firstChild().toElement();
          while(!nextEl.isNull()){
              QString nextKey = nextEl.text();
              QDomElement nextData = nextEl.nextSibling().toElement();
              if (nextKey == "frame"){
                frameInfo.parseFrame(nextData.text());
              }else if (nextKey == "offset"){
                frameInfo.parseOffset(nextData.text());
              }else if (nextKey == "rotated"){
                frameInfo.rotated = nextData.tagName().contains("true");
              }else if (nextKey == "sourceColorRect"){
                frameInfo.parseSourceColorRect(nextData.text());
              }else if (nextKey == "sourceSize"){
                frameInfo.parseSourceSize(nextData.text());
              }

              nextEl = nextData.nextSibling().toElement();
          }

          frameInfoKeyElem = frameInfoDictElem.nextSibling().toElement();

          //frameInfo.dump();
          this->_frames.append(frameInfo);
      }

      // 把图片显示出来
      _ui->horizontalSlider->setEnabled(true);

      _scene->clear();
      _frameItems.clear();

      QPixmap pixmap(pngfile);
      _itemImg = _scene->addPixmap(pixmap);


      QStringList strls;
      foreach(const FrameInfo &info , this->_frames){

          strls << info.frameName;
          QRect rf = info.rframe();

          QPen pen;
          QBrush brush(Qt::cyan);

          QGraphicsRectItem* item = _scene->addRect(rf.x(), rf.y(), rf.width(), rf.height(), pen, brush);
          _frameItems.append(item);
          item->setAcceptTouchEvents(true);
          item->setOpacity(0.5);
          item->setFlag(QGraphicsItem::ItemIsSelectable);


      }

      _ui->listWidget->addItems(strls);

}

qreal MainWindow::updateZoom()
{
    int val = _ui->horizontalSlider->value();
    _ui->label->setText(QString::number(val/50.0f, 'f', 1));

    qreal ret = val > _tscale ? (1 + 0.1) : (1- 0.1);
    _tscale = val;

    return ret;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasFormat("text/uri-list"))
        e->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *e)
{
    QList<QUrl> urls = e->mimeData()->urls();
    if(urls.isEmpty())
        return ;

    QString fileName = urls.first().toLocalFile();
    if(fileName.isEmpty())
        return;

    this->showPlist(fileName);
}


void MainWindow::changeScaleValue(int val)
{

    qreal uscale = this->updateZoom();
    _scale *= uscale;

    _ui->graphicsView->scale(uscale, uscale);
}

void MainWindow::selectListItem(QModelIndex idx)
{
    this->selectUpdate(idx.row(), true, false);
}

void MainWindow::selectionChanged()
{
    // 更新图片上的显示
    foreach(QGraphicsRectItem *item, _frameItems){
        item->setBrush(QBrush(Qt::cyan));
    }

    QList<QGraphicsItem *> items = _scene->selectedItems();
    foreach(QGraphicsItem *item, items){
        QGraphicsRectItem*n = (QGraphicsRectItem*)item;
        if (n){
            n->setBrush(QBrush(Qt::red));
            int idx = _frameItems.indexOf(n);
            this->selectUpdate(idx, false, true);
        }
    }
}

void MainWindow::selectUpdate(int idx, bool bView, bool bList)
{
    if (idx <= -1){
        return;
    }
    if (bView){
        QGraphicsRectItem *item = _frameItems.at(idx);
        item->setBrush(QBrush(Qt::red));
    }
    if (bList){
        QListWidgetItem *litem = _ui->listWidget->item(idx);
        litem->setSelected(true);
    }
}

void MainWindow::showWarning(const QString& str)
{
    QMessageBox::warning(this, tr("warning"), str);
}

void MainWindow::openFile()
{
    QStringList filters;
    filters << "Image files (*.png)"
              << "Text files (*.plist)"
              << "Any files (*)";

    QFileDialog dialog(this);
    dialog.setNameFilters(filters);
    if (dialog.exec()){
        QStringList list = dialog.selectedFiles();
        // 直接用第1个文件就好了
        if (list.size() > 0)
            this->showPlist(list.at(0));
    }

}

void MainWindow::saveFiles()
{
    if (!_itemImg){
        showWarning(tr("you need to open a plist file first."));
        return;
    }


    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

//    qDebug() << qPrintable(dir);
    QPixmap img = _itemImg->pixmap();
    QList<QGraphicsItem *> items = _scene->selectedItems();
    foreach(QGraphicsItem *item, items){
        QGraphicsRectItem*n = (QGraphicsRectItem*)item;
        if (n){
            n->setBrush(QBrush(Qt::red));
            int idx = _frameItems.indexOf(n);
            if (idx != -1){
                FrameInfo info = _frames.at(idx);
                const QRect r = info.rframe();
                QPixmap subimg = img.copy(r.x(), r.y(), r.width(), r.height());
                QString spath = dir + "/" + info.frameName;
                QFileInfo finfo(spath);
                QDir dd = finfo.dir();
                if (!dd.exists()){
                    dd.mkpath(dd.dirName());
                }

                subimg.save(spath, "PNG");

            }
        }
    }

}

void MainWindow::saveAllFiles()
{
    if (!_itemImg){
        showWarning(tr("you need to open a plist file first."));
        return;
    }

    QPixmap img = _itemImg->pixmap();
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    foreach(const FrameInfo& info, _frames){
        const QRect r = info.rframe();
        QPixmap subimg = img.copy(r.x(), r.y(), r.width(), r.height());
        QString spath = dir + "/" + info.frameName;
        QFileInfo finfo(spath);
        QDir dd = finfo.dir();
        if (!dd.exists()){
            dd.mkpath(dd.dirName());
        }

        subimg.save(spath, "PNG");
    }

}
