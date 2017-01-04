/*
Name: QtRpt
Version: 2.0.0
Web-site: http://www.qtrpt.tk
Programmer: Aleksey Osipov
E-mail: aliks-os@ukr.net
Web-site: http://www.aliks-os.tk

Copyright 2012-2016 Aleksey Osipov

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "GraphicsScene.h"
#include <QApplication>
#include <QSettings>
#include "mainwindow.h"
#include<QDebug>

GraphicsScene::GraphicsScene(QObject *parent) : QGraphicsScene(parent) {
    sceneMode = SelectObject;
    m_trackingMoves = false;

    //m_backgroundItem = 0;
    m_undoStack = new QUndoStack(this);
}

//void GraphicsScene::setBackground(QString path) {
//    if (m_backgroundItem != 0 ) {
//        this->removeItem(m_backgroundItem);
//        m_backgroundItem = 0;
//    }

//    m_backgroundItem = new QGraphicsPixmapItem();
//    this->addItem(m_backgroundItem);
//    m_backgroundPath = path;
//    QPixmap pixmap(path);
//    m_backgroundItem->setPixmap(pixmap);
//}

void GraphicsScene::addItem(QGraphicsItem * item) {
    if (item->type() == ItemType::GLine) {
        GraphicsLine *line = static_cast<GraphicsLine*>(item);
        QObject::connect(line, SIGNAL(itemRemoving()), this, SLOT(itemRemoving()));
    }
    if (item->type() == ItemType::GBox || item->type() == ItemType::GBand) {
        GraphicsBox *box = static_cast<GraphicsBox*>(item);
        QObject::connect(box, SIGNAL(itemRemoving()), this, SLOT(itemRemoving()));
    }
    QGraphicsScene::addItem(item);
}

void GraphicsScene::setMode(Mode mode){
    sceneMode = mode;
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event){
    emit sceneClick();
    m_movedItems.clear();
    m_trackingMoves = true;

    QGraphicsItem *pItem = itemAt(event->scenePos(), this->views().at(0)->transform() );
    if (pItem == nullptr) return;
    if (pItem->type() == ItemType::GBox)
        pItem = pItem->parentItem();

    QPointF origPoint = pItem->mapFromScene(event->scenePos());

    if(sceneMode == Mode::DrawLine) {
        QPointF startPoint(0,0);
        GraphicsLine *newLine = new GraphicsLine();
        newLine->setFieldType(m_newFieldType);
        newLine->setArrow(QtRptName::ArrowStart, newLineArrowStart);
        newLine->setArrow(QtRptName::ArrowEnd, newLineArrowEnd);
        newLine->setPos(startPoint);
        newLine->addPoint(origPoint);
        newLine->addPoint(origPoint);
        //addItem(newLine);
        //newLine->setParentItem(pItem);
        //band->newFieldTreeItem(newLine);
        m_undoStack->push(new AddCommand(newLine, this, pItem));
        newLine->setSelected(true);
        emit itemAdded(newLine);
        emit sceneModeChanged(newLine, Mode::SelectObject);
        setMode(Mode::SelectObject);
        m_trackingMoves = false;
    }
    if(sceneMode == DrawContainer) {
        GraphicsBox *graphicsBox = new GraphicsBox();
        graphicsBox->setFieldType(m_newFieldType);
        graphicsBox->setPos(origPoint);
        graphicsBox->setMenu(m_newFieldMenu);
        //addItem(graphicsBox);
        //graphicsBox->setParentItem(pItem);
        //band->newFieldTreeItem(graphicsBox);
        m_undoStack->push(new AddCommand(graphicsBox, this, pItem));
        graphicsBox->setSelected(true);
        emit itemAdded(graphicsBox);
        emit sceneModeChanged(graphicsBox, Mode::SelectObject);
        setMode(Mode::SelectObject);
        m_trackingMoves = false;
    }

    QGraphicsScene::mousePressEvent(event);
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    emit mousePos(event->scenePos());
    QGraphicsScene::mouseMoveEvent(event);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    setMode(Mode::SelectObject);
    QApplication::restoreOverrideCursor();

    if (sceneMode == Mode::SelectObject) {
        if (QApplication::keyboardModifiers() != Qt::ControlModifier) {
//            GraphicsBox *b = static_cast<GraphicsBox*>(itemAt(event->scenePos(), this->views().at(0)->transform()));
//            if (b == 0 or b->type() == 7) {  //Make image transparent for mouse clicking
//                QApplication::restoreOverrideCursor();
//                this->clearSelection();
//                emit itemSelected(0);
//            }
        }
    }

    for(auto itm : m_movedItems) {
        if (itm.item->type() == ItemType::GBox) {
            GraphicsBox *box = static_cast<GraphicsBox*>(itm.item);
            itm.newHeight = box->getHeight();
            itm.newWidth = box->getWidth();
            itm.newPos = box->pos();
            m_undoStack->push(new MoveGItemCommand(itm));
        }
        if (itm.item->type() == ItemType::GLine) {
            GraphicsLine *line = static_cast<GraphicsLine*>(itm.item);
            itm.newPointList = line->getPointList();
            itm.newPos = line->pos();
            m_undoStack->push(new MoveLineCommand(itm));
        }
    }

    QGraphicsScene::mouseReleaseEvent(event);
    m_trackingMoves = false;
}

void GraphicsScene::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Delete) {
        m_undoStack->push(new DelItemCommand(this));
        update();
        return;
    }

    for(auto item : this->items()) {
        bool isSelected = false;
        GraphicsBox *box = nullptr;
        if (item->type() == ItemType::GBox || item->type() == ItemType::GBand) {
            box = static_cast<GraphicsBox*>(item);
            isSelected = box->isSelected();
        }
        GraphicsLine *line = nullptr;
        if (item->type() == ItemType::GLine) {
            line = static_cast<GraphicsLine*>(item);
            isSelected = line->isSelected();
        }

        if (item->type() == ItemType::GBox || item->type() == ItemType::GLine || item->type() == ItemType::GBand) {
            if (isSelected) {
                if(event->key() == Qt::Key_Left && item->type() != ItemType::GBand) {
                    if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
                        if (box != nullptr)
                            box->setPos(box->pos().x()-1, box->pos().y());
                        if (line != nullptr)
                            line->setPos(line->pos().x()-1, line->pos().y());
                    }
                    if (QApplication::keyboardModifiers() == Qt::ShiftModifier) {
                        if (box != nullptr)
                            box->setWidth(box->getWidth()-1);
                        if (line != nullptr)
                            line->setLength(line->getLength()-1);
                    }
                    getMW()->setReportChanged();
                }
                if(event->key() == Qt::Key_Up) {
                    if (QApplication::keyboardModifiers() == Qt::ControlModifier && item->type() != ItemType::GBand) {
                        if (box != nullptr)
                            box->setPos(box->pos().x(), box->pos().y()-1);
                        if (line != nullptr)
                            line->setPos(line->pos().x(), line->pos().y()-1);
                    }
                    if (QApplication::keyboardModifiers() == Qt::ShiftModifier) {
                        if (box != nullptr)
                            box->setHeight(box->getHeight()-1);
                    }
                    getMW()->setReportChanged();
                }
                if(event->key() == Qt::Key_Right && item->type() != ItemType::GBand) {
                    if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
                        if (box != nullptr)
                            box->setPos(box->pos().x()+1, box->pos().y());
                        if (line != nullptr)
                            line->setPos(line->pos().x()+1, line->pos().y());
                    }
                    if (QApplication::keyboardModifiers() == Qt::ShiftModifier) {
                        if (box != nullptr)
                            box->setWidth(box->getWidth()+1);
                        if (line != nullptr)
                            line->setLength(line->getLength()+1);
                    }
                    getMW()->setReportChanged();
                }
                if(event->key() == Qt::Key_Down) {
                    if (QApplication::keyboardModifiers() == Qt::ControlModifier && item->type() != ItemType::GBand) {
                        if (box != nullptr)
                            box->setPos(box->pos().x(), box->pos().y()+1);
                        if (line != nullptr)
                            line->setPos(line->pos().x(), line->pos().y()+1);
                    }
                    if (QApplication::keyboardModifiers() == Qt::ShiftModifier) {
                        if (box != nullptr)
                            box->setHeight(box->getHeight()+1);
                    }
                    getMW()->setReportChanged();
                }
            }
        }
        emit itemResized(item);
    }
}

void GraphicsScene::itemRemoving() {
    //Из поп-апа
    //GraphicsHelperClass *helper = qobject_cast<GraphicsHelperClass*>(sender());
    //QGraphicsItem *item = static_cast<QGraphicsItem*>(helper->graphicsItem);
    //removeItem(item);
    m_undoStack->push(new DelItemCommand(this));
    update();
}

void GraphicsScene::removeItem(QGraphicsItem *item) {
    GraphicsHelperClass *helper = nullptr;
    if (item->type() == ItemType::GBand || item->type() == ItemType::GBox) {
        auto box = static_cast<GraphicsBox*>(item);
        helper = static_cast<GraphicsHelperClass*>(box);
        emit itemDeleting(box, helper->itemInTree);
    }
    if (item->type() == ItemType::GLine) {
        auto line = static_cast<GraphicsLine*>(item);
        helper = static_cast<GraphicsHelperClass*>(line);
        emit itemDeleting(line, helper->itemInTree);
    }

    QGraphicsScene::removeItem(item);
}

void GraphicsScene::itemResizing(QGraphicsItem *item) {
    emit itemResized(item);
}

void GraphicsScene::itemMoving(QGraphicsItem *item) {
    if (m_trackingMoves) {
        bool founded = false;
        QList<ItemsAndParams>::iterator i;
        for (i = m_movedItems.begin(); i != m_movedItems.end(); ++i)
            if ((*i).item == item)
                founded = true;

        if (!founded) {
            ItemsAndParams param;
            param.item = item;
            param.oldPos = item->pos();
            if (item->type() == ItemType::GBox) {
                GraphicsBox *box = static_cast<GraphicsBox*>(item);
                param.oldHeight = box->getHeight();
                param.oldWidth = box->getWidth();
            }
            if (item->type() == ItemType::GLine) {
                GraphicsLine *line = static_cast<GraphicsLine*>(item);
                param.oldPointList = line->getPointList();
            }
            m_movedItems.append(param);
        }
    }
}

void GraphicsScene::unselectAll() {
    for(auto item : this->items()) {
        if (item->type() == ItemType::GLine) {
            auto line = static_cast<GraphicsLine*>(item);
            line->setSelected(false);
        } else if (item->type() == ItemType::GBox) {
            auto box = static_cast<GraphicsBox*>(item);
            box->setSelected(false);
        } else if (item->type() == ItemType::GBand) {
            auto box = static_cast<GraphicsBox*>(item);
            box->setSelected(false);
        } else
            item->setSelected(false);
    }
}

void GraphicsScene::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsScene::drawBackground(painter,rect);

    if (isShowGrid == false) return;
    QColor c(200,200,255,125);
    painter->setPen(c);
    const int gridSize = m_koef*m_gridStep;
    QRectF rectView = this->views().at(0)->rect();

    qreal left = /*rect.left() +*/ -19 + m_leftM;
    qreal top = /*rect.top() +*/ -193 + m_topM;
    qreal bottom = rectView.height()-m_topM-m_bottomM+top;

    QVarLengthArray<QLineF, 100> lines;
    painter->drawRect(left, top,
                     rectView.width()-m_rightM-m_leftM,
                     rectView.height()-m_topM-m_bottomM);

    //vertical lines
    for (qreal x = left; x <= rect.right()-m_rightM; x += gridSize)
        lines.append(QLineF(x, top, x, bottom));
    //horizontal lines
    for (qreal y = top; y <= rect.bottom()-m_bottomM; y += gridSize)
        lines.append(QLineF(rect.left()+m_leftM, y, rect.right()-m_rightM, y));

    painter->drawLines(lines.data(), lines.size());
}
