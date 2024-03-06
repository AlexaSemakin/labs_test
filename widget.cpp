#include "widget.h"

#include <QDebug>

BinaryTreeWidget::BinaryTreeWidget()
{
    inputLineEdit = new QLineEdit(this);
    buildButton = new QPushButton("Построить дерево", this);
    inputOneSpinBox = new QSpinBox(this);
    inputOneSpinBox->setMaximum(1000000);
    inputOneSpinBox->setMinimum(-inputOneSpinBox->maximum());
    addOne = new QPushButton("Добавить ноду", this);

    removeOneSpinBox = new QSpinBox(this);
    removeOneSpinBox->setMaximum(1000000);
    removeOneSpinBox->setMinimum(-removeOneSpinBox->maximum());
    removeOne = new QPushButton("Удалить ноду", this);

    connect(buildButton, &QPushButton::clicked, this, &BinaryTreeWidget::buildTree);

    connect(addOne, &QPushButton::clicked, this, [this]() {
        auto value = inputOneSpinBox->value();
        numbers.push_back(value);
        insertNode(root, value);
        displayTree(root, scene, 300, 60, 60);
    });

    connect(removeOne, &QPushButton::clicked, this, [this]() {
        auto value = removeOneSpinBox->value();
        auto it = std::find(numbers.begin(), numbers.end(), value);
        if (it != numbers.end()) {
            numbers.erase(it);
            removeOne->setText("Удалить ноду");
            // removeNode(root, value);
            QList<QGraphicsItem *> itemList = scene->items();
            for (auto item : itemList) {
                scene->removeItem(item);
                delete item;
            }

            scene->clear();
            root = nullptr;

            for (int val : numbers) {
                insertNode(root, val);
            }

            displayTree(root, scene, 300, 60, 60);
        } else {
            removeOne->setText("Такой ноды нет");
        }
    });

    layout = new QVBoxLayout(this);
    subLayout = new QHBoxLayout();
    layout->addWidget(inputLineEdit);
    layout->addWidget(buildButton);
    layout->addLayout(subLayout);
    subLayout->addWidget(inputOneSpinBox);
    subLayout->addWidget(addOne);
    subLayout->addWidget(removeOneSpinBox);
    subLayout->addWidget(removeOne);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    layout->addWidget(view);
}

TreeNode *findMinNode(TreeNode *node)
{
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

void BinaryTreeWidget::removeNode(TreeNode *&root, int value)
{
    if (root == nullptr)
        return;

    if (value < root->data) {
        removeNode(root->left, value);
    } else if (value > root->data) {
        removeNode(root->right, value);
    } else {
        if (root->left == nullptr) {
            TreeNode *temp = root->right;
            delete root;
            root = temp;
        } else if (root->right == nullptr) {
            TreeNode *temp = root->left;
            delete root;
            root = temp;
        } else {
            TreeNode *minNode = findMinNode(root->right);

            root->data = minNode->data;

            removeNode(root->right, minNode->data);
        }
    }
}

void BinaryTreeWidget::buildTree()
{
    QString inputString = inputLineEdit->text();
    QStringList stringList = inputString.split(" ", QString::SkipEmptyParts);

    numbers.clear();
    for (const QString &str : stringList) {
        numbers.push_back(str.toInt());
    }

    QList<QGraphicsItem *> itemList = scene->items();
    for (auto item : itemList) {
        scene->removeItem(item);
        delete item;
    }

    scene->clear();
    root = nullptr;

    for (int value : numbers) {
        insertNode(root, value);
    }

    displayTree(root, scene, 300, 60, 800);
}

void BinaryTreeWidget::insertNode(TreeNode *&root, int value)
{
    if (root == nullptr) {
        root = new TreeNode(value);
    } else {
        if (rand() % 2) {
            insertNode(root->left, value);
        } else {
            insertNode(root->right, value);
        }
    }
}

void BinaryTreeWidget::displayTree(
    TreeNode *root, QGraphicsScene *scene, qreal x, qreal y, qreal xOffset)
{
    if (root != nullptr) {
        scene->addEllipse(x, y, 30, 30);
        QGraphicsTextItem *textItem = scene->addText(QString::number(root->data));

        textItem->setPos(x + 5, y + 5);

        if (root->left != nullptr) {
            qreal leftX = x - xOffset;
            qreal leftY = y + 60;
            scene->addLine(x + 15, y + 30, leftX + 15, leftY, QPen(Qt::black));
            displayTree(root->left, scene, leftX, leftY, xOffset / 2);
        }

        if (root->right != nullptr) {
            qreal rightX = x + xOffset;
            qreal rightY = y + 60;
            scene->addLine(x + 15, y + 30, rightX + 15, rightY, QPen(Qt::black));
            displayTree(root->right, scene, rightX, rightY, xOffset / 2);
        }
    }
}
