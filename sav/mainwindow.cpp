#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMediaService>
#include <QMediaRecorder>
#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QMediaMetaData>

#include <QMessageBox>
#include <QPalette>

#include <QDebug>

#include <QtWidgets>


#include <QtPrintSupport/qtprintsupportglobal.h>

#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

#include "commandes.h"

Q_DECLARE_METATYPE(QCameraInfo)


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->showFullScreen();

    settings=new QSettings(QString("config.ini"),QSettings::IniFormat);

    settings->setValue("version", 1);

    // ouvre la fenetre de commande
    commandes= new Commandes(this);
    commandes->showFullScreen();
    commandes->setModal(false);
    commandes->open();

    //Camera devices:
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();

    setCamera(QCameraInfo::defaultCamera());

    connect(commandes,&Commandes::takePhoto,this,&MainWindow::takeImage);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setCamera(const QCameraInfo &cameraInfo)
{
    m_camera.reset(new QCamera(cameraInfo));


    connect(m_camera.data(), &QCamera::stateChanged, this, &MainWindow::updateCameraState);
    connect(m_camera.data(), QOverload<QCamera::Error>::of(&QCamera::error), this, &MainWindow::displayCameraError);

    m_imageCapture.reset(new QCameraImageCapture(m_camera.data()));
    m_imageCapture.get()->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

    // Définition de la camera
        QCameraViewfinderSettings viewfinderSettings;
        viewfinderSettings.setResolution(1920, 1080);
        //viewfinderSettings.setResolution(1280, 720);
        viewfinderSettings.setMaximumFrameRate(5.0);
        //viewfinderSettings.setPixelFormat(QVideoFrame::Format_YUYV);
        viewfinderSettings.setPixelFormat(QVideoFrame::Format_YV12);
        m_camera->setViewfinderSettings(viewfinderSettings);

        m_camera->setViewfinder(ui->viewfinder);

    updateCameraState(m_camera->state());
    updateLockStatus(m_camera->lockStatus(), QCamera::UserRequest);

    connect(m_imageCapture.data(), &QCameraImageCapture::readyForCaptureChanged, this, &MainWindow::readyForCapture);
    connect(m_imageCapture.data(), &QCameraImageCapture::imageCaptured, this, &MainWindow::processCapturedImage);
    connect(m_imageCapture.data(), &QCameraImageCapture::imageSaved, this, &MainWindow::imageSaved);
    connect(m_imageCapture.data(), QOverload<int, QCameraImageCapture::Error, const QString &>::of(&QCameraImageCapture::error),
            this, &MainWindow::displayCaptureError);

    connect(m_camera.data(), QOverload<QCamera::LockStatus, QCamera::LockChangeReason>::of(&QCamera::lockStatusChanged),
            this, &MainWindow::updateLockStatus);

    updateCaptureMode();

    displayViewfinder();
    m_camera->start();

}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    if (event->isAutoRepeat())
        return;

    switch (event->key()) {
    case Qt::Key_CameraFocus:
        displayViewfinder();
        m_camera->searchAndLock();
        event->accept();
        break;
    case Qt::Key_Camera:
        if (m_camera->captureMode() == QCamera::CaptureStillImage) {
            takeImage();
        } else {

        }
        event->accept();
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    switch (event->key()) {
    case Qt::Key_CameraFocus:
        m_camera->unlock();
        break;
    default:
        QMainWindow::keyReleaseEvent(event);
    }
}


void MainWindow::processCapturedImage(int requestId, const QImage& img)
{
    qDebug() << "processCapturedImage";
    
    Q_UNUSED(requestId);

    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("yyyy-MM-dd-hh-mm-ss");

    qDebug() << "Date:"+formattedTime;

    QString imagePath;
    QString masquePath;

    imagePath=settings->value("imagePath","./photos").toString();
    masquePath=settings->value("maskPath","./masque").toString();

    // saveImage
    QString filename=imagePath + "/" +formattedTime + "_photo.jpeg";
    img.save(filename,"JPEG");
    qDebug() << "image save " << filename;

    QImage masque(masquePath+"/fond.png","PNG");

    QImage compo(img);
    QPainter p(&compo);
    //p.begin(this);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);

    p.drawImage(0,0,masque);
    p.end();
    
    filename=imagePath + "/" +formattedTime + "_photo-compo.jpeg";
    compo.save(filename,"JPEG");

    qDebug() << "compo save " << filename;
    QImage scaledImage = compo.scaled(ui->viewfinder->size(),
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);

    ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));

    // Display captured image for 4 seconds.
    displayCapturedImage();
    QTimer::singleShot(4000, this, &MainWindow::displayViewfinder);

}

void MainWindow::configureCaptureSettings()
{

}

void MainWindow::toggleLock()
{
    qDebug() << "toggleLock " << m_camera->lockStatus();
    switch (m_camera->lockStatus()) {
    case QCamera::Searching:
    case QCamera::Locked:
        m_camera->unlock();
        break;
    case QCamera::Unlocked:
        m_camera->searchAndLock();
    }
}

void MainWindow::updateLockStatus(QCamera::LockStatus status, QCamera::LockChangeReason reason)
{
    QColor indicationColor = Qt::black;

    switch (status) {
    case QCamera::Searching:
        indicationColor = Qt::yellow;
        qDebug() << tr("Focusing...");
        //ui->lockButton->setText(tr("Focusing..."));
        break;
    case QCamera::Locked:
        indicationColor = Qt::darkGreen;
        qDebug() << tr("Locked");
        //ui->statusbar->showMessage(tr("Focused"), 2000);
        break;
    case QCamera::Unlocked:
        indicationColor = reason == QCamera::LockFailed ? Qt::red : Qt::black;
        qDebug() << tr("Unlocked");
        if (reason == QCamera::LockFailed)
            qDebug() << tr("Focus Failed");
    }

}

void MainWindow::takeImage()
{
    qDebug() << "takeImage";
    m_isCapturingImage = true;

    m_imageCapture->capture();
}

void MainWindow::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString)
{
    qDebug() << "displayCaptureError " << errorString;
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    m_isCapturingImage = false;
}

void MainWindow::startCamera()
{
    m_camera->start();
}

void MainWindow::stopCamera()
{
    m_camera->stop();
}

void MainWindow::updateCaptureMode()
{
    qDebug() << "updateCaptureMode";
    //int tabIndex = ui->captureWidget->currentIndex();
    QCamera::CaptureModes captureMode = QCamera::CaptureStillImage; //tabIndex == 0 ? QCamera::CaptureStillImage : QCamera::CaptureVideo;

    if (m_camera->isCaptureModeSupported(captureMode))
        m_camera->setCaptureMode(captureMode);
    else qDebug() << "Mode capture non supporte";

}

void MainWindow::updateCameraState(QCamera::State state)
{
    qDebug() << "updateCameraState";
    switch (state) {
    case QCamera::ActiveState:
        qDebug() << "ActiveState";
        break;
    case QCamera::UnloadedState:
    case QCamera::LoadedState:
        qDebug() << "LoadedState/UnloadedState";
        break;
    }
}


void MainWindow::setExposureCompensation(int index)
{
    m_camera->exposure()->setExposureCompensation(index*0.5);
}

void MainWindow::displayCameraError()
{
    QMessageBox::warning(this, tr("Camera Error"), m_camera->errorString());
}

void MainWindow::updateCameraDevice(QAction *action)
{
    setCamera(qvariant_cast<QCameraInfo>(action->data()));
}

void MainWindow::displayViewfinder()
{
    qDebug() << "displayViewfinder";
    ui->stackedWidget->setCurrentIndex(0);
    commandes->setMode(Commandes::wait);
}

void MainWindow::displayCapturedImage()
{
    qDebug() << "displayCapturedImage";
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::readyForCapture(bool ready)
{
    qDebug() << "readyForCapture";
    //ui->takeImageButton->setEnabled(ready);
}

void MainWindow::imageSaved(int id, const QString &fileName)
{
    Q_UNUSED(id);
    //ui->statusbar->showMessage(tr("Captured \"%1\"").arg(QDir::toNativeSeparators(fileName)));

    qDebug() << "image saved : " << fileName;

    m_isCapturingImage = false;
    if (m_applicationExiting)
        close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_isCapturingImage) {
        setEnabled(false);
        m_applicationExiting = true;
        event->ignore();
    } else {
        event->accept();
    }
}


void MainWindow::preview()
{
QPrinter printer(QPrinter::HighResolution);
QPrintPreviewDialog preview(&printer, this);
connect(&preview, SIGNAL(paintRequested(QPrinter*)),
        this, SLOT(printDocument(QPrinter*)));
preview.exec();
}

void MainWindow::printDocument(QPrinter *printer)
{

    printer->setFromTo(1, 1);  // une seule page imprimée

    //QProgressDialog progress(tr("Preparing font samples..."), tr("&Cancel"),
    //                         0, pageMap.count(), this);
    //progress.setWindowModality(Qt::ApplicationModal);
    //progress.setWindowTitle(tr("Font Sampler"));
    //progress.setMinimum(printer->fromPage() - 1);
    //progress.setMaximum(printer->toPage());

    QPainter painter;
    painter.begin(printer);
    bool firstPage = true;

    for (int page = printer->fromPage(); page <= printer->toPage(); ++page) {

        if (!firstPage)
            printer->newPage();

        qApp->processEvents();
        //if (progress.wasCanceled())
        //   break;

        printPage(page - 1, &painter, printer);
        //progress.setValue(page);
        firstPage = false;
    }

    painter.end();

}

void MainWindow::printPage(int index, QPainter *painter, QPrinter *printer)
{

    //QString family = pageMap.keys()[index];
    //StyleItems items = pageMap[family];

    // Find the dimensions of the text on each page.
    /*
    qreal width = 0.0;
    qreal height = 0.0;
    foreach (QTreeWidgetItem *item, items) {
        QString style = item->text(0);
        int weight = item->data(0, Qt::UserRole).toInt();
        bool italic = item->data(0, Qt::UserRole + 1).toBool();

        // Calculate the maximum width and total height of the text.

        foreach (int size, sampleSizes) {
            QFont font(family, size, weight, italic);
            font.setStyleName(style);
            font = QFont(font, painter->device());
            QFontMetricsF fontMetrics(font);
            QRectF rect = fontMetrics.boundingRect(
            QString("%1 %2").arg(family).arg(style));
            width = qMax(rect.width(), width);
            height += rect.height();
        }

    }
    */

     /*
    qreal xScale = printer->pageRect().width() / width;
    qreal yScale = printer->pageRect().height() / height;
    qreal scale = qMin(xScale, yScale);


    qreal remainingHeight = printer->pageRect().height()/scale - height;
    qreal spaceHeight = (remainingHeight / 4.0) / (items.count() + 1);
    qreal interLineHeight = (remainingHeight / 4.0) / (sampleSizes.count() * items.count());

    painter->save();
    painter->translate(printer->pageRect().width() / 2.0, printer->pageRect().height() / 2.0);
    painter->scale(scale, scale);
    painter->setBrush(QBrush(Qt::black));

    qreal x = -width / 2.0;
    qreal y = -height / 2.0 - remainingHeight / 4.0 + spaceHeight;

    foreach (QTreeWidgetItem *item, items) {
        QString style = item->text(0);
        int weight = item->data(0, Qt::UserRole).toInt();
        bool italic = item->data(0, Qt::UserRole + 1).toBool();

        // Draw each line of text.
        foreach (int size, sampleSizes) {
            QFont font(family, size, weight, italic);
            font.setStyleName(style);
            font = QFont(font, painter->device());
            QFontMetricsF fontMetrics(font);
            QRectF rect = fontMetrics.boundingRect(QString("%1 %2").arg(
                          font.family()).arg(style));
            y += rect.height();
            painter->setFont(font);
            painter->drawText(QPointF(x, y), QString("%1 %2").arg(family).arg(style));
            y += interLineHeight;
        }
        y += spaceHeight;
    }
*/
    painter->restore();

}
