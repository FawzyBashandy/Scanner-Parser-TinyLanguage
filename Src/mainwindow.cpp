#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QMessageBox>
#include "Parser.h"
#include "qlistwidget.h"
#include <QGuiApplication>
#include <QScreen>
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void browseFile();
    void parseFile();
    void scanTokens();
private:
    QLineEdit *filePathLineEdit;
    QPushButton *browseButton;
    QPushButton *parseButton;
    QPushButton *scanButton;
    QGraphicsView *graphicsView;
    QGraphicsScene *scene;
    QListWidget *tokenList;  // Displaying tokens
    QString parseText();  // Function to handle text parsing
    QStringList tokenize(const std::vector<Token> &tokens);
    Parser *parser; // Instance of your Parser class
    Scanner * SCanner;
    void calculatePositions(Node* root, qreal x, qreal y,
                            map<Node*, QPointF>& positions);
    qreal findHighestXAtY(const map<Node*, QPointF>& positions, qreal specificY);
    void drawSyntaxTree(Node* root, map<Node*, QPointF>& positions);
    void drawNode(Node *node, qreal x, qreal y);
    void drawEdge(qreal x1, qreal y1, qreal x2, qreal y2);
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), parser(nullptr) {

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // First Layer: File layout
    QHBoxLayout *fileLayout = new QHBoxLayout();
    filePathLineEdit = new QLineEdit(this);
    browseButton = new QPushButton("Browse", this);
    scanButton = new QPushButton("Scan", this);
    fileLayout->addWidget(filePathLineEdit);
    fileLayout->addWidget(browseButton);
    fileLayout->addWidget(scanButton);
    mainLayout->addLayout(fileLayout);

    // Second Layer: Parse Button
    parseButton = new QPushButton("Parse", this);
    mainLayout->addWidget(parseButton);

    // Third Layer: Tokens list and GraphicsView side by side
    QHBoxLayout *thirdLayerLayout = new QHBoxLayout();
    tokenList = new QListWidget(this);
    graphicsView = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    graphicsView->setScene(scene);
    thirdLayerLayout->addWidget(tokenList);
    thirdLayerLayout->addWidget(graphicsView);
    tokenList->setFixedWidth(200);
    QFont font = tokenList->font();
    font.setPointSize(14);
    font.bold();
    tokenList->setFont(font);
    mainLayout->addLayout(thirdLayerLayout);

    setCentralWidget(centralWidget);
    QRect availableGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    resize(availableGeometry.width() - 5, availableGeometry.height() - 30);

    connect(browseButton, &QPushButton::clicked, this, &MainWindow::browseFile);
    connect(parseButton, &QPushButton::clicked, this, &MainWindow::parseFile);
    connect(scanButton, &QPushButton::clicked, this, &MainWindow::scanTokens);
}


MainWindow::~MainWindow() {
    delete parser;
}
void MainWindow::scanTokens() {
    QString filePath = filePathLineEdit->text();
    if (filePath.isEmpty()) {
        scene->clear();
        tokenList->clear();
        QString errorMessage = QString("ERROR : Enter a non-empty filepath");
        QMessageBox::critical(nullptr, "Empty FilePath ERROR", errorMessage);
        return;
    }

    try
    {
        SCanner = new Scanner();
        SCanner->scanFile(filePath.toStdString());
    }
    catch (const ios_base::failure& e) {
        QString errorMessage = QString(e.what());
        delete SCanner;
        SCanner = nullptr; // Set parser to nullptr
        scene->clear();
        tokenList->clear();
        QMessageBox::critical(nullptr, "Aborting :: File Error", errorMessage);
        return;
    }
    catch(const exception& e)
    {
        QString errorMessage = QString("Error during scanning -> ").append(e.what());
        delete SCanner;
        scene->clear();
        tokenList->clear();
        QMessageBox::critical(nullptr, "Aborting :: Scanner Error", errorMessage);
        return;
    }
    QStringList tokens = tokenize(SCanner->tokens);
    if (tokens.isEmpty()) {
        tokenList->addItem("No tokens found.");
    } else {
        tokenList->clear();  // Clear previous tokens
        tokenList->addItems(tokens);  // Display scanned tokens
    }
    delete SCanner;
    SCanner = nullptr;
}
QStringList MainWindow::tokenize(const std::vector<Token> &tokens) {
    QStringList tokenStrings;

    for (const Token &token : tokens) {
        tokenStrings.append(QString("%1: %2").arg(QString::fromStdString(token.type)).arg(QString::fromStdString(token.value)));
    }

    return tokenStrings;
}
void MainWindow::browseFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt);;All Files (*)");
    if (!fileName.isEmpty()) {
        filePathLineEdit->setText(fileName);
    }

    if (parser) {
        delete parser;
        parser = nullptr;

    }
    if(SCanner)
    {
        SCanner = nullptr;
    }

    // Clear the previous drawing
    tokenList->clear();
    scene->clear();
}

void MainWindow::parseFile() {
    QString filePath = filePathLineEdit->text();
    if (filePath.isEmpty()) {
        scene->clear();
        tokenList->clear();
        QString errorMessage = QString("ERROR : Enter a non-empty filepath");
        QMessageBox::critical(nullptr, "Empty FilePath ERROR", errorMessage);
        return;
    }

    try
    {
        Scanner scanner;
        scanner.scanFile(filePath.toStdString());
        parser = new Parser(scanner);
    }
    catch (const ios_base::failure& e) {
        QString errorMessage = QString(e.what());
        delete parser;
        parser = nullptr; // Set parser to nullptr
        scene->clear();
        tokenList->clear();
        QMessageBox::critical(nullptr, "Aborting :: File Error", errorMessage);
        return;
    }
    catch(const exception& e)
    {
        QString errorMessage = QString("Error during scanning -> ").append(e.what());
        delete parser;
        scene->clear();
        tokenList->clear();
        parser = nullptr; // Set parser to nullptr
        QMessageBox::critical(nullptr, "Aborting :: Scanner Error", errorMessage);
        return;
    }


    try {
        Node* root = parser->Parse();
        scene->clear();               // Clear previous drawings
        map<Node*, QPointF> positions;
        calculatePositions(root, 0, 800, positions);
        drawSyntaxTree(root, positions);
        qDebug() << "Parsing completed successfully.";
        delete parser;
        parser = nullptr;
    } catch (const exception& e) {
        QString errorMessage = QString("Error during parsing: ").append(e.what());

        delete parser;
        parser = nullptr;
        QMessageBox::critical(nullptr, "Aborting :: Parser Error", errorMessage);
    }
}

void MainWindow::calculatePositions(Node* root, qreal x, qreal y,
                                    map<Node*, QPointF>& positions) {
    if (!root) return;

    // Only set the position if it has not been set before
    if (positions.find(root) == positions.end()) {
        positions[root] = QPointF(x, y);
    }

    // Save the original x for horizontal adjustments
    qreal originalX = x;

    // Increase the y level for all children once
    qreal childYOffset = 100;
    qreal childY = y + childYOffset;

    Node* CurrentNode = root->getLeftChild();
    qreal leftChildXOffset = -50; // Horizontal offset for the left child
    if (CurrentNode) {
        // Position the left child
        qreal leftChildX = originalX + leftChildXOffset;
        if (positions.find(CurrentNode) == positions.end())
        {
            positions[CurrentNode] = QPointF(leftChildX, childY);
        }
        calculatePositions(CurrentNode, leftChildX, childY, positions); // Recurse for the left child
    }

    // Handle the right child (if the node has no left child,right child should be drawn perpendicular)
    CurrentNode = root->getRightChild();
    if (CurrentNode) {
        // If there's no left child and there's a right child, draw right child directly below the current node
        if (!root->getLeftChild()) {
            if (positions.find(CurrentNode) == positions.end())
            {
            positions[CurrentNode] = QPointF(originalX, childY);
            }
        } else {
            // Position the right child normally
            qreal rightChildXOffset = 100; // Minimum horizontal offset
            qreal rightChildX = findHighestXAtY(positions,-9000) + rightChildXOffset;
            if(rightChildX == numeric_limits<qreal>::lowest()) // it means no there's no nodes in this level yet.
            {
                rightChildX = originalX + rightChildXOffset;
            }
            if (positions.find(CurrentNode) == positions.end())
            {
                positions[CurrentNode] = QPointF(rightChildX, childY);
            }
        }
        calculatePositions(CurrentNode, positions[CurrentNode].x(), positions[CurrentNode].y(), positions); // Recurse for the right child
    }

    // Handle the else child
    CurrentNode = root->getElseChild();
    if (CurrentNode) {
        qreal elseChildXOffset = 130; // Horizontal offset

        qreal elseChildX = findHighestXAtY(positions,-9000) + elseChildXOffset + 50 ; //Find max x at all level so that else never overlap
        if(elseChildX == numeric_limits<qreal>::lowest())
        {
            elseChildX = originalX + elseChildXOffset;
        }
        if (positions.find(CurrentNode) == positions.end())
        {
            childY = childY - 30;
            positions[CurrentNode] = QPointF(elseChildX, childY);
        }
        calculatePositions(CurrentNode, elseChildX, childY, positions); // Recurse for the else child
    }

    // Handle siblings
    CurrentNode = root->getSibling();
    qreal siblingXOffset = 150; // offset for siblings
    while (CurrentNode) {
        qreal x = findHighestXAtY(positions,-9000) + siblingXOffset +10 ;
        if(x == numeric_limits<qreal>::lowest())
        {
            x += siblingXOffset;
        }
        if (positions.find(CurrentNode) == positions.end())
        {
            positions[CurrentNode] = QPointF(x, y);
        }
        calculatePositions(CurrentNode, x, y, positions); // Recurse for siblings
        CurrentNode = CurrentNode->getSibling();
    }
}

void MainWindow::drawSyntaxTree(Node* root, map<Node*, QPointF>& positions) {
    if (!root) return;

    // Draw the current node
    drawNode(root, positions[root].x(), positions[root].y());

    // Draw the edges to children (if any)
    if (root->getLeftChild()) {
        drawEdge(positions[root].x(), positions[root].y(), positions[root->getLeftChild()].x(), positions[root->getLeftChild()].y());
        drawSyntaxTree(root->getLeftChild(), positions);
    }
    if (root->getRightChild()) {
        drawEdge(positions[root].x(), positions[root].y(), positions[root->getRightChild()].x(), positions[root->getRightChild()].y());
        drawSyntaxTree(root->getRightChild(), positions);
    }
    if (root->getElseChild()) {
        drawEdge(positions[root].x(), positions[root].y(), positions[root->getElseChild()].x(), positions[root->getElseChild()].y());
        drawSyntaxTree(root->getElseChild(), positions);
    }

    // Handle siblings
    if (root->getSibling()) {
        drawEdge(positions[root].x(), positions[root].y(), positions[root->getSibling()].x(), positions[root->getSibling()].y());
        drawSyntaxTree(root->getSibling(), positions);
    }
}

void MainWindow::drawNode(Node* node, qreal x, qreal y) {
    if (!node) return;

    if (node->getNodeType() == "ASSIGN" || node->getNodeType() == "IF" ||
        node->getNodeType() == "WRITE" || node->getNodeType() == "READ" ||
        node->getNodeType() == "REPEAT") {
        // Draw rectangle
        QRectF rect(x - 30, y - 15, 60, 30); // x, y is the center, width=60, height=30
        QGraphicsRectItem* rectangle = new QGraphicsRectItem(rect);
        rectangle->setBrush(QBrush(Qt::white)); // Fill color
        rectangle->setPen(QPen(Qt::black));    // Border color
        scene->addItem(rectangle);

        QGraphicsTextItem* text = scene->addText(QString::fromStdString(node->getVal()));
        text->setFont(QFont("Arial", 10));
        text->setDefaultTextColor(Qt::black);

        // Center the text within the rectangle
        qreal textWidth = text->boundingRect().width();
        qreal textHeight = text->boundingRect().height();

        // Expand the rectangle if the text is wider than the current rectangle width
        if (textWidth > rect.width()) {
            qreal extraWidth = textWidth - rect.width();
            rect.adjust(-extraWidth / 2, 0, extraWidth / 2, 0); // Expand rectangle symmetrically
            rectangle->setRect(rect); // Update the rectangle
        }

        // Recalculate the center of the rectangle
        qreal rectCenterX = rect.center().x();
        qreal rectCenterY = rect.center().y();

        // Set the position of the text to be at the center of the rectangle
        text->setPos(rectCenterX - textWidth / 2, rectCenterY - textHeight / 2);
    } else {
        QGraphicsTextItem* text = scene->addText(QString::fromStdString(node->getVal()));
        text->setFont(QFont("Arial", 10));
        text->setDefaultTextColor(Qt::black);

        QRectF textRect = text->boundingRect();
        qreal textWidth = textRect.width();
        qreal textHeight = textRect.height();

        // Calculate the radius of the circle based on the text size (leave some padding)
        qreal radius = qMax(textWidth, textHeight) / 2;

        // Create the circle with the calculated radius
        QGraphicsEllipseItem* circle = scene->addEllipse(x - radius, y - radius, 2 * radius, 2 * radius,
                                                         QPen(Qt::black), QBrush(Qt::white));

        // Set the position of the text to center it inside the circle
        text->setPos(x - textWidth / 2, y - textHeight / 2);

        circle->setZValue(-1);  // Set the circle's Z-value to be behind the text

    }
}

void MainWindow::drawEdge(qreal x1, qreal y1, qreal x2, qreal y2) {
    qreal dx = x2 - x1;
    qreal dy = y2 - y1;
    qreal length = sqrt(dx * dx + dy * dy);
    qreal offsetX = 20.0 * dx / length;
    qreal offsetY = 20.0 * dy / length;

    scene->addLine(x1 + offsetX, y1 + offsetY, x2 - offsetX, y2 - offsetY, QPen(Qt::black));
}

qreal MainWindow::findHighestXAtY(const map<Node*, QPointF>& positions, qreal specificY) {
    qreal max_x = numeric_limits<qreal>::lowest();
    bool overallMax = (specificY == -9000); // Special value check

    for (const auto& pair : positions) {
        if (overallMax || abs(pair.second.y() - specificY) < 0.001) { // Check if we want the overall max or specific y-level
            if (pair.second.x() > max_x) {
                max_x = pair.second.x();
            }
        }
    }

    return max_x;

}
