#include "nodewidget.h"

NodeWidget::NodeWidget(Node* node,QWidget *parent, int size, NodeSettings settings)
    : QWidget(parent),
      ui(new Ui::NodeWidget)
{
    qDebug("Initializing nodeWidget ...");
    ui->setupUi(this);
    isMoving = false;

    _size = size;
	this->node = node;
    this->nodeDrawer = parent;
    this->settings = settings;

    if(settings.nodeType == NodeType::Clip)
        SetNodeImage("Pin.png");
    if(settings.nodeType == NodeType::Pin)
        SetNodeImage("Pin-v2.png");
    if(settings.nodeType == NodeType::Trash)
        SetNodeImage("Trash.png");

    if(settings.nodeType == NodeType::Trash) return;

    isBucketOpened = false;
    bucket = new Bucket(settings, this->nodeDrawer);
    bucket->setVisible(false);
    bucket->move(BottomLeft());
    //ToggleBucket();

    QObject::connect(this, SIGNAL(DropOnNode(QDropEvent*)), bucket, SLOT(GetADropEvent(QDropEvent*)));
    QObject::connect(bucket, SIGNAL(ReceivedSmth(QString)), this, SLOT(ChangeNodeImage(QString)));
    QObject::connect(bucket, SIGNAL(ItemSelectedByUser(QListWidgetItem*)), this, SLOT(CreatePin(QListWidgetItem*)));
    QObject::connect(bucket, SIGNAL(IWantAFile(QUrl,QString)), this, SLOT(FileRequested(QUrl,QString)));

    qDebug("Done");
	//m_bouton->setToolTip("Ceci est une info bulle");
}

NodeWidget::~NodeWidget()
{
    bucket->deleteLater();
}

void NodeWidget::HandleNewFile() {
    if(settings.nodeType == NodeType::Clip)
        bucket->UpdateVisibleFiles();
}

void NodeWidget::CreatePin(QListWidgetItem *item) {
    emit BucketItemSelected(this, item);
}

void NodeWidget::FileRequested(QUrl url, QString pathToSave) {
    emit FileNeeded(url, pathToSave);
}

void NodeWidget::ChangeNodeImage(QString str) {
    SetNodeImage(str);
}

void NodeWidget::SetNodeImage(QString fileName) {
    qDebug() << "Setting image " << fileName;

    if(fileName == actualImage) return;

    //Get new image
    QPixmap mypix (":/node/main/Ressources/" + fileName);
    mypix = mypix.scaledToWidth(_size,Qt::SmoothTransformation);
    ui->label->setPixmap(mypix);
    ui->label->adjustSize();
    this->resize(this->size());

    //Open animation
//    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
//    animation->setDuration(250);
//    animation->setStartValue(QRect(this->pos().x(), this->pos().y(), this->size().width(), this->size().height()));

   // ui->label->move(this->pos());
    this->adjustSize();

    ui->label->setAcceptDrops(true);
    actualImage = fileName;
    qDebug("Set.");
}

void NodeWidget::InitializeWindow()
{
    Qt::WindowFlags flags2;
    flags2 |= Qt::FramelessWindowHint;
    //flags2 |= Qt::X11BypassWindowManagerHint;
    flags2 |= Qt::WindowStaysOnTopHint;

    //     //flags |= Qt::WA_TranslucentBackground;
    this->setWindowFlags(flags2);
    this->setAttribute(Qt::WA_TranslucentBackground);
    //this;setStyleSheet(QString("background-color: rgba(255,0,0,255);");
}

void NodeWidget::InitializeName() {
    //Content
    nodeName = new QLabel(node->GetName(), this);
    nodeName->setFont(QFont("Helvetica [Cronyx]", 11));

//    //Appearance
//    QPalette sample_palette;
//    sample_palette.setColor(QPalette::Window, Qt::transparent);
//    sample_palette.setColor(QPalette::WindowText, Qt::blue);

//    nodeName->setAutoFillBackground(true);
//    nodeName->setPalette(sample_palette);
    nodeName->adjustSize();
    //Position
    auto namePosition = Center();
    namePosition.setX(namePosition.x() - (nodeName->size().width()/2));
    namePosition.setY(namePosition.y() - (nodeName->size().height()/2));

    //Doesn't work for unknown reason
//    qDebug("Name position before : " + 1);//nodeName->pos().x());
//    nodeName->move(namePosition);
//    qDebug("Name position after : " + nodeName->pos().x());
    //nodeName->move(Center());
}

void NodeWidget::CopyClipboard() {
//    QClipboard *clipboard = QApplication::clipboard();
//    bucket->addText(clipboard->text());
//    clipboard->clear();
}

QPoint NodeWidget::BottomLeft() {
    QPoint botCenter;
    botCenter.setX(this->pos().x());//+(this->size().width()/2));
    botCenter.setY(this->pos().y()+(this->size().height()));
    return botCenter;
}

void NodeWidget::DockingCompleted() {
    bucket->MoveTo(BottomLeft());
}


void NodeWidget::OpenBucket(bool open)
{
    if(settings.nodeType == NodeType::Trash) return;
    if (open) {
        bucket->Open();
        isBucketOpened = true;
        qDebug() << "Open";
    }
    else {
        bucket->Close();
        isBucketOpened = false;
        qDebug() << "Close";
    }
}

void NodeWidget::ToggleBucket()
{
    if(settings.nodeType == NodeType::Trash) return;
    if (!bucket->isClosed) {
        bucket->Close();
        isBucketOpened = false;
        qDebug() << "Close";
    }
    else {
        bucket->Open();
        isBucketOpened = true;
        qDebug() << "Open";
    }
}

void NodeWidget::DockNode() {
    if(settings.nodeType == NodeType::Trash) return;

    if(!bucket->isClosed)
        bucket->Close();

    //Dock node
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(250);
    animation->setStartValue(QRect(this->pos().x(), this->pos().y(), this->size().width(), this->size().height()));

    auto nodeDrawer = ((QWidget*)this->parent());
    auto nodeDrawerXAndWidth = nodeDrawer->pos().x() + nodeDrawer->size().width();
    animation->setEndValue(QRect(nodeDrawerXAndWidth-this->size().width(), this->pos().y(), this->size().width(), this->size().height()));
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(DockingCompleted()));

    animation->setEasingCurve(QEasingCurve::OutExpo);
    animation->start();
}

void NodeWidget::MoveTo(int x, int y)
{
    QPoint target(x, y);
    MoveTo(target);
}

//Limited by parent border
void NodeWidget::MoveTo(QPoint target){

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
    if(bucketNewYAndHeight > nodeDrawerYAndHeight - bucket->size().height())
        newPos.setY(nodeDrawerHeight - bucketHeight - bucket->size().height());

    //Check top
    if(target.y() < nodeDrawer->pos().y())
        newPos.setY(nodeDrawer->pos().y());


    this->move(newPos);
    bucket->MoveTo(BottomLeft());

    emit NodeMoving(this);
}
QString NodeWidget::GetName()
{
	return nodeName->text();
}

void NodeWidget::paintEvent(QPaintEvent *event)
{	
    QColor backgroundColor = QColor(0,0,0);//palette().dark().color();
     backgroundColor.setAlpha(0);
     QPainter customPainter(this);
     customPainter.fillRect(rect(),backgroundColor);
    this->show();
//	QRadialGradient brush(rect().center().x(), rect().center().y(), _radius);
//	brush.setColorAt(0.0, _color);
//	//brush.setColorAt(0.3, Qt::white);
//	brush.setColorAt(1.0, _color);

	/*p.begin(this);
	b.setCenter(rect().center().x(), rect().center().y());
	b.setRadius(_radius);
	b.setColorAt(0.0, Qt::white);
	b.setColorAt(0.2, Qt::green);
	b.setColorAt(1.0, Qt::black);
	p.setBrush(b);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setPen(QPen(Qt::green, 0));
	p.drawEllipse(rect());
	p.end();*/

//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.setPen(QPen(_color, 0));
//    painter.setBrush(brush);

//    painter.drawEllipse(rect().center(), _radius, _radius);
//    painter.end();

//    parentWidget()->update();


	//Update bucket position
    //auto newPositionX = this->pos().x() + this->frameGeometry().width();
    //auto newPositionY = this->pos().y() - node->bucket->frameGeometry().height() / 2;
    //auto newPositionPoint = new QPoint(newPositionX, newPositionY);

    //auto nodeDrawerRect = parentWidget()->frameGeometry();

	//Black magic
    //auto a = float(nodeDrawerRect.height() - float(node->bucket->height())) / float(nodeDrawerRect.height());
    //node->bucket->move((this->pos().x() - node->bucket->width()), this->pos().y()*a);

	//node->bucket->move(*newPositionPoint);

}

void NodeWidget::enterEvent(QEvent* e) {
	QWidget::enterEvent(e);

	//_radius += _radius*int(50/100);

	/*QPropertyAnimation animation(this, "geometry");
	animation.setDuration(500);
	animation.setStartValue(this->rect());
	auto rect = this->rect();
	rect.setX(rect.x() - value);
	rect.setY(rect.y() - value);
	rect.setWidth(rect.width() + value);
	rect.setHeight(rect.height() + value);

	animation.setEndValue(rect);

	animation.start();*/
}

void NodeWidget::leaveEvent(QEvent* e) {
	//QWidget::enterEvent(e);

	//_radius -= _radius*int(50 / 100);
}

void NodeWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton)
        ToggleBucket();
}

void NodeWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
		dragPosition = event->globalPos() - frameGeometry().topLeft();
        displacementLength = displacementStartTime = QTime::currentTime().msec();
        displacementLength = 0;//dragPosition.manhattanLength();
	}

    if (event->button() == Qt::RightButton) {
        DockNode();
	}
	event->accept();
}

void NodeWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(settings.nodeType == NodeType::Trash) return;

    if (event->buttons() & Qt::LeftButton) {
        if(!isMoving) {
            bucketWasOpen = isBucketOpened;
            if(isBucketOpened)
                OpenBucket(false);
        }
		QPoint newPos = event->globalPos() - dragPosition;
        displacementLength += (newPos - pos()).manhattanLength();
		QPoint differencePosition = newPos - pos(); //Then all his child with this node's delta
        MoveTo(newPos.x(), newPos.y());//Firstly we move the node
        isMoving = true;
	}
    event->accept();
}

void NodeWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if(isMoving) {
            if(bucketWasOpen)
                OpenBucket(true);
        }
        isMoving = false;

        //Check if velocity was enough to dock node
       // int totalDisplacementLength = (displacementStartPosition - event->globalPos()).manhattanLength();
        float totalDisplacementTime = QTime::currentTime().msec() - displacementStartTime;
        float velocity = displacementLength / totalDisplacementTime;
        //qDebug() << "velocity : " << velocity << " | length : " << displacementLength;
        if(qFabs(velocity) > dockTiggerValue && displacementLength < 200)
            DockNode();

        emit NodeMoved(this);
    }
}

void NodeWidget::dragEnterEvent(QDragEnterEvent *event)
{
	//  if (event->mimeData()->hasFormat("text/plain"))

	event->acceptProposedAction();
	
}

void NodeWidget::dropEvent(QDropEvent *event) {
    if(settings.nodeType == NodeType::Clip)
        emit DropOnNode(event);
}

QPoint NodeWidget::Center() {
	QPoint center;
	auto x = this->pos().x();
	auto y = this->pos().y();
    auto width = this->size().width();
    auto height = this->size().height();
	center.setX(x + width/2);
	center.setY(y + height/2);
	return center;
}
