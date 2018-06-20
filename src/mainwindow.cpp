#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nodewidget.h"

#include <QTimer>
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	InitializeWindow();

    // ---- Shortcut
    QxtGlobalShortcut *launchShortcut = new QxtGlobalShortcut(this);
    launchShortcut->setShortcut(QKeySequence("F2"));
    QObject::connect(launchShortcut, SIGNAL(activated()), this, SLOT(ToggleNodeShortcut()));

    QxtGlobalShortcut *launchShortcut2 = new QxtGlobalShortcut(this);
    launchShortcut2->setShortcut(QKeySequence("F1"));
    QObject::connect(launchShortcut2, SIGNAL(activated()), this, SLOT(ToggleUI()));

    // ---- Download Manager
    manager = new DownloadManager(this);

    manager->moveToThread(&workerThread);
    //connect(&workerThread, &QThread::finished, manager,SLOT(WarnNodesFileDownloaded));
    connect(manager, SIGNAL(finished()), this ,SLOT(WarnNodesFileDownloaded()));
    workerThread.start();

    // ---- Nodes
    Node *trash = new Node("Trash");
    NodeSettings settings2;
    settings2.nodeType = NodeType::Trash;
    settings2.radius = 70;
    AddNode(trash, settings2);



	Node *node1 = new Node("Node1");
    NodeSettings settings;
    settings.nodeType = NodeType::Clip;
    settings.radius = 70;
    AddNode(node1, settings);

    HideTrash();

    backgroundAlpha = 0;
    SetDay();
    trashWidget->setVisible(true);


//    QTimer *timer = new QTimer(this);
//       connect(timer, SIGNAL(timeout()), this, SLOT(Test()));
//       timer->start(2000);
}

MainWindow::~MainWindow()
{
    for(int i=0 ; i<_nodes.size() ; i++)
        _nodes[i]->deleteLater();
    workerThread.quit();
    //workerThread.wait();
    delete ui;
}


void MainWindow::DownloadFile(QUrl url, QString path) {
    qDebug() << "File requested : " << url << " Destination : " << path;
    manager->append(url, path);
}

void MainWindow::CreatePin(NodeWidget* node, QListWidgetItem* item) {
    Node* node3 = new Node("Node" + _nodes.size());
    NodeSettings settings;
    settings.nodeType = NodeType::Pin;
    settings.radius = 60;
    settings.path = node->bucket->workingDirectory.absolutePath() + "/" + item->text();

    AddNode(node3, settings);
}

void MainWindow::InitializeWindow()
{
	//QWidget::activateWindow();
    this->setAttribute(Qt::WA_TranslucentBackground);
    Qt::WindowFlags flags = this->windowFlags();
	flags |= Qt::FramelessWindowHint;
	flags |= Qt::X11BypassWindowManagerHint;
    flags |= Qt::WindowStaysOnTopHint;
    this->setWindowFlags(flags | Qt::WindowStaysOnTopHint);
	
	QRect rec = QApplication::desktop()->screenGeometry();
    height = rec.height();
    width = rec.width();
	this->resize(width, height);

    SetDay();


}

void MainWindow::ToggleNodeShortcut() {
   // qDebug() << "launch shortcut activated";
    if(UIVisible)
        emit launchShortcutPressed();
}

void MainWindow::ToggleUI() {
 //   qDebug() << "UI shortcut activated";
    for(int i=0 ; i<_nodes.size() ; i++) {
        _nodes[i]->setVisible(!_nodes[i]->isVisible());
    }
    this->setVisible(!this->isVisible());
    UIVisible = this->isVisible();
}

void MainWindow::ClipboardShortcutActivated() {
    qDebug() << "clipboard shortcut activated";
    emit clipboardShortcutPressed();
}

void MainWindow::AddNode(Node* newNode, NodeSettings settings)
{
    qDebug() << "Adding node : " << settings.nodeType;
    NodeWidget* nodeWidget = new NodeWidget(newNode, this, settings.radius, settings);
    QObject::connect(this, SIGNAL(launchShortcutPressed()),
                         nodeWidget, SLOT(ToggleBucket()));

    QObject::connect(this, SIGNAL(clipboardShortcutPressed()),
                         nodeWidget, SLOT(CopyClipboard()));
    QObject::connect(nodeWidget, SIGNAL(BucketItemSelected(NodeWidget*, QListWidgetItem*)), this, SLOT(CreatePin(NodeWidget*, QListWidgetItem*)));
    QObject::connect(nodeWidget, SIGNAL(FileNeeded(QUrl,QString)), this, SLOT(DownloadFile(QUrl,QString)));
    QObject::connect(nodeWidget, SIGNAL(NodeMoved(NodeWidget*)), this, SLOT(NodeMoved(NodeWidget*)));
    QObject::connect(nodeWidget, SIGNAL(NodeMoving(NodeWidget*)), this, SLOT(NodeMoving(NodeWidget*)));
    QObject::connect(manager, SIGNAL(finished()), nodeWidget, SLOT(HandleNewFile()));

    nodeWidget->setVisible(true); //LOLOLOLOL
    _nodes.append(nodeWidget);

    if(settings.nodeType == NodeType::Trash) {
        //Compute bottom center
        int x = width / 2.1;
        int y = height * 1.1;
        QPoint position(x, y);
        nodeWidget->move(position);
        trashWidget = nodeWidget;
        trashWidget->setVisible(false);
    }
    else {
        nodeWidget->MoveTo(QCursor::pos());
        nodeWidget->DockNode();
    }
    trashWidget->raise();
    qDebug() << "Nb node : " << _nodes.count();

    SetDay();
    HideTrash();
    qDebug("Done");
}

void MainWindow::RemoveNode(QString name) {
   // _nodes.remove(name);
}

void MainWindow::WarnNodesFileDownloaded() {
    for(int i=0 ; i<_nodes.size() ; i++) {
        _nodes[i]->HandleNewFile();
    }
}
void MainWindow::NodeMoving(NodeWidget* node) {
    if(!trashVisible) {
        ShowTrash();
        qDebug() << "Node moving ...";
        SetNight();
    }
}

void MainWindow::NodeMoved(NodeWidget* node) {
    HideTrash();
    SetDay();
    qDebug() << "Node moved !";

    bool destroyed = false;
    int i=0;
    for(i=0 ; i<_nodes.size() ; i++) {
        if(_nodes[i]->settings.nodeType == NodeType::Trash) {
            QPoint distance = node->pos() - _nodes[i]->pos();
            if (distance.manhattanLength() < 60) {
                node->deleteLater();
                destroyed = true;
                break;
            }
        }
    }

    if(destroyed) {
        _nodes.removeAt(_nodes.indexOf(node));
        if(_nodes.size() <= 1) {
            this->deleteLater();
            exit(0);
        }
    }
}

void MainWindow::ShowTrash() {
//qDebug() << "Showing trash ...";
    int x = width / 2.1;
    int y = height * 0.875;
//qDebug() << "From " << QPoint(trashWidget->pos().x(), trashWidget->pos().y()) << "to " << QPoint(x,y);

    QPropertyAnimation *animation = new QPropertyAnimation(trashWidget, "pos");
    animation->setDuration(300);
    animation->setStartValue(QPoint(trashWidget->pos().x(), trashWidget->pos().y()));
    animation->setEndValue(QPoint(x, y));
    animation->setEasingCurve(QEasingCurve::OutExpo);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    trashVisible = true;
}

void MainWindow::HideTrash() {
//qDebug() << "Hiding trash ...";
    int x = width / 2.1;
    int y = height * 1.1;
//qDebug() << "From " << QPoint(trashWidget->pos().x(), trashWidget->pos().y()) << "to " << QPoint(x,y);
    QPropertyAnimation *animation = new QPropertyAnimation(trashWidget, "pos");
    animation->setDuration(300);
    animation->setStartValue(QPoint(trashWidget->pos().x(), trashWidget->pos().y()));
    animation->setEndValue(QPoint(x, y));
    animation->setEasingCurve(QEasingCurve::InExpo);
    //connect(animation, SIGNAL(finished()), this, SLOT(MoveToNode()));

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    trashVisible = false;
}

void MainWindow::SetNight() {
    QPropertyAnimation *dayNightAnimation = new QPropertyAnimation(this, "backgroundAlpha");
    dayNightAnimation->setDuration(250);
    dayNightAnimation->setStartValue(backgroundAlpha);
    dayNightAnimation->setEndValue(75);
    dayNightAnimation->setDuration(500);
    dayNightAnimation->setEasingCurve(QEasingCurve::OutExpo);
    dayNightAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    qDebug() << "Set Night";
}

void MainWindow::SetDay() {
    QPropertyAnimation *animation = new QPropertyAnimation(this, "backgroundAlpha");
    animation->setDuration(250);
    animation->setStartValue(backgroundAlpha);
    animation->setEndValue(0);
    animation->setEasingCurve(QEasingCurve::InExpo);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    qDebug() << "Set Day";
}


void MainWindow::paintEvent(QPaintEvent *event) {
    QColor backgroundColor = QColor(0,0,0,0);
    backgroundColor.setAlpha(backgroundAlpha);
    QPainter customPainter(this);
    customPainter.fillRect(rect(),backgroundColor);
//    QColor backgroundColor = palette().light().color();
//     backgroundColor.setAlpha(backgroundAlpha);
//     QPainter customPainter(this);
//     customPainter.fillRect(rect(),backgroundColor);
//    QWidget::paintEvent(event);
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.setPen(QPen(Qt::blue, 3));
//    auto i = 0;
//    for each (NodeWidget* node in _nodes)
//	{
//		auto childs = node->node->GetAllChilds();
//        for each (Node* child in childs)
//		{
//			auto widget = _nodes[child->GetName()];
//			auto pos1 = node->rect();
//			auto pos2 = node->pos();
//			auto pos3 = node->mapToGlobal(node->pos());
//			//auto pos4 = node->mapToGlobal(node->rect()->center());
//			//painter.drawLine(node->mapToGlobal(node->pos()), widget->mapToGlobal(widget->pos()));
//			//painter.drawLine(node->mapToGlobal(node->rect().center()), widget->rect().center());
//			painter.drawLine(node->Center(), widget->Center());

//		  //  painter.drawLine(QPoint(50*i, 50), QPoint(500, 500));
//			i++;
//		}
//	}
    //painter.drawEllipse(40,40,65,90);
 //   painter.end();
}



void MainWindow::enterEvent(QEvent* e) {
    QWidget::enterEvent(e);
}

void MainWindow::leaveEvent(QEvent* e) {
    QWidget::enterEvent(e);
}


void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
    }

    event->accept();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    //  if (event->mimeData()->hasFormat("text/plain"))
    event->acceptProposedAction();

}

void MainWindow::dropEvent(QDropEvent *event) {
    qDebug("MainWindow received a drop event !");
}

