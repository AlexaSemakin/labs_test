#ifndef WIDGET_H
#define WIDGET_H

#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <vector>
#include <iostream>

struct TreeNode
{
    int data;
    TreeNode *left;
    TreeNode *right;

    TreeNode(int value) : data(value), left(nullptr), right(nullptr)
    {}
};

class BinaryTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BinaryTreeWidget();

private slots:
    void buildTree();

private:
    QLineEdit *inputLineEdit;
    QPushButton *buildButton;
    QSpinBox *inputOneSpinBox;
    QSpinBox *removeOneSpinBox;
    QPushButton *addOne;
    QPushButton *removeOne;
    QVBoxLayout *layout;
    QHBoxLayout *subLayout;

    QGraphicsScene *scene;
    QGraphicsView *view;
    TreeNode *root { nullptr };
    std::vector<int> numbers;

    void insertNode(TreeNode *&root, int value);
    void displayTree(TreeNode *root, QGraphicsScene *scene, qreal x, qreal y, qreal xOffset);
    void removeNode(TreeNode *&root, int value);
};

#endif // WIDGET_H
