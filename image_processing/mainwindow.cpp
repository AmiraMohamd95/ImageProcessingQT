#include <rotdialog.h>
#include <math.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <stdlib.h>
#include <QStack>
#include <QGuiApplication>
#include <QScreen>
#include <QMenuBar>
#include <QImageReader>
#include <QMessageBox>
#include <QImageWriter>
#include <QScrollBar>
#include <QStandardPaths>
#include <QMouseEvent>
#include <QPainter>
#include <iostream>
#include <string>
#include <ostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    init = true;
    originStack = new QStack<ImageStruct>;
    redoStack = new QStack<ImageStruct>;
    rotationAngle = 0;
    rubberBand = NULL;
    isSelected = false;

    ui->setupUi(this);

    createActions();
    disableActions();

    ui->imageLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    //ui->imageLabel->setBackgroundRole(QPalette::Base);
    ui->scrollArea->setBackgroundRole(QPalette::Base);

    ui->imageLabel->setScaledContents(false);
    setCentralWidget(ui->scrollArea);
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ... GUI ... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */




void MainWindow::createActions(){

    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FILE MENU ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    fileToolBar->setMovable(false);

    const QIcon openIcon = QIcon::fromTheme("image-open", QIcon(":/img/open.png"));
    openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveAsIcon = QIcon::fromTheme("image-save-as", QIcon(":/img/save.png"));
    saveAsAct = new QAction(saveAsIcon, tr("&Save"), this);
    saveAsAct->setShortcuts(QKeySequence::Save);
    saveAsAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::save_as);
    fileMenu->addAction(saveAsAct);
    fileToolBar->addAction(saveAsAct);

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("image-exit", QIcon(":/img/exit.png"));
    exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ EDIT MENU ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit..."));
    editToolBar->setMovable(false);

    const QIcon undoIcon = QIcon::fromTheme("image-undo", QIcon(":/img/undo.png"));
    undoAct = new QAction(undoIcon, tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo ..."));
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);
    editMenu->addAction(undoAct);
    editToolBar->addAction(undoAct);

    const QIcon redoIcon = QIcon::fromTheme("image-redo", QIcon(":/img/redo.png"));
    redoAct = new QAction(redoIcon, tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo ..."));
    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);
    editMenu->addAction(redoAct);
    editToolBar->addAction(redoAct);

    editMenu->addSeparator();

    const QIcon zoomInIcon = QIcon::fromTheme("image-zoom-in", QIcon(":/img/zoom_in.png"));
    zoomInAct = new QAction(zoomInIcon, tr("&Zoom In"), this);
    zoomInAct->setShortcuts(QKeySequence::ZoomIn);
    zoomInAct->setStatusTip(tr("Zoom In ..."));
    connect(zoomInAct, &QAction::triggered, this, &MainWindow::zoom_in);
    editMenu->addAction(zoomInAct);
    editToolBar->addAction(zoomInAct);

    const QIcon zoomOutIcon = QIcon::fromTheme("image-zoom-out", QIcon(":/img/zoom_out.png"));
    zoomOutAct = new QAction(zoomOutIcon, tr("&Zoom Out"), this);
    zoomOutAct->setShortcuts(QKeySequence::ZoomOut);
    zoomOutAct->setStatusTip(tr("Zoom Out ..."));
    connect(zoomOutAct, &QAction::triggered, this, &MainWindow::zoom_out);
    editMenu->addAction(zoomOutAct);
    editToolBar->addAction(zoomOutAct);

    editMenu->addSeparator();

    const QIcon rotateIcon = QIcon::fromTheme("image-rotate", QIcon(":/img/rotate.png"));
    rotateAct = new QAction(rotateIcon, tr("&Rotate"), this);
    rotateAct->setStatusTip(tr("Rotate ..."));
    connect(rotateAct, &QAction::triggered, this, &MainWindow::rotate);
    editMenu->addAction(rotateAct);
    editToolBar->addAction(rotateAct);

    const QIcon resetIcon = QIcon::fromTheme("image-reset", QIcon(":/img/reset.png"));
    resetAct = new QAction(resetIcon, tr("&Reset"), this);
    resetAct->setStatusTip(tr("Reset ..."));
    connect(resetAct, &QAction::triggered, this, &MainWindow::reset);
    editMenu->addAction(resetAct);
    editToolBar->addAction(resetAct);

    const QIcon cropIcon = QIcon::fromTheme("image-crop", QIcon(":/img/crop.png"));
    cropAct = new QAction(cropIcon, tr("&crop"), this);
    cropAct->setStatusTip(tr("Crop ..."));
    connect(cropAct, &QAction::triggered, this, &MainWindow::crop);
    editMenu->addAction(cropAct);
    editToolBar->addAction(cropAct);

    editMenu->addSeparator();

    const QIcon selectIcon = QIcon::fromTheme("image-select", QIcon(":/img/select.png"));
    selectAct = new QAction(selectIcon, tr("&Select"), this);
    selectAct->setStatusTip(tr("select..."));
    connect(selectAct, &QAction::triggered, this, &MainWindow::select);
    editMenu->addAction(selectAct);
    editToolBar->addAction(selectAct);


    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ HELP MENU ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QToolBar *helpToolBar = addToolBar(tr("Help"));
    helpToolBar->setMovable(false);

    const QIcon aboutIcon = QIcon::fromTheme("image-about", QIcon(":/img/about.png"));
    aboutAct = new QAction(aboutIcon, tr("&About"), this);
    aboutAct->setStatusTip(tr("About ..."));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
    helpMenu->addAction(aboutAct);
    helpToolBar->addAction(aboutAct);
}

bool MainWindow::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }
    setImage(newImage);
    setWindowFilePath(fileName);
    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(fileName)).arg(originImage.width()).arg(originImage.height()).arg(originImage.depth());
    statusBar()->showMessage(message);
    return true;
}

bool MainWindow::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(savedImage)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(), tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}

void MainWindow::setImage(const QImage &newImage)
{


    originStack->clear();
    redoStack->clear();
    rubberBand = NULL;
    originImage = newImage;
    sourceImage = newImage;
    savedImage = newImage;
    enableActions();


    ui->imageLabel->setPixmap(QPixmap::fromImage(newImage));
    ui->scrollArea->setVisible(true);
}

void MainWindow::scaleImage(double factor)
{
    Q_ASSERT(ui->imageLabel->pixmap());

    QPixmap pixmap(*ui->imageLabel->pixmap());
    QImage img;
    img = pixmap.toImage();

    ui->imageLabel->setPixmap(ui->imageLabel->pixmap()->scaled(factor * ui->imageLabel->pixmap()->width(), factor * ui->imageLabel->pixmap()->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    adjustScrollBar(ui->scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(ui->scrollArea->verticalScrollBar(), factor);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void MainWindow::disableActions()
{
    saveAsAct->setEnabled(false);
    undoAct->setEnabled(false);
    redoAct->setEnabled(false);
    zoomInAct->setEnabled(false);
    zoomOutAct->setEnabled(false);
    rotateAct->setEnabled(false);
    resetAct->setEnabled(false);
    selectAct->setEnabled(false);
    cropAct->setEnabled(false);
}

void MainWindow::enableActions()
{
    openAct->setEnabled(true);
    saveAsAct->setEnabled(false);
    undoAct->setEnabled(false);
    redoAct->setEnabled(false);
    zoomInAct->setEnabled(true);
    zoomOutAct->setEnabled(true);
    rotateAct->setEnabled(true);
    resetAct->setEnabled(false);
    selectAct->setEnabled(true);
    cropAct->setEnabled(false);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ... SLOTS ... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void MainWindow::open()
{
    /*
    if (!init) {
      QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Save", "Do you want to save the changes?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
            save_as();
      }
    }else {
        init = false;
    }*/

    if(rubberBand != NULL){
        rubberBand->hide();
    }
    rotationAngle = 0;
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);
    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void MainWindow::save_as()
{
    if(rubberBand != NULL){
        rubberBand->hide();
    }
    QString imagePath = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("JPEG(*.jpg *.jpeg);; PNG(*.png);; Bitmap Files(*.bmp *.dib)"));
    savedImage.save(imagePath);
}

void MainWindow::zoom_in()
{

    if(rubberBand != NULL){
        rubberBand->hide();
    }
    if(isSelected){
        isSelected = false;
        if(rubberBand == NULL) return;

        double zoomScale = std::min(ui->scrollArea->width()/width,ui->scrollArea->height()/hight); // get zoom factor
        scaleImage(zoomScale); // zoom whole image
        // get top left point of the area

        QPoint loc = globalPos;// ui->imageLabel->mapFromGlobal(QPoint(std::min(c.x(),d.x()),std::min(c.y(),d.y())));  // get top left point of the area

        loc *= zoomScale;    // scale it to new position

        ui->scrollArea->setUpdatesEnabled(true);// enable scroll bars to be updated
        ui->scrollArea->horizontalScrollBar()->setUpdatesEnabled(true);
        ui->scrollArea->verticalScrollBar()->setUpdatesEnabled(true);

        ui->scrollArea->horizontalScrollBar()->setRange(0,ui->imageLabel->width()); // set range of horizntal  bar = width of label
        ui->scrollArea->horizontalScrollBar()->setValue(loc.x());   // set postion of horzintal bar= scaled top left of selected area
        ui->scrollArea->verticalScrollBar()->setRange(0,ui->imageLabel->height());  // set range of vertical  bar = height of label
        ui->scrollArea->verticalScrollBar()->setValue(loc.y());// set postion of vertical bar= scaled top left of selected area

        QScrollBar* vert = ui->scrollArea->verticalScrollBar();
        QScrollBar* horiz = ui->scrollArea->horizontalScrollBar();

        if(vert){
            vert->setEnabled(true); //enable vertical  scroll bar
        }
        if(horiz){
            horiz->setEnabled(true);//enable horizontal scroll bar
        }


        rotateAct->setEnabled(true);
        cropAct->setEnabled(false);


    }else{
        scaleImage(1.25);
        statusBar()->showMessage(tr("25% Zoom In"), 2000);
    }

    ImageStruct imgStruct;
    imgStruct.srcImg = sourceImage;
    QPixmap pixmap(*ui->imageLabel->pixmap());
    imgStruct.editedImg = savedImage = pixmap.toImage();
    originStack->push(imgStruct);
    undoAct->setEnabled(true);
    saveAsAct->setEnabled(true);
    redoAct->setEnabled(false);
    redoStack->clear();
    resetAct->setEnabled(true);
}

void MainWindow::zoom_out()
{
    if(rubberBand != NULL){
        rubberBand->hide();
    }
    if(isSelected){
        isSelected = false;
        if(rubberBand == NULL) return;
        float x = point.x() - ui->imageLabel->pos().x() - 12;
        float y = point.y() - ui->imageLabel->pos().y() - 12;
        QRect rect = rubberBand->rect();
        if (rect.isEmpty() || rect.isNull() || rect.width() <= 10 || rect.height() <= 10)
                return;
        float newScale = std::max(((float) rect.width() / ui->scrollArea->width()),
                                     ((float) rect.height() / ui->scrollArea->height()));
        scaleImage(newScale);
        float widthRatio = (float) x / (ui->imageLabel->pixmap()->width());
        float hightRatio = (float) y / (ui->imageLabel->pixmap()->height());
        ui->scrollArea->verticalScrollBar()->setValue(hightRatio * ui->imageLabel->pixmap()->height());
        ui->scrollArea->horizontalScrollBar()->setValue(widthRatio * ui->imageLabel->pixmap()->width());
        rotateAct->setEnabled(true);
        cropAct->setEnabled(false);
    }else{
        scaleImage(0.8);
        statusBar()->showMessage(tr("20% Zoom out"), 2000);
    }

    ImageStruct imgStruct;
    imgStruct.srcImg = sourceImage;
    QPixmap pixmap(*ui->imageLabel->pixmap());
    imgStruct.editedImg = savedImage = pixmap.toImage();
    originStack->push(imgStruct);
    undoAct->setEnabled(true);
    saveAsAct->setEnabled(true);
    redoAct->setEnabled(false);
    redoStack->clear();
    resetAct->setEnabled(true);
}

void MainWindow::rotate() {

    if(rubberBand != NULL){
        rubberBand->hide();
    }

    // displaying the rotation dialog
    RotDialog dialog;
    dialog.setModal(true);
    dialog.exec();

    ImageStruct imgStruct;
    QImage srcImage;
    QPixmap pixmap;
    QMatrix rm;

    srcImage = (originStack->isEmpty()) ? originImage : originStack->top().srcImg;
    pixmap = QPixmap::fromImage(srcImage);

    imgStruct.rotAngle = dialog.getDirection() * dialog.getAngle();
    rotationAngle += imgStruct.rotAngle;

    rm.translate(ui->imageLabel->width()/2, ui->imageLabel->height()/2);
    rm.rotate(rotationAngle);
    rm.translate(-ui->imageLabel->width()/2, -ui->imageLabel->height()/2);
    pixmap = pixmap.transformed(rm);

    ui->imageLabel->setPixmap(pixmap);
    ui->imageLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    // sourceImage --> the image before rotation
    imgStruct.srcImg = sourceImage;
    // savedImage --> the image after rotation
    imgStruct.editedImg = savedImage = pixmap.toImage();

    originStack->push(imgStruct);


    redoStack->clear();
    undoAct->setEnabled(true);
    redoAct->setEnabled(false);
    saveAsAct->setEnabled(true);
    resetAct->setEnabled(true);
}

void MainWindow::undo() {
    if(rubberBand != NULL){
        rubberBand->hide();
    }
    redoStack->push(originStack->pop());
    rotationAngle -= redoStack->top().rotAngle;
    redoAct->setEnabled(true);

    if(originStack->isEmpty()){
        sourceImage = savedImage = originImage;
        ui->imageLabel->setPixmap(QPixmap::fromImage(originImage));
        resetAct->setEnabled(false);
        saveAsAct->setEnabled(false);
        undoAct->setEnabled(false);
    }else{
        ui->imageLabel->setPixmap(QPixmap::fromImage(originStack->top().editedImg));

    }
}

void MainWindow::redo() {
    if(rubberBand != NULL){
        rubberBand->hide();
    }    
    originStack->push(redoStack->pop());
    undoAct->setEnabled(true);
    saveAsAct->setEnabled(true);
    resetAct->setEnabled(true);
    ui->imageLabel->setPixmap(QPixmap::fromImage(originStack->top().editedImg));
    rotationAngle += originStack->top().rotAngle;
    if(redoStack->isEmpty()){
        redoAct->setEnabled(false);
    }
}

void MainWindow::reset(){
    if(rubberBand != NULL){
        rubberBand->hide();
    }
    ui->imageLabel->setPixmap(QPixmap::fromImage(originImage));
    enableActions();
    rotationAngle = 0;
    sourceImage = savedImage = originImage;
    originStack->clear();
    redoStack->clear();

}

void MainWindow::about()
{
    if(rubberBand != NULL){
        rubberBand->hide();
    }

    QMessageBox::about(this, tr("About Photo Editor"),
            tr("<p>The <b>Photo Editor</b> is a Simple Image Processing Project "
               "that supports the following options on a photo: Open, Save As, "
               "Zoom In, Zoom Out, Undo, Redo, Select, Crop, Rotate and Reset.</p>"
               "<p>Created by:</p><p><b>1. Mayar Abd Elaziz</b></p>"
               "<p><b>2. Amira Mohamed Fathy</b></p>"
               "<p><b>3. Aya Osama</b></p><p>"
               "As a project for the <b>Human Computer Interaction</b> course.</p>"));
}

void MainWindow::select() {



    cropAct->setEnabled(true);


    if(rubberBand != NULL){
        rubberBand->hide();
    }
    ui->scrollArea->setCursor(QCursor(Qt::CrossCursor));
    isSelected = true;
}

void MainWindow::crop() {
    if(isSelected){
        if(rubberBand == NULL) return;
        rubberBand->hide();
        QImage copyImage;
        QPixmap OriginalPix(*(ui->imageLabel->pixmap()));
        QRect rect(a, b);
        QImage newImage;
        newImage = OriginalPix.toImage();
        copyImage = newImage.copy(rect);
        sourceImage = copyImage;
        rotationAngle = 0;
        ui->imageLabel->setPixmap(QPixmap::fromImage(copyImage));
        scaleFactor = 1.0;
        ui->imageLabel->resize(scaleFactor * ui->imageLabel->pixmap()->size());
        ui->imageLabel->adjustSize();
        ui->scrollArea->setBackgroundRole(QPalette::Base);
        // ui->scrollArea->setWidget(imageLabel);
        setCentralWidget(ui->scrollArea);
        rubberBand= NULL;
        isSelected = false;

        ImageStruct imgStruct;
        imgStruct.srcImg = imgStruct.editedImg = sourceImage;
        imgStruct.rotAngle = rotationAngle;
        savedImage = sourceImage;
        originStack->push(imgStruct);
        redoStack->clear();


        resetAct->setEnabled(true);
        undoAct->setEnabled(true);
        saveAsAct->setEnabled(true);
        redoAct->setEnabled(false);
        cropAct->setEnabled(false);
        rotateAct->setEnabled(true);
    }
}

QPoint pt;
QPoint diff;

void MainWindow::mousePressEvent(QMouseEvent *event){
    if(rubberBand != NULL){
        rubberBand->hide();
    }
    point = event->pos();
    firstPt = event->globalPos();
    if(isSelected){

        ui->scrollArea->setCursor(QCursor(Qt::CrossCursor));
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand->setGeometry(QRect(point, QSize()));
        rubberBand->show();
        show();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    pt = event->pos();
    diff = pt-point;
    if(isSelected){
        globalPos = ui->imageLabel->mapFromGlobal(QPoint(std::min(firstPt.x(),event->globalPos().x()),std::min(firstPt.y(),event->globalPos().y())));  // get top left point of the area
        hight = std::abs(firstPt.y()-event->globalPos().y());// set height of selectted area
        width = std::abs(firstPt.x()-event->globalPos().x()); // set width of selecte area
       rubberBand->setGeometry(QRect(point, event->pos()).normalized());
    }

}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    if(isSelected){
        a = mapToGlobal(point);
        b = event->globalPos();

        /* check all possibility */
        if(a.x() != b.x() && a.y() != b.y() && isSelected) {
               if(a.x() > b.x() && a.y() < b.y() )
               {
                   int y = a.x();
                   a.setX(b.x());
                   b.setX(y);
               }

               if(a.x() > b.x() && a.y() > b.y() )
               {
                   int x = a.x();
                   int y = a.y();
                   a.setX(b.x());
                   a.setY(b.y());
                   b.setX(x);
                   b.setY(y);
               }

               if(a.x() < b.x() && a.y() > b.y() )
               {
                   int y1 = b.y();
                   int y2 = a.y();
                   a.setY(y1);
                   b.setY(y2);
               }

            a = ui->imageLabel->mapFromGlobal(a);
            b = ui->imageLabel->mapFromGlobal(b);
        }
    }
}
