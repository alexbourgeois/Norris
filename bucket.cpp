#include "bucket.hpp"

Bucket::Bucket(NodeSettings settings, QWidget * parent) :
	QMainWindow(parent),
	ui(new Ui::Bucket)
{
	ui->setupUi(this);
	InitializeWindow();

    connect(ui->lineEdit, SIGNAL(editingFinished()), this, SLOT(setTargetDirectory()));

    width = 315;//this->size().width();
    height = 444;//this->size().height();

    this->resize(width, 0);
    imageNumber = 0;

    if(settings.nodeType == NodeType::Pin)
        InitializePin(settings.path);

    if(settings.nodeType == NodeType::Clip)
        InitializeClip();

    this->adjustSize();
    Close();
    this->setVisible(false);
}

Bucket::~Bucket()
{

}

void Bucket::SetNewPath(QString path) {
    qDebug() << path << path.replace("\\", "/");
    workingDirectory.setPath(path);
    ui->lineEdit->setText(workingDirectory.absolutePath() + "/");
}

void Bucket::InitializeClip() {
    listWidget = new QListWidget(this);
    listWidget->setAccessibleName("listWidget");
    ui->verticalLayout->addWidget(listWidget);
    listWidget->show();

    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(listWidgetItemDoubleClicked(QListWidgetItem*)));

    listWidget->setViewMode(QListWidget::IconMode);
//    qDebug() << "Width : " << listWidget->width();
//    qDebug() << "Width2 : " << listWidget->size().width();
//    qDebug() << "Width2 : " <<  this->width;
    //listWidget->setIconSize(QSize(300, this->width));
    listWidget->setIconSize(QSize(300, 400));
    listWidget->setResizeMode(QListWidget::Adjust);
    listWidget->setStyleSheet(QString("background-color: rgba(255, 255, 255, 255);"));

    ui->openExplorerButton->setStyleSheet(QString("background-color: rgba(255, 255, 255, 255);"));
    //Set working path
    workingDirectory.setPath("D:/Content/");//QDir::currentPath() + "/");
    ui->lineEdit->setText(workingDirectory.absolutePath() + "/");
    UpdateVisibleFiles();
}

void Bucket::InitializePin(QString path) {
    QLabel *label = new QLabel(this);
    ui->verticalLayout->addWidget(label);
    QPixmap pixmap(path);
    label->setPixmap(pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));

//    QStatusBar *menuBar = new QStatusBar(this);
//    ui->verticalLayout->addWidget(menuBar);

    this->adjustSize();
    width = this->size().width();
    height = this->size().height();
    qDebug() << "Path of file : " << path;
}

void Bucket::addText(QString str) {
    auto newLabel = new QLabel(listWidget);
    newLabel->setText(str);
    auto item = new QListWidgetItem(str);
    listWidget->addItem(item);
}

void Bucket::setTargetDirectory() {
    auto path = ui->lineEdit->text();

    if(path == workingDirectory.absolutePath())
        return;

    QDir dir(path);
    if (!dir.exists()) {
        qDebug() << "Path " << path << " doesn't exist";
        if( dir.mkpath("."))
            qDebug() << "Path created";
        else {
            qDebug() << "Failed to create path";
            return;
        }
    }
    SetNewPath(path);
    qDebug() << "New path set";

    listWidget->clear();
    UpdateVisibleFiles();
}

void Bucket::InitializeWindow()
{
	Qt::WindowFlags flags2;

    flags2 |= Qt::FramelessWindowHint;
	//flags2 |= Qt::X11BypassWindowManagerHint;
	flags2 |= Qt::WindowStaysOnTopHint;
	//     //flags |= Qt::WA_TranslucentBackground;
	this->setWindowFlags(flags2);
}

void Bucket::Open() {
    this->setVisible(true);
    QPropertyAnimation *animation = new QPropertyAnimation(this, "size");
    animation->setDuration(200);
//    animation->setStartValue(QRect(this->pos().x(), this->pos().y(), width, 0));
//    animation->setEndValue(QRect(this->pos().x(), this->pos().y(), width, height));
    animation->setStartValue(QSize(width, 0));
    animation->setEndValue(QSize(width, height));
    animation->setEasingCurve(QEasingCurve::OutExpo);

    animation->start();

    isClosed = false;
}

void Bucket::Close() {
    QPropertyAnimation *animation = new QPropertyAnimation(this, "size");
    animation->setDuration(200);
//    animation->setStartValue(QRect(this->pos().x(), this->pos().y(), width, height));
//    animation->setEndValue(QRect(this->pos().x(), this->pos().y(), width, 0));
    animation->setStartValue(QSize(width, height));
    animation->setEndValue(QSize(width, 0));
    animation->setEasingCurve(QEasingCurve::OutExpo);
    //connect(animation, SIGNAL(finished()), this, SLOT(MoveToNode()));

    animation->start();

    isClosed = true;
}

bool Bucket::IsImage(QString suffix)
{
    suffix = suffix.toLower();
    if(suffix.contains("gif") || suffix.contains("jpeg") || suffix.contains("jpg") || suffix.contains("png"))
        return true;

    return false;
}

void Bucket::GetADropEvent(QDropEvent *event)
{
    dropEvent(event);
}

//void Bucket::mouseMoveEvent(QMouseEvent *event)
//{
//    if (event->buttons() & Qt::LeftButton) {
//        int distance = (event->pos() - startPos).manhattanLength();
//        if (distance >= QApplication::startDragDistance())
//            performDrag();
//    }
//    QTableWidget::mouseMoveEvent(event);
//}

//void Bucket::performDrag()
//{
//    QString plainText = selectionAsPlainText();
//    if (plainText.isEmpty())
//        return;

//    QMimeData *mimeData = new QMimeData;
//    mimeData->setText(ui->listWidget->selectedItems().at(0)->text());;
////    mimeData->setHtml(toHtml(plainText));
////    mimeData->setData("text/csv", toCsv(plainText).toUtf8());

//    QDrag *drag = new QDrag(this);
//    drag->setMimeData(mimeData);
//    if (drag->exec(Qt::CopyAction | Qt::MoveAction) == Qt::MoveAction)
//        deleteSelection();
//}


void Bucket::enterEvent(QEvent* e) {
	QWidget::enterEvent(e);
}

void Bucket::dragLeaveEvent(QDragLeaveEvent *event)
{
    qDebug() << "Drag event !";
startDrag();
    //event->acceptProposedAction();

}


void Bucket::createData(const QString &mimeType)
{
    if (mimeType != "image/png")
        return;

//    QImage image(imageLabel->size(), QImage::Format_RGB32);
//    QPainter painter;
//    painter.begin(&image);
//    imageLabel->renderer()->render(&painter);
//    painter.end();

    QByteArray data;
//    QBuffer buffer(&data);
//    buffer.open(QIODevice::WriteOnly);
//    image.save(&buffer, "PNG");
//    buffer.close();

    mimeData->setData("image/png", data);
}

void Bucket::startDrag()
{
    mimeData = new MimeData;

    connect(mimeData, SIGNAL(dataRequested(QString)),
            this, SLOT(createData(QString)), Qt::DirectConnection);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(QPixmap(":/images/drag.png"));

    drag->exec(Qt::CopyAction);
}













void Bucket::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Bucket mouse press event !!!!!";
//    if (!(event->buttons() & Qt::LeftButton))
//        return;
//    qDebug("Drag event");
//    QDrag *drag = new QDrag(this);
//    QMimeData *mimeData = new QMimeData;

//    mimeData->setData("text/plain", "Coucou");
//    drag->setMimeData(mimeData);

//    Qt::DropAction dropAction = drag->exec();
//    event->accept();
}

//Limited by parent border
void Bucket::MoveTo(QPoint target){
    QPoint newPos;
    newPos = target;
    // ---- X part ----
    auto bucketNewXAndWidth = target.x() + this->size().width();
    auto nodeDrawer = (QWidget*)this->parent();
    auto nodeDrawerWidth = nodeDrawer->size().width();
    auto nodeDrawerXAndWidth = nodeDrawer->pos().x() + nodeDrawerWidth;
    auto bucketWidth = this->size().width();
    //Check right
    if(bucketNewXAndWidth > nodeDrawerXAndWidth)
        newPos.setX(nodeDrawerWidth - bucketWidth);

    //check left
    if(target.x() < nodeDrawer->pos().x())
        newPos.setX(nodeDrawer->pos().x());

    // ---- Y part ----
    auto bucketNewYAndHeight = target.y() + this->size().height();
    auto nodeDrawerHeight= nodeDrawer->size().height();
    auto nodeDrawerYAndHeight = nodeDrawer->pos().y() + nodeDrawerHeight;
    auto bucketHeight = this->size().height();
    //check bottom
    if(bucketNewYAndHeight > nodeDrawerYAndHeight)
        newPos.setY(nodeDrawerHeight - bucketHeight);

    //Check top
    if(target.y() < nodeDrawer->pos().y())
        newPos.setY(nodeDrawer->pos().y());


    this->move(newPos);
}

void Bucket::UpdateVisibleFiles() {
    //Add not already present file to bucket
    QDirIterator it(workingDirectory);
    while (it.hasNext()) {
        it.next();
        //qDebug() << it.next();
        if(it.fileName() != "." && it.fileName() != "..")
        {
            bool addFile = true;
            //Check if file exists in list
            for(auto i=0 ; i<listWidget->count() ; i++) {
                QStringList stringList = listWidget->item(i)->text().split("/");
                QString fileName = stringList.last();
            //    qDebug() << "Comparing " << it.fileName() << " with" << fileName << ":" << fileName.compare(it.fileName());
                if(fileName.compare(it.fileName()) > 0) {
            //        qDebug() << "File already visible";
                    addFile = false;
                    break;
                }
            }
            if(addFile) {
                qDebug() << "Adding new element : " << workingDirectory.absolutePath() << "/" << it.fileName() << it.fileInfo().suffix();

                QListWidgetItem* newItem = new QListWidgetItem();
                if(IsImage(it.fileInfo().suffix())) {
                    newItem->setIcon(QIcon(workingDirectory.absolutePath() + "/" + it.fileName()));
                }
                newItem->setText(it.fileName());
                listWidget->addItem(newItem);
            }
        }
    }
}

void Bucket::dropEvent(QDropEvent *event) {

    const QMimeData *mimeData = event->mimeData();

    if(mimeData->hasColor())
        qDebug() << "MimeData has color";

    if(mimeData->hasHtml())
        qDebug() << "MimeData has html";

    if(mimeData->hasText())
        qDebug() << "MimeData has text";

    if(mimeData->hasUrls())
        qDebug() << "MimeData has URLs";

    qDebug() << "Formats : " << mimeData->formats();

    //Add file from folder
    if(!mimeData->hasHtml() && mimeData->hasText()) {
        QStringList stringList = mimeData->text().split("/");
        QFile::copy(mimeData->text(), this->workingDirectory.absolutePath() + "/" + stringList.last());
        qDebug() << "Copying " << mimeData->text() << " in " << workingDirectory.absolutePath() << "/" << stringList.last();
    }

    else if(IsImage(mimeData->text())) {
       // emit ReceivedSmth("ImageNode.png");

//        QFileInfo check_file(QDir::currentPath() + "\\" + manager->saveFileName(QUrl(mimeData->text())));
//            // check if file exists and if yes: Is it really a file and no directory?
//            if (check_file.exists() && check_file.isFile()) {
//                qDebug("Oui");
//            }
            emit IWantAFile(QUrl(mimeData->text()), workingDirectory.absolutePath());
        if(mimeData->text().endsWith(".gif")) {

            // On crée une vidéo. Ici, c'est un GIF
  //          QMovie movie(QDir::currentPath() + "\\" + manager->saveFileName(QUrl(mimeData->text())));
            // On l'associe à un label
            //ui->label->setMovie (&movie);
            // On lance la vidéo
     //       movie.start ();
        }
    }
    else {
      //  emit ReceivedSmth("TextNode.png");
        //qLabel->append(mimeData->text());
        addText(mimeData->text());
      }


//    if (mimeData->hasImage()) {
//        setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
//    } else if (mimeData->hasHtml()) {
//        setText(mimeData->html());
//        setTextFormat(Qt::RichText);
//    } else if (mimeData->hasText()) {
//        setText(mimeData->text());
//        setTextFormat(Qt::PlainText);
//    } else if (mimeData->hasUrls()) {
//        QList<QUrl> urlList = mimeData->urls();
//        QString text;
//        for (int i = 0; i < urlList.size() && i < 32; ++i)
//            text += urlList.at(i).path() + QLatin1Char('\n');
//        setText(text);
//    } else {
//        setText(tr("Cannot display data"));
//    }
}

void Bucket::listWidgetItemDoubleClicked(QListWidgetItem *item)
{
    emit ItemSelectedByUser(item);
//   this->setVisible(false);
}

void Bucket::on_openExplorerButton_clicked()
{
    qDebug() << "Opening explorer .. " <<workingDirectory.absolutePath() ;
    QString program = "explorer";
       QStringList arguments;
       QString tempPath = workingDirectory.absolutePath().replace("/", "\\");
       arguments <<tempPath;// workingDirectory.absolutePath() + "/";

       QProcess *myProcess = new QProcess();
       myProcess->start(program, arguments);

//    QProcess process;
//    process.setWorkingDirectory(workingDirectory.absolutePath());
//    process.start("exporer");//, QStringList() << "gui");
    QProcess::startDetached("git", QStringList() << "gui", "D:\\MyWork\\Temp\\source");
}
