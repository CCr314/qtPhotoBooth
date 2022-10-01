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
#include <QStorageInfo>
#include <QPrintPreviewDialog>

#include "commandes.h"
#include "resultat.h"

#include "params.h"

Q_DECLARE_METATYPE(QCameraInfo)


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->showFullScreen();

    qApp->processEvents();

    // ouvre la fenetre de commande
    commandes= new Commandes(this);

    commandes->open();

    //Camera devices:
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();

    setCamera(QCameraInfo::defaultCamera());

    connect(commandes,&Commandes::takePhoto,this,&MainWindow::takeImage);
    commandes->setMode(Commandes::wait);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setCamera(const QCameraInfo &cameraInfo)
{
    qDebug() << "setCamera";
    m_camera.reset(new QCamera(cameraInfo));


    connect(m_camera.data(), &QCamera::stateChanged, this, &MainWindow::updateCameraState);
    connect(m_camera.data(), QOverload<QCamera::Error>::of(&QCamera::error), this, &MainWindow::displayCameraError);

    m_imageCapture.reset(new QCameraImageCapture(m_camera.data()));
    m_imageCapture.get()->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

    // Définition de la camera
    QCameraViewfinderSettings viewfinderSettings;
    viewfinderSettings.setResolution(1920,1080);
    viewfinderSettings.setMaximumFrameRate(5.0);
    viewfinderSettings.setPixelFormat(QVideoFrame::Format_YUYV);
    //viewfinderSettings.setPixelFormat(QVideoFrame::Format_YV12);
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

    m_camera->start();

}

void MainWindow::processCapturedImage(int requestId, const QImage& img)
{
    qDebug() << "processCapturedImage";
    Q_UNUSED(requestId);

    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("yyyy-MM-dd-hh-mm-ss");

    qDebug() << "Date:"+formattedTime;

    qDebug() << "image capture size " << img.width() << "X" << img.height();

    QImage masque(params::getMasquePath()+"/fond.png","PNG");

    int imprWidth = params::getImprSize().width();
    int imprHeight = params::getImprSize().height();

    QImage compo(imprWidth,imprHeight,QImage::Format_RGB32);
    QPainter p(&compo);

    p.begin(this);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QRectF target(0.0, 0.0, imprWidth, imprHeight);

    qreal width=(img.width()-imprWidth)/2;
    qreal height=(img.height()-imprHeight)/2;
    QRectF source(width, height, imprWidth, imprHeight);

    p.drawImage(target, img, source);

    p.drawImage(0,0,masque);
    p.end();

    QImage scaledImage = compo.scaled(ui->viewfinder->size(),
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);

    qDebug() << "lastImagePreviewLabel";

    Resultat resultat(compo,this);

    if (resultat.exec()) {
        qDebug() << "lance l'impression";

        if(params::isModePreview())
            preview(compo);
        else {
            print(compo);
        }

        // saveImage
        QString filename = params::getImagePath() + "/" +formattedTime + "_photo.jpeg";
        img.save(filename,"JPEG");
        qDebug() << "image save " << filename;
        filename=params::getImagePath() + "/" +formattedTime + "_photo-compo.jpeg";
        compo.save(filename,"JPEG");
        qDebug() << "compo save " << filename;
    }

    commandes->setMode(Commandes::wait);


}

void MainWindow::keyPressEvent(QKeyEvent*)
{

}

void MainWindow::keyReleaseEvent(QKeyEvent*)
{

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

    qDebug() << "updateLockStatus " << status;
    switch (status) {
    case QCamera::Searching:
        indicationColor = Qt::yellow;
        qDebug() << tr("Focusing...");
        //ui->lockButton->setText(tr("Focusing..."));
        break;
    case QCamera::Locked:
        indicationColor = Qt::darkGreen;
        qDebug() << tr("Unlock");
        //ui->statusbar->showMessage(tr("Focused"), 2000);
        break;
    case QCamera::Unlocked:
        indicationColor = reason == QCamera::LockFailed ? Qt::red : Qt::black;
        qDebug() << tr("Focus");
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
    qDebug() << "updateCaptureMode ";
    //int tabIndex = ui->captureWidget->currentIndex();
    QCamera::CaptureModes captureMode = QCamera::CaptureStillImage; //tabIndex == 0 ? QCamera::CaptureStillImage : QCamera::CaptureVideo;

    if (m_camera->isCaptureModeSupported(captureMode))
        m_camera->setCaptureMode(captureMode);
    else qDebug() << "Mode capture non supporte";

}

void MainWindow::updateCameraState(QCamera::State state)
{
        qDebug() << "updateCameraState " << state;
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
    qDebug() << "displayCameraError " << m_camera->errorString() << " no "<< m_camera->error();

    QMessageBox::warning(this, tr("Camera Error"), m_camera->errorString());

}

void MainWindow::updateCameraDevice(QAction *action)
{
    qDebug() << "updateCameraDevice ";
    setCamera(qvariant_cast<QCameraInfo>(action->data()));
}

void MainWindow::displayViewfinder()
{


}

void MainWindow::displayCapturedImage()
{

}

void MainWindow::readyForCapture(bool ready)
{
    qDebug() << "readyForCapture ";
    //ui->takeImageButton->setEnabled(ready);
}

void MainWindow::imageSaved(int id, const QString &fileName)
{
        qDebug() << "imageSaved ";
    Q_UNUSED(id);
    //ui->statusbar->showMessage(tr("Captured \"%1\"").arg(QDir::toNativeSeparators(fileName)));

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

void MainWindow::print(QImage img)
{
QPrinter printer(QPrinter::HighResolution);

toPrint = &img;

    printDocument( &printer);
}

void MainWindow::preview(QImage img)
{
QPrinter printer(QPrinter::HighResolution);
QPrintPreviewDialog preview(&printer, this);
toPrint = &img;
connect(&preview, SIGNAL(paintRequested(QPrinter*)),
        this, SLOT(printDocument(QPrinter*)));
preview.exec();
}


void MainWindow::printDocument(QPrinter *printer)
{

    printer->setFromTo(1, 1);  // une seule page imprimée

    printer->setFullPage(true);
    printer->setOrientation(QPrinter::Landscape);
    printer->setPageSize(QPageSize(QPageSize::Postcard));
    printer->setPageMargins(QMarginsF(0,0,0,0));

    qDebug() << printer->pageRect().width() << "x" << printer->pageRect().height();

    QPainter painter;
    painter.begin(printer);
    bool firstPage = true;

    for (int page = printer->fromPage(); page <= printer->toPage(); ++page) {

        if (!firstPage)
            printer->newPage();

        qApp->processEvents();
        printPage(&painter, printer);

        QString printerState;
        switch(printer->printerState() )
        {
            case QPrinter::Idle: printerState="Idle"; break;
            case QPrinter::Active : printerState="Active"; break;
            case QPrinter::Aborted :printerState="Aborted"; break;
            case QPrinter::Error : printerState="Error"; break;
        }

        if(printer->printerState()!= QPrinter::Active)
        {
            QMessageBox::warning(this, tr("Photobooth - Erreur Imprimante"),
                                           printerState);
        }
        firstPage = false;
    }

    painter.end();

}

void MainWindow::printPage( QPainter *painter, QPrinter *printer)
{


    qreal width = toPrint->width();
    qreal height = toPrint->height();

    qreal xScale = printer->pageRect().width() / width;
    qreal yScale = printer->pageRect().height() / height;

    qDebug() << "scale : " << xScale << "," << yScale;

    qreal scale = qMin(xScale, yScale);

    painter->save();
    painter->scale(scale,scale);

    painter->drawImage(0,0,*toPrint);
    painter->restore();

}
