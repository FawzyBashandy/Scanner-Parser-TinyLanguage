#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include "Parser.h"
#include "qlistwidget.h"

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
    QStringList tokenize(const QString &text);  // Tokenizing method
    Parser *parser; // Instance of your Parser class
    void calculatePositions(Node* root, qreal x, qreal y,
                            map<Node*, QPointF>& positions);
    qreal findHighestXAtY(const map<Node*, QPointF>& positions, qreal specificY);
    void drawSyntaxTree(Node* root, map<Node*, QPointF>& positions);
    void drawNode(Node *node, qreal x, qreal y);
    void drawEdge(qreal x1, qreal y1, qreal x2, qreal y2);
};

#endif // MAINWINDOW_H
