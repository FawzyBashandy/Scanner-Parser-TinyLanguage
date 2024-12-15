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

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void browseFile();
    void parseFile();

private:
    QLineEdit *filePathLineEdit;
    QPushButton *browseButton;
    QPushButton *parseButton;
    QGraphicsView *graphicsView;
    QGraphicsScene *scene;

    Parser *parser; // Instance of your Parser class
    int calculateSubtreeWidth(Node* node);
    void calculatePositions(Node* node, qreal x, qreal y, std::map<Node*, QPointF>& positions);
    void drawSyntaxTree(Node* root, std::map<Node*, QPointF>& positions);
    void drawNode(Node *node, qreal x, qreal y);
    void drawEdge(qreal x1, qreal y1, qreal x2, qreal y2);
};

#endif // MAINWINDOW_H
