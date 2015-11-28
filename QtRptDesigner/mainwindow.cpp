/*
Name: QtRpt
Version: 1.5.5
Web-site: http://www.qtrpt.tk
Programmer: Aleksey Osipov
E-mail: aliks-os@ukr.net
Web-site: http://www.aliks-os.tk

Copyright 2012-2015 Aleksey Osipov

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

#include "mainwindow.h"
#include "ui_mainwindow.h"

EditorDelegate::EditorDelegate(QObject *parent) : QItemDelegate(parent) {
    QObject::connect(this, SIGNAL(closeEditor(QWidget *, QAbstractItemDelegate::EndEditHint)),
                     this, SLOT(editorClose_(QWidget *, QAbstractItemDelegate::EndEditHint)));
}

void EditorDelegate::editorClose_(QWidget *editor, QAbstractItemDelegate::EndEditHint hint) {
    Q_UNUSED(editor);
    Q_UNUSED(hint);
    emit editorClose(this);
}

QWidget* EditorDelegate::createEditor(QWidget *parent,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const {
    if (index.column() == 1) {
        int command = index.model()->data(index, Qt::UserRole).toInt();
        switch(command) {
            case FontName: {
                QFontComboBox *editor = new QFontComboBox(parent);
                connect(editor, SIGNAL(activated(int)), this, SLOT(commitAndCloseEditor()));
                return editor;
                break;
            }
            case AligmentH: {
                QComboBox *editor = new QComboBox(parent);
                editor->addItem(tr("Left"),Qt::AlignLeft);
                editor->addItem(tr("Center"),Qt::AlignHCenter);
                editor->addItem(tr("Right"),Qt::AlignRight);
                editor->addItem(tr("Justify"),Qt::AlignJustify);
                connect(editor, SIGNAL(activated(int)), this, SLOT(commitAndCloseEditor()));
                return editor;
                break;
            }
            case AligmentV: {
                QComboBox *editor = new QComboBox(parent);
                editor->addItem(tr("Top"),Qt::AlignTop);
                editor->addItem(tr("Center"),Qt::AlignVCenter);
                editor->addItem(tr("Bottom"),Qt::AlignBottom);
                connect(editor, SIGNAL(activated(int)), this, SLOT(commitAndCloseEditor()));
                return editor;
                break;
            }
            case BarcodeType: {
                QComboBox *editor = new QComboBox(parent);
                BarCode::BarcodeTypePairList list = BarCode::getTypeList();
                for (int i=0; i < list.size(); i++) {
                    editor->addItem(list.at(i).second,list.at(i).first);
                }
                connect(editor, SIGNAL(activated(int)), this, SLOT(commitAndCloseEditor()));
                return editor;
                break;
            }
            case BarcodeFrameType: {
                QComboBox *editor = new QComboBox(parent);
                BarCode::FrameTypePairList list = BarCode::getFrameTypeList();
                for (int i=0; i < list.size(); i++) {
                    editor->addItem(list.at(i).second,list.at(i).first);
                }
                connect(editor, SIGNAL(activated(int)), this, SLOT(commitAndCloseEditor()));
                return editor;
                break;
            }
            case BorderColor:
            case FontColor:
            case BackgroundColor: {
                SelectColor *editor = new SelectColor(parent);
                QMargins margins(1,1,1,1);
                editor->setMargins(margins);
                QObject::connect(editor->button, SIGNAL(clicked()), this, SIGNAL(btnClicked()));
                return editor;
                break;
            }
            default: return QItemDelegate::createEditor(parent, option, index);
        }
    }
    return 0;
}

void EditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    if (index.column() == 1) {
        int command = index.model()->data(index, Qt::UserRole).toInt();
        switch (command) {
            case BarcodeType:
            case BarcodeFrameType:
            case AligmentH:
            case AligmentV: {
                QString value = index.model()->data(index, Qt::EditRole).toString();
                QComboBox *ed = qobject_cast<QComboBox*>(editor);
                ed->setCurrentIndex(ed->findText(value));
                break;
            }
            case FontName: {
                QString value = index.model()->data(index, Qt::EditRole).toString();
                QFontComboBox *ed = qobject_cast<QFontComboBox*>(editor);
                ed->setCurrentFont(QFont(value));
                break;
            }
            case BorderColor:
            case FontColor:
            case BackgroundColor: {
                QString value = index.model()->data(index, Qt::EditRole).toString();
                SelectColor *ed = qobject_cast<SelectColor*>(editor);
                ed->setBackGroundColor(value);
                break;
            }
            default: QItemDelegate::setEditorData(editor,index);
        }
    } else return;
}

void EditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex& index) const {
    if (index.column() == 1) {
        int command = index.model()->data(index, Qt::UserRole).toInt();
        switch (command) {
            case BarcodeFrameType:
            case BarcodeType: {
                QComboBox *ed = static_cast<QComboBox*>(editor);
                model->setData(index, ed->itemData(ed->currentIndex()));
                break;
            }
            case AligmentH:
            case AligmentV: {
                QComboBox *ed = static_cast<QComboBox*>(editor);
                model->setData(index, ed->currentIndex());
                break;
            }
            case FontName: {
                QFontComboBox *ed = static_cast<QFontComboBox*>(editor);
                model->setData(index, ed->currentFont().family());
                break;
            }
            case FontSize: {
                QItemDelegate::setModelData(editor,model,index);
                break;
            }
            case BorderColor:
            case FontColor:
            case BackgroundColor: {
                SelectColor *ed = qobject_cast<SelectColor*>(editor);
                model->setData(index, ed->getBackGroundColor(), Qt::EditRole);
                QBrush brush(Qt::white);
                model->setData(index, brush, Qt::ForegroundRole);
                break;
            }
            default: QItemDelegate::setModelData(editor,model,index);
        }
    } return;
}

void EditorDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const {
    if (index.column() == 1) {
        if (option.state & QStyle::State_Active) {
            QItemDelegate::paint(painter,option,index);
        } else {
            int command = index.model()->data(index, Qt::UserRole).toInt();
            switch (command) {
                case BorderColor:
                case FontColor:
                case BackgroundColor: {
                    QRect rect = option.rect;
                    rect.setHeight(rect.height()-2);
                    rect.setY(rect.y()+1);
                    rect.setWidth(rect.width()-4);
                    rect.setX(rect.x()+2);
                    QString value = index.model()->data(index, Qt::EditRole).toString();
                    painter->fillRect(rect,colorFromString(value));
                    break;
                }
                default:
                    QItemDelegate::paint(painter,option,index);
            }
        }
    } else
        QItemDelegate::paint(painter,option,index);
}

void EditorDelegate::commitAndCloseEditor() {
    QWidget *editor = qobject_cast<QWidget*>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

MainWindow::MainWindow(QWidget *parent) :  QMainWindow(parent), ui(new Ui::MainWindow) {    
    ui->setupUi(this);    
    widgetInFocus = 0;
    this->showMaximized();

    m_undoStack = new QUndoStack(this);
    QObject::connect(m_undoStack, SIGNAL(canUndoChanged(bool)), ui->actUndo, SLOT(setEnabled(bool)));
    QObject::connect(m_undoStack, SIGNAL(canRedoChanged(bool)), ui->actRedo, SLOT(setEnabled(bool)));
    QObject::connect(ui->actUndo, SIGNAL(triggered()), this, SLOT(undo()));
    QObject::connect(ui->actRedo, SIGNAL(triggered()), this, SLOT(redo()));

    cloneCont = 0;
    ui->treeParams->setColumnWidth(0,150);
    ui->treeParams->setColumnWidth(1,70);
    ui->treeParams->setFocusPolicy(Qt::NoFocus);
    ui->treeWidget->setFocusPolicy(Qt::NoFocus);

    EditorDelegate *d = new EditorDelegate(ui->treeParams);
    QObject::connect(d, SIGNAL(editorClose(QItemDelegate *)), this,  SLOT(closeEditor()));
    QObject::connect(d, SIGNAL(btnClicked()), this, SLOT(chooseColor()));
    ui->treeParams->setItemDelegate(d);

    cbFontName = new QFontComboBox(ui->toolBar);
    cbFontName->setEditable(true);
    cbFontName->setToolTip(tr("Font name"));
    cbFontSize = new QComboBox( ui->toolBar );
    cbFontSize->setEditable(true);
    cbFontSize->setToolTip(tr("Font size"));
    cbFontSize->addItem("6");
    cbFontSize->addItem("7");
    cbFontSize->addItem("8");
    cbFontSize->addItem("9");
    cbFontSize->addItem("10");
    cbFontSize->addItem("11");
    cbFontSize->addItem("12");
    cbFontSize->addItem("14");
    cbFontSize->addItem("16");
    cbFontSize->addItem("18");
    cbFontSize->addItem("20");
    cbFontSize->addItem("22");
    cbFontSize->addItem("24");
    cbFontSize->addItem("26");
    cbFontSize->addItem("28");
    cbFontSize->addItem("36");
    cbFontSize->addItem("48");
    cbFontSize->addItem("72");
    ui->toolBar->insertWidget(ui->actionBold,cbFontName);
    ui->toolBar->insertWidget(ui->actionBold,cbFontSize);

    cbZoom = new QComboBox(ui->mainToolBar);
    cbZoom->setEditable(true);
    cbZoom->setFixedWidth(70);
    cbZoom->setToolTip(tr("Zoom"));
    cbZoom->addItem("50%");
    cbZoom->addItem("75%");
    cbZoom->addItem("100%");
    cbZoom->addItem("125%");
    cbZoom->addItem("150%");
    cbZoom->addItem("175%");
    cbZoom->addItem("200%");
    cbZoom->addItem("300%");
    cbZoom->setCurrentIndex(2);
    ui->mainToolBar->insertWidget(ui->actFieldLeft,cbZoom);
    ui->mainToolBar->insertSeparator(ui->actFieldLeft);

    cbFrameWidth = new QComboBox( ui->toolBar );
    cbFrameWidth->setToolTip(tr("Frame width"));
    cbFrameWidth->addItem("1");
    cbFrameWidth->addItem("2");
    cbFrameWidth->addItem("3");
    cbFrameWidth->addItem("4");
    cbFrameWidth->addItem("5");
    ui->toolBar->addWidget(cbFrameWidth);
    QObject::connect(cbFrameWidth, SIGNAL(activated(int)), this, SLOT(changeFrameWidth()));
    QObject::connect(cbFontName, SIGNAL(activated(int)), this, SLOT(changeTextFont()));
    QObject::connect(cbFontSize, SIGNAL(activated(int)), this, SLOT(changeTextFont()));
    QObject::connect(cbZoom, SIGNAL(activated(int)), this, SLOT(changeZoom()));

    listFrameStyle = new QListWidget(this);
    listFrameStyle->setFixedHeight(116);
    listFrameStyle->setIconSize(QSize(85, 16));
    QObject::connect(listFrameStyle, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(setFrameStyle(QListWidgetItem *)));

    for (int i=1; i < 7; i++) {
        QListWidgetItem *item = new QListWidgetItem(listFrameStyle);
        QIcon icn;
        icn.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/fs%1.png").arg(i)), QIcon::Normal, QIcon::On);
        item->setIcon(icn);
        item->setData(Qt::UserRole,i);
        listFrameStyle->addItem(item);
    }

    this->setMouseTracking(true);
    newContMoving = false;

    rootItem = new QTreeWidgetItem(ui->treeWidget,0);
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/report.png")), QIcon::Normal, QIcon::On);
    rootItem->setIcon(0,icon);
    rootItem->setText(0,tr("Report"));
    rootItem->setExpanded(true);

    QList<int> lst;
    lst << 15 << 300;
    ui->splitter->setSizes(lst);

    newContList = new QList<RptContainer *>();
    cloneContList = new QList<RptContainer *>();
    QList<QWidget*> widgets = ui->toolBar->findChildren<QWidget*>();
    foreach (QWidget* widget, widgets)
        widget->installEventFilter(this);

    QActionGroup *alignmentHGroup = new QActionGroup(this);
    alignmentHGroup->addAction(ui->actAlignLeft);
    alignmentHGroup->addAction(ui->actAlignRight);
    alignmentHGroup->addAction(ui->actAlignJustify);
    alignmentHGroup->addAction(ui->actAlignCenter);

    QActionGroup *alignmentVGroup = new QActionGroup(this);
    alignmentVGroup->addAction(ui->actAlignTop);
    alignmentVGroup->addAction(ui->actAlignBottom);
    alignmentVGroup->addAction(ui->actAlignVCenter);

    QActionGroup *addGroup = new QActionGroup(this);
    addGroup->addAction(ui->actionInsert_band);
    addGroup->addAction(ui->actSelect_tool);
    addGroup->addAction(ui->actAddField);
    addGroup->addAction(ui->actAddPicture);
    addGroup->addAction(ui->actMagnifying);
    addGroup->addAction(ui->actAddDiagram);
    addGroup->addAction(ui->actAddDrawing);
    addGroup->addAction(ui->actAddBarcode);
    addGroup->addAction(ui->actAddRichText);

    ui->actSelect_tool->setChecked(true);

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        ui->menuFile->insertAction(ui->actionExit,recentFileActs[i]);
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

    separatorAct = ui->menuFile->addSeparator();

    updateRecentFileActions();
    ui->menuFile->insertAction(ui->actionExit,separatorAct);

    QObject::connect(ui->actAddBarcode, SIGNAL(triggered()), this, SLOT(addBarcode()));
    QObject::connect(ui->actAddField, SIGNAL(triggered()), this, SLOT(addFieldText()));
    QObject::connect(ui->actAddPicture, SIGNAL(triggered()), this, SLOT(AddPicture()));
    QObject::connect(ui->actAddRichText, SIGNAL(triggered()), this, SLOT(addFieldTextRich()));
    QObject::connect(ui->actAddDiagram, SIGNAL(triggered()), this, SLOT(addDiagram()));
    QObject::connect(ui->actAddCrossTab, SIGNAL(triggered()), this, SLOT(addFieldCrossTab()));

    QObject::connect(ui->actGroup, SIGNAL(triggered()), this, SLOT(setGroupingField()));
    QObject::connect(ui->actUngroup, SIGNAL(triggered()), this, SLOT(setGroupingField()));

    QObject::connect(ui->actionOpenReport, SIGNAL(triggered()), this, SLOT(openFile()));
    QObject::connect(ui->actionNewReport, SIGNAL(triggered()), this, SLOT(newReport()));
    QObject::connect(ui->actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(closeProgram()));
    QObject::connect(ui->actSaveAs, SIGNAL(triggered()), this, SLOT(saveReport()));
    QObject::connect(ui->actAlignCenter, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actAlignLeft, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actAlignRight, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actAlignJustify, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actAlignTop, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actAlignVCenter, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actAlignBottom, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actionBold, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actionItalic, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actionUnderline, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actionStrikeout, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actLineAll, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actLineBottom, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actLineLeft, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actLineNo, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actLineRight, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actLineTop, SIGNAL(triggered()), this, SLOT(changeTextFont()));
    QObject::connect(ui->actCut, SIGNAL(triggered()), this, SLOT(clipBoard()));
    QObject::connect(ui->actCopy, SIGNAL(triggered()), this, SLOT(clipBoard()));
    QObject::connect(ui->actPaste, SIGNAL(triggered()), this, SLOT(clipBoard()));
    QObject::connect(ui->actBackgroundColor, SIGNAL(triggered()), this, SLOT(chooseColor()));
    QObject::connect(ui->actBorderColor, SIGNAL(triggered()), this, SLOT(chooseColor()));
    QObject::connect(ui->actFontColor, SIGNAL(triggered()), this, SLOT(chooseColor()));    
    QObject::connect(ui->actAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    QObject::connect(ui->actNewReportPage, SIGNAL(triggered()), this, SLOT(newReportPage()));
    QObject::connect(ui->actDeleteReportPage, SIGNAL(triggered()), this, SLOT(deleteReportPage()));
    //QObject::connect(ui->actFrameStyle, SIGNAL(triggered()), this, SLOT(showFrameStyle()));
    QObject::connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(selTree(QTreeWidgetItem *, int)));
    QObject::connect(ui->treeParams, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(itemChanged(QTreeWidgetItem *, int)));
    QObject::connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(reportPageChanged(int)));
    QObject::connect(ui->actFieldBottom, SIGNAL(triggered()), this, SLOT(alignFields()));
    QObject::connect(ui->actFieldCenter, SIGNAL(triggered()), this, SLOT(alignFields()));
    QObject::connect(ui->actFieldLeft, SIGNAL(triggered()), this, SLOT(alignFields()));
    QObject::connect(ui->actFieldMiddle, SIGNAL(triggered()), this, SLOT(alignFields()));
    QObject::connect(ui->actFieldRight, SIGNAL(triggered()), this, SLOT(alignFields()));
    QObject::connect(ui->actFieldTop, SIGNAL(triggered()), this, SLOT(alignFields()));
    QObject::connect(ui->actFieldSameHeight, SIGNAL(triggered()), this, SLOT(alignFields()));
    QObject::connect(ui->actFieldSameWidth, SIGNAL(triggered()), this, SLOT(alignFields()));
    QObject::connect(ui->actSettings, SIGNAL(triggered()), this, SLOT(showSetting()));
    QObject::connect(ui->actGroupProperty, SIGNAL(triggered()), this, SLOT(openDBGroupProperty()));
    QObject::connect(ui->actCheckUpdates, SIGNAL(triggered()), this, SLOT(checkUpdates()));
    QObject::connect(ui->actPageSettings, SIGNAL(triggered()), this, SLOT(showPageSetting()));
    QObject::connect(ui->actPreview, SIGNAL(triggered()), this, SLOT(showPreview()));
    QObject::connect(ui->actDataSource, SIGNAL(triggered()), this, SLOT(showDataSource()));
    QObject::connect(ui->actReadme, SIGNAL(triggered()), this, SLOT(openReadme()));

    actRepTitle = new QAction(tr("Report Title"),this);
    actRepTitle->setObjectName("actRepTitle");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/reportTitle.png")), QIcon::Normal, QIcon::On);
    actRepTitle->setIcon(icon);
    QObject::connect(actRepTitle, SIGNAL(triggered()), this, SLOT(addBand()));

    actReportSummary = new QAction(tr("Report Summary"),this);
    actReportSummary->setObjectName("actReportSummary");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/reportFooter.png")), QIcon::Normal, QIcon::On);
    actReportSummary->setIcon(icon);
    QObject::connect(actReportSummary, SIGNAL(triggered()), this, SLOT(addBand()));

    actPageHeader = new QAction(tr("Page Header"),this);
    actPageHeader->setObjectName("actPageHeader");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/pageTitle.png")), QIcon::Normal, QIcon::On);
    actPageHeader->setIcon(icon);
    QObject::connect(actPageHeader, SIGNAL(triggered()), this, SLOT(addBand()));

    actPageFooter = new QAction(tr("Page Footer"),this);
    actPageFooter->setObjectName("actPageFooter");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/pageFooter.png")), QIcon::Normal, QIcon::On);
    actPageFooter->setIcon(icon);
    QObject::connect(actPageFooter, SIGNAL(triggered()), this, SLOT(addBand()));

    actMasterData = new QAction(tr("Master Data"),this);
    actMasterData->setObjectName("actMasterData");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/data.png")), QIcon::Normal, QIcon::On);
    actMasterData->setIcon(icon);
    QObject::connect(actMasterData, SIGNAL(triggered()), this, SLOT(addBand()));

    actDataGroupingHeader = new QAction(tr("Data Grouping Header"),this);
    actDataGroupingHeader->setObjectName("actDataGroupingHeader");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/dataGroupingHeader.png")), QIcon::Normal, QIcon::On);
    actDataGroupingHeader->setIcon(icon);
    QObject::connect(actDataGroupingHeader, SIGNAL(triggered()), this, SLOT(addBand()));
    actDataGroupingHeader->setEnabled(false);

    actDataGroupingFooter = new QAction(tr("Data Grouping Footer"),this);
    actDataGroupingFooter->setObjectName("actDataGroupingFooter");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/dataGroupingFooter.png")), QIcon::Normal, QIcon::On);
    actDataGroupingFooter->setIcon(icon);
    QObject::connect(actDataGroupingFooter, SIGNAL(triggered()), this, SLOT(addBand()));
    //actDataGroupingFooter->setEnabled(false);

    actMasterHeader = new QAction(tr("Master Header"),this);
    actMasterHeader->setObjectName("actMasterHeader");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/dataTitle.png")), QIcon::Normal, QIcon::On);
    actMasterHeader->setIcon(icon);
    QObject::connect(actMasterHeader, SIGNAL(triggered()), this, SLOT(addBand()));

    actMasterFooter = new QAction(tr("Master Footer"),this);
    actMasterFooter->setObjectName("actMasterFooter");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/dataFooter.png")), QIcon::Normal, QIcon::On);
    actMasterFooter->setIcon(icon);
    QObject::connect(actMasterFooter, SIGNAL(triggered()), this, SLOT(addBand()));

    //Menu for selecting bands type
    QMenu *subBand1 = new QMenu(this);
    subBand1->setObjectName("subBand1");
    subBand1->addAction(actRepTitle);
    subBand1->addAction(actPageHeader);
    subBand1->addAction(actDataGroupingHeader);
    subBand1->addAction(actMasterHeader);
    subBand1->addAction(actMasterData);
    subBand1->addAction(actMasterFooter);
    subBand1->addAction(actDataGroupingFooter);
    subBand1->addAction(actPageFooter);
    subBand1->addAction(actReportSummary);

    ui->actionInsert_band->setMenu(subBand1);
    QToolButton * btn1 = qobject_cast<QToolButton *>(ui->toolBar_3->widgetForAction(ui->actionInsert_band));
    btn1->setPopupMode(QToolButton::InstantPopup);
    QObject::connect(subBand1, SIGNAL(aboutToShow()), this, SLOT(clickOnTBtn()));

    //Actions for drawing
    /*QAction *actDrawLine1 = new QAction(tr("Line1"),this);
    actDrawLine1->setEnabled(false);
    actDrawLine1->setObjectName("actDrawLine1");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/line1.png")), QIcon::Normal, QIcon::On);
    actDrawLine1->setIcon(icon);
    QObject::connect(actDrawLine1, SIGNAL(triggered()), this, SLOT(addDraw()));*/

    QAction *actDrawLine2 = new QAction(tr("Line"),this);
    actDrawLine2->setObjectName("actDrawLine2");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/line2.png")), QIcon::Normal, QIcon::On);
    actDrawLine2->setIcon(icon);
    QObject::connect(actDrawLine2, SIGNAL(triggered()), this, SLOT(addDraw()));

    QAction *actDrawLine3 = new QAction(tr("Line with arrow at the end"),this);
    actDrawLine3->setObjectName("actDrawLine3");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/line3.png")), QIcon::Normal, QIcon::On);
    actDrawLine3->setIcon(icon);
    QObject::connect(actDrawLine3, SIGNAL(triggered()), this, SLOT(addDraw()));

    QAction *actDrawLine4 = new QAction(tr("Line with arrow at the start"),this);
    actDrawLine4->setObjectName("actDrawLine4");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/line4.png")), QIcon::Normal, QIcon::On);
    actDrawLine4->setIcon(icon);
    QObject::connect(actDrawLine4, SIGNAL(triggered()), this, SLOT(addDraw()));

    QAction *actDrawLine5 = new QAction(tr("Line with arrows at both side"),this);
    actDrawLine5->setObjectName("actDrawLine5");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/line5.png")), QIcon::Normal, QIcon::On);
    actDrawLine5->setIcon(icon);
    QObject::connect(actDrawLine5, SIGNAL(triggered()), this, SLOT(addDraw()));
    //
    QAction *actDrawRectangle = new QAction(tr("Rectangle"),this);
    actDrawRectangle->setObjectName("actDrawRectangle");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/rectangle.png")), QIcon::Normal, QIcon::On);
    actDrawRectangle->setIcon(icon);
    QObject::connect(actDrawRectangle, SIGNAL(triggered()), this, SLOT(addDraw()));

    QAction *actDrawRoundedRectangle = new QAction(tr("Rounded rectangle"),this);
    actDrawRoundedRectangle->setObjectName("actDrawRoundedRectangle");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/roundedReactangle.png")), QIcon::Normal, QIcon::On);
    actDrawRoundedRectangle->setIcon(icon);
    QObject::connect(actDrawRoundedRectangle, SIGNAL(triggered()), this, SLOT(addDraw()));

    QAction *actDrawEllipse = new QAction(tr("Ellipse"),this);
    actDrawEllipse->setObjectName("actDrawEllipse");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/ellipse.png")), QIcon::Normal, QIcon::On);
    actDrawEllipse->setIcon(icon);
    QObject::connect(actDrawEllipse, SIGNAL(triggered()), this, SLOT(addDraw()));

    QAction *actDrawTriangle = new QAction(tr("Triangle"),this);
    actDrawTriangle->setObjectName("actDrawTriangle");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/triangle.png")), QIcon::Normal, QIcon::On);
    actDrawTriangle->setIcon(icon);
    QObject::connect(actDrawTriangle, SIGNAL(triggered()), this, SLOT(addDraw()));

    QAction *actDrawRhombus = new QAction(tr("Rhombus"),this);
    actDrawRhombus->setObjectName("actDrawRhombus");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/rhombus.png")), QIcon::Normal, QIcon::On);
    actDrawRhombus->setIcon(icon);
    QObject::connect(actDrawRhombus, SIGNAL(triggered()), this, SLOT(addDraw()));

    //Menu for selecting drawing
    QMenu *subBand2 = new QMenu(this);
    subBand2->setObjectName("subBand2");
    //subBand2->addAction(actDrawLine1);
    subBand2->addAction(actDrawLine2);
    subBand2->addAction(actDrawLine3);
    subBand2->addAction(actDrawLine4);
    subBand2->addAction(actDrawLine5);
    subBand2->addAction(actDrawRectangle);
    subBand2->addAction(actDrawRoundedRectangle);
    subBand2->addAction(actDrawEllipse);
    subBand2->addAction(actDrawTriangle);
    subBand2->addAction(actDrawRhombus);    

    ui->actAddDrawing->setMenu(subBand2);
    QToolButton * btn2 = qobject_cast<QToolButton *>(ui->toolBar_3->widgetForAction(ui->actAddDrawing));
    btn2->setPopupMode(QToolButton::InstantPopup);
    QObject::connect(subBand2, SIGNAL(aboutToShow()), this, SLOT(clickOnTBtn()));

    //Menu for container
    contMenu = new QMenu(this);
    contMenu->addAction(ui->actCut);
    contMenu->addAction(ui->actCopy);
    contMenu->addAction(ui->actPaste);
    contMenu->setFocusPolicy(Qt::NoFocus);
    contMenu->addSeparator();

    //Menu for Data Group Band
    bandMenu = new QMenu(this);
    bandMenu->addAction(ui->actPaste);
    bandMenu->addSeparator();
    bandMenu->addAction(ui->actGroupProperty);
    bandMenu->setFocusPolicy(Qt::NoFocus);
    bandMenu->addSeparator();

    newReportPage();

    sqlDesigner = new SqlDesigner(this);
    QObject::connect(sqlDesigner, SIGNAL(changed(bool)), ui->actSaveReport, SLOT(setEnabled(bool)));
    ui->horizontalLayout->addWidget(sqlDesigner);
    sqlDesigner->setVisible(false);
    QDomElement dsElement;
    sqlDesigner->showDataSource(dsElement);

    xmlDoc = new QDomDocument("Report");

    QSettings settings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    if (settings.value("CheckUpdates",true).toBool())
        checkUpdates();

    if (QCoreApplication::instance()->arguments().size() > 1) {
        fileName = QCoreApplication::instance()->arguments().at(1);
        openFile();
    }
}

void MainWindow::openReadme() {
    QDesktopServices::openUrl(QUrl("file:///"+QCoreApplication::applicationDirPath()+"/readme.pdf", QUrl::TolerantMode));
}

void MainWindow::checkUpdates() {
    XYZDownloadManager dl(this);
    dl.setParent(this);
    QString urlVersion = "http://garr.dl.sourceforge.net/project/qtrpt/version.txt";
    dl.setTarget(urlVersion);
    dl.download(true);

    QEventLoop loop;
    QObject::connect(&dl,SIGNAL(done()),&loop,SLOT(quit()));
    loop.exec();
}

void MainWindow::openDBGroupProperty() {
    ReportBand *band = qobject_cast<ReportBand *>(widgetInFocus);
    FldPropertyDlg *dlg = new FldPropertyDlg(this);
    if ((band !=0) && (band->bandType == DataGroupHeader)) {
        dlg->showThis(1,band,"");
        setParamTree(StartNewNumeration, band->getStartNewNumertaion());
        setParamTree(StartNewPage, band->getStartNewPage());
        ui->actSaveReport->setEnabled(true);
    }
    delete dlg;
}

void MainWindow::updateRecentFileActions() {
    QSettings settings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) { //QFileInfo(fullFileName).fileName()
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    separatorAct->setVisible(numRecentFiles > 0);
}

void MainWindow::openRecentFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        fileName = action->data().toString();
        openFile();
    }
}

void MainWindow::setCurrentFile(const QString &fileName) {
    setWindowFilePath(fileName);

    QSettings settings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}

void MainWindow::bandResing(int value) {
    ui->actSaveReport->setEnabled(true);
    setParamTree(Height, value);
}

void MainWindow::changeFrameWidth() {
    listFrameStyle->close();

    if (widgetInFocus == 0) return;
    RptContainer *cont = qobject_cast<RptContainer *>(widgetInFocus);
    if (cont == 0) return;

    ui->actSaveReport->setEnabled(true);

    cont->setBorder(FrameWidth,this->cbFrameWidth->currentText());
    cont->setFocus();
}

void MainWindow::showFrameStyle(QPoint pos) {
    pos.setY(pos.y()+81);
    pos.setX(pos.x());
    listFrameStyle->move(pos);
    listFrameStyle->show();
}

void MainWindow::setFrameStyle(QListWidgetItem * item) {
    listFrameStyle->close();

    if (widgetInFocus == 0) return;
    RptContainer *cont = qobject_cast<RptContainer *>(widgetInFocus);
    if (cont == 0) return;

    ui->actSaveReport->setEnabled(true);

    switch (item->data(Qt::UserRole).toInt()) {
        case 1: {
            cont->setBorder(FrameStyle,Solid);
            break;
        }
        case 2: {
            cont->setBorder(FrameStyle,Dashed);
            break;
        }
        case 3: {
            cont->setBorder(FrameStyle,Dotted);
            break;
        }
        case 4: {
            cont->setBorder(FrameStyle,Dot_dash);
            break;
        }
        case 5: {
            cont->setBorder(FrameStyle,Dot_dot_dash);
            break;
        }
        case 6: {
            cont->setBorder(FrameStyle,Double);
            break;
        }
    }

    cont->setFocus();
}

void MainWindow::showAbout() {
    AboutDlg *dlg = new AboutDlg(this);
    dlg->exec();
    delete dlg;
}

void MainWindow::reportPageChanged(int index) {
    rootItem->takeChildren();
    widgetInFocus = 0;

    this->actRepTitle->setEnabled(true);
    this->actReportSummary->setEnabled(true);
    this->actPageHeader->setEnabled(true);
    this->actPageFooter->setEnabled(true);
    this->actMasterData->setEnabled(true);
    this->actMasterFooter->setEnabled(true);
    this->actMasterHeader->setEnabled(true);
    this->actDataGroupingHeader->setEnabled(true);
    this->actDataGroupingFooter->setEnabled(true);

    if (ui->tabWidget->count() == 1)
        ui->actDeleteReportPage->setEnabled(false);
    else
        ui->actDeleteReportPage->setEnabled(true);

    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->widget(index));
    foreach (QWidget *widget, repPage->getReportItems()) {
        ReportBand *band = qobject_cast<ReportBand *>(widget);
        if (band != 0) {
            rootItem->addChild(band->itemInTree);
            band->itemInTree->setExpanded(true);
            band->setFocus();
            //---
            if (band->bandType == ReportTitle) this->actRepTitle->setEnabled(false);
            if (band->bandType == ReportSummary) this->actReportSummary->setEnabled(false);
            if (band->bandType == PageHeader) this->actPageHeader->setEnabled(false);
            if (band->bandType == PageFooter) this->actPageFooter->setEnabled(false);
            if (band->bandType == MasterData) this->actMasterData->setEnabled(false);
            if (band->bandType == MasterFooter) this->actMasterFooter->setEnabled(false);
            if (band->bandType == MasterHeader) this->actMasterHeader->setEnabled(false);
            if (band->bandType == DataGroupHeader) this->actDataGroupingHeader->setEnabled(false);
            if (band->bandType == DataGroupFooter) this->actDataGroupingFooter->setEnabled(false);
        }
    }
}

void MainWindow::newReportPage() {
    RepScrollArea *repScrollArea = new RepScrollArea(this);
    repScrollArea->rootItem = rootItem;
    repScrollArea->icon = icon;
    repScrollArea->isShowGrid = ui->actShowGrid->isChecked();
    QObject::connect(ui->actShowGrid, SIGNAL(triggered(bool)), repScrollArea, SLOT(showGrid(bool)));
    QObject::connect(repScrollArea, SIGNAL(bandResing(int)), this, SLOT(bandResing(int)));

    ui->tabWidget->addTab(repScrollArea,tr("Page %1").arg(ui->tabWidget->count()+1));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);

    QSettings settings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    ui->actShowGrid->setChecked(settings.value("ShowGrid",true).toBool());
    repScrollArea->showGrid(settings.value("ShowGrid",true).toBool());
}

void MainWindow::deleteReportPage() {
    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
    enableAdding();
}

void MainWindow::generateName(RptContainer *cont) {    
    bool good = false;
    QString contName;
    switch(cont->getType()) {
        case Barcode: {
            contName = "barcode%1";
            break;
        }
        case Text: {
            contName = "field%1";
            break;
        }
        case TextRich: {
            contName = "richText%1";
            break;
        }
        case Image: {
            contName = "image%1";
            break;
        }
        case Diagram: {
            contName = "diagram%1";
            break;
        }
        case Reactangle: {
            contName = "reactangle%1";
            break;
        }
        case Circle: {
            contName = "circle%1";
            break;
        }
        case RoundedReactangle: {
            contName = "roundedReactangle%1";
            break;
        }
        case Triangle: {
            contName = "triangle%1";
            break;
        }
        case Rhombus: {
            contName = "rhombus%1";
            break;
        }
        case QtRptName::Line: {
            contName = "line%1";
            break;
        }
        default:
            contName = "field%1";
    }

    int cf = 1;
    QList<RptContainer *> lst = this->findChildren<RptContainer *>();
    while (!good) {
        bool fnd = false;
        QList<RptContainer*>::const_iterator it = lst.constBegin();
        while (it != lst.end()) {
            if ((*it)->objectName() == QString(contName).arg(cf)) {
                fnd = true;
                break;
            }
            ++it;
        }
        if (fnd)
            cf += 1;
        else {
            cont->setObjectName(QString(contName).arg(cf));
            return;
        }
    }
}

void MainWindow::clickOnTBtn() {
    if (sender()->objectName() == "subBand1") {
        ui->actionInsert_band->setChecked(true);
    }
    if (sender()->objectName() == "subBand2") {
        ui->actAddDrawing->setChecked(true);
    }
}

void MainWindow::showPageSetting() {
    PageSettingDlg *dialog = new PageSettingDlg(this);
    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->widget( ui->tabWidget->currentIndex() ));
    dialog->showThis(repPage->pageSetting);
    if (dialog->result() == QDialog::Accepted) {
        repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->widget(ui->tabWidget->currentIndex()));
        repPage->pageSetting = dialog->pageSetting;
        repPage->setPaperSize(0);
        ui->actSaveReport->setEnabled(true);
    }
    delete dialog;
}

void MainWindow::showSetting() {
    SettingDlg *dialog = new SettingDlg(this);
    dialog->showThis();
    delete dialog;
    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    QSettings settings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    ui->actShowGrid->setChecked(settings.value("ShowGrid",true).toBool());
    repPage->showGrid(settings.value("ShowGrid",true).toBool());
}

void MainWindow::showDataSource() {
    sqlDesigner->setVisible( ui->actDataSource->isChecked() );
    if (ui->actDataSource->isChecked() ) {

    }
}

QDomElement MainWindow::getDataSourceElement(QDomNode n) {
    QDomElement dsElement;
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if ((!e.isNull()) && (e.tagName() == "DataSource")) {
            dsElement = e;
        }
        n = n.nextSibling();
    }
    return dsElement;
}

void MainWindow::delItemInTree(QTreeWidgetItem *item) {
    if (item == 0) return;
    QTreeWidgetItem *itemAbove = ui->treeWidget->itemAbove(item);
    if (itemAbove == 0) return;
    QTreeWidgetItem *parent = item->parent();
    while (item->childCount() > 0) {
        QTreeWidgetItem *tmp = item->takeChild(0);
        tmp = 0;
        delete tmp;
    }
    int index = parent->indexOfChild(item);
    delete parent->takeChild(index);
    ui->treeWidget->setCurrentItem(itemAbove);
    ui->actSaveReport->setEnabled(true);

    RptContainer *r = qobject_cast<RptContainer *>(sender());
    if (r != 0) {        
        r->parentWidget()->repaint();
    }

    //Корректируем расположение бэндов
    ReportBand *reportBand = qobject_cast<ReportBand *>(sender());
    if (reportBand == 0) return;

    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    if (repPage != 0) repPage->correctBandGeom(reportBand);

    if (reportBand->bandType == ReportTitle) actRepTitle->setEnabled(true);
    if (reportBand->bandType == ReportSummary) actReportSummary->setEnabled(true);
    if (reportBand->bandType == PageHeader) actPageHeader->setEnabled(true);
    if (reportBand->bandType == PageFooter) actPageFooter->setEnabled(true);
    if (reportBand->bandType == MasterData) actMasterData->setEnabled(true);
    if (reportBand->bandType == MasterFooter) actMasterFooter->setEnabled(true);
    if (reportBand->bandType == MasterHeader) actMasterHeader->setEnabled(true);
    if (reportBand->bandType == DataGroupHeader) actDataGroupingHeader->setEnabled(true);
    if (reportBand->bandType == DataGroupFooter) actDataGroupingFooter->setEnabled(true);
}

void MainWindow::closeProgram() {
    if (ui->actSaveReport->isEnabled()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Saving"),tr("The report was changed.\nSave the report?"),
                                         QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
            saveReport();
    }
    this->close();
}

//Open file
void MainWindow::openFile() {    
    if (ui->actSaveReport->isEnabled()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Saving"),tr("The report was changed.\nSave the report?"),
                                         QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
            saveReport();
    }
    ui->actSaveReport->setEnabled(false);

    if (sender() != 0 && sender() == ui->actionOpenReport) {
        newReport();

        QSettings settings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
        settings.setIniCodec("UTF-8");
        QString folderPath = QApplication::applicationDirPath();
        if (settings.value("recentFileList").toStringList().count() > 0)
            folderPath = QFileInfo(settings.value("recentFileList").toStringList().at(0)).path();
        fileName = QFileDialog::getOpenFileName(this, tr("Select File"), folderPath, "XML (*.xml)");
        if (fileName.isEmpty())
            return;
    }

    QFile file(fileName);
    setCurrentFile(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!xmlDoc->setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    QDomElement docElem = xmlDoc->documentElement();  //get root element
    QDomElement repElem;

    while (ui->tabWidget->count() > 1) {
        ui->tabWidget->removeTab(ui->tabWidget->count()-1);
    }

    for (int t=0; t<docElem.childNodes().count(); t++) {
        if (docElem.tagName() == "Reports" )  //Делаем проверку для совместимости со старыми версиями
            repElem = docElem.childNodes().at(t).toElement();
        else
            repElem = docElem;

        if (t != 0) newReportPage();

        RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->widget(t));
        repPage->clearReport();
        repPage->pageSetting.marginsLeft     = repElem.attribute("marginsLeft").toInt();
        repPage->pageSetting.marginsRight    = repElem.attribute("marginsRight").toInt();
        repPage->pageSetting.marginsTop      = repElem.attribute("marginsTop").toInt();
        repPage->pageSetting.marginsBottom   = repElem.attribute("marginsBottom").toInt();
        repPage->pageSetting.pageWidth       = repElem.attribute("pageWidth").toInt();
        repPage->pageSetting.pageHeight      = repElem.attribute("pageHeight").toInt();
        repPage->pageSetting.pageOrientation = repElem.attribute("orientation").toInt();
        repPage->pageSetting.border          = repElem.attribute("border", "0").toInt();
        repPage->pageSetting.borderWidth     = repElem.attribute("borderWidth", "1").toInt();
        repPage->pageSetting.borderColor     = repElem.attribute("borderColor", "rgba(0,0,0,255)");
        repPage->pageSetting.borderStyle     = repElem.attribute("borderStyle", "solid");

        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), tr("Page %1").arg(ui->tabWidget->currentIndex()+1));
        repPage->setPaperSize(0);
        repPage->setUpdatesEnabled(false);

        //гуляем во всем элементам, родителем которых является корневой
        QDomNode n = repElem.firstChild();
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if ((!e.isNull()) && (e.tagName() == "ReportBand")) {
                BandType type = QtRptName::Undefined;
                if (e.attribute("type") == "ReportTitle")
                    type = ReportTitle;
                if (e.attribute("type") == "PageHeader")
                    type = PageHeader;
                if (e.attribute("type") == "MasterHeader")
                    type = MasterHeader;
                if (e.attribute("type") == "MasterData")
                    type = MasterData;
                if (e.attribute("type") == "MasterFooter")
                    type = MasterFooter;
                if (e.attribute("type") == "PageFooter")
                    type = PageFooter;
                if (e.attribute("type") == "ReportSummary")
                    type = ReportSummary;
                if (e.attribute("type") == "DataGroupHeader")
                    type = DataGroupHeader;
                if (e.attribute("type") == "DataGroupFooter")
                    type = DataGroupFooter;

                if (type == QtRptName::Undefined) continue;

                RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
                ReportBand *reportBand = repPage->m_addBand(e.attribute("name"),type,
                                                            bandMenu,e.attribute("height").toInt());
                reportBand->setGroupingField(e.attribute("groupingField"));
                reportBand->setStartNewNumeration(e.attribute("startNewNumeration").toInt());
                reportBand->setShowInGroup(e.attribute("showInGroup").toInt());
                reportBand->setStartNewPage(e.attribute("startNewPage").toInt());

                QDomNode c = n.firstChild();
                while(!c.isNull()) {
                    QDomElement e = c.toElement(); // try to convert the node to an element.
                    if (!e.isNull() && (e.tagName() == "TContainerField" || e.tagName() == "TContainerLine")) {
                        QPoint p(e.attribute("left").toInt(),e.attribute("top").toInt());
                        RptContainer *cont = nullptr;
                        if (e.tagName() == "TContainerField") {
                            TContainerField *contField = new TContainerField(reportBand->contWidget,p);                            
                            cont = contField;
                        }
                        if (e.tagName() == "TContainerLine") {
                            TContainerLine *contLine = new TContainerLine(reportBand->contWidget,p);
                            cont = contLine;
                        }

                        if (cont != nullptr) {
                            cont->loadParamFromXML(e);
                            cont->setMenu(contMenu);

                            setContainerConnections(cont);
                            reportBand->newFieldTreeItem(cont);
                        }
                        QCoreApplication::processEvents();
                    }
                    c = c.nextSibling();
                }
                repPage->overlay->raise();
            }

            n = n.nextSibling();
        }

        QDomElement dsElement = getDataSourceElement( repElem.firstChild() );
        sqlDesigner->showDataSource(dsElement);

        repPage->setUpdatesEnabled(true);
        QCoreApplication::processEvents();
    }

    ui->treeWidget->clearSelection();
    widgetInFocus = 0;
    this->setFocus();
    ui->tabWidget->setCurrentIndex(0);
    reportPageChanged(0);
    this->cbZoom->setCurrentIndex(2);
    ui->actSaveReport->setEnabled(false);
    this->setWindowTitle("QtRPT Designer "+fileName);
    enableAdding();
    m_undoStack->clear();
}

//Select color from dialog and set param
void MainWindow::chooseColor() {
    if (widgetInFocus == 0) return;
    RptContainer *cont = qobject_cast<RptContainer *>(widgetInFocus);
    EditorDelegate *ed = qobject_cast<EditorDelegate*>(sender());
    if (cont == 0) return;
    QColor color;
    QColorDialog *dlg = new QColorDialog(color, this);
    if (dlg->exec() == QDialog::Accepted) {
        color = dlg->selectedColor();
        ui->actSaveReport->setEnabled(true);
    } else return;

    Command command;
    if (ed != 0) {
        command = (Command)ui->treeParams->currentItem()->data(1,Qt::UserRole).toInt();
    } else {
        command = getCommand(sender());
    }
    execButtonCommand(command,color);

    delete dlg;
}

void MainWindow::changeTextFont() {
    if (widgetInFocus == 0) return;
    TContainerField *widget = qobject_cast<TContainerField *>(widgetInFocus);
    if (widget == 0) return;

    QAction *action = qobject_cast<QAction *>(sender());
    QComboBox *cmb = qobject_cast<QComboBox *>(sender());
    Command command = getCommand(sender());
    QVariant v;
    if (action != 0) {
        v = action->isChecked();
        if (action == ui->actAlignLeft) v=0;
        if (action == ui->actAlignRight) v=2;
        if (action == ui->actAlignCenter) v=1;
        if (action == ui->actAlignJustify) v=3;
        if (action == ui->actAlignTop) v=0;
        if (action == ui->actAlignVCenter) v=1;
        if (action == ui->actAlignBottom) v=2;
    }
    if (cmb != 0) {
        v = cmb->itemText(cmb->currentIndex());
    }

    execButtonCommand(command,v);
    ui->actSaveReport->setEnabled(true);
}

//Определяем, тип команды в зависимости от нажатой кнопки
Command MainWindow::getCommand(QObject *widget) {
    if (widget == 0) return None;
    QAction *action = qobject_cast<QAction *>(widget);
    QComboBox *cmb = qobject_cast<QComboBox *>(widget);
    if (action != 0) {
        if (action == ui->actionBold) return Bold;
        else if (action == ui->actionItalic) return Italic;
        else if (action == ui->actionUnderline) return Underline;
        else if (action == ui->actionStrikeout) return Strikeout;
        else if (action == ui->actLineLeft) return FrameLeft;
        else if (action == ui->actLineRight) return FrameRight;
        else if (action == ui->actLineBottom) return FrameBottom;
        else if (action == ui->actLineTop) return FrameTop;
        else if (action == ui->actLineNo) return FrameNo;
        else if (action == ui->actLineAll) return FrameAll;
        else if (action == ui->actAlignLeft) return AligmentH;
        else if (action == ui->actAlignRight) return AligmentH;
        else if (action == ui->actAlignCenter) return AligmentH;
        else if (action == ui->actAlignJustify) return AligmentH;
        else if (action == ui->actAlignTop) return AligmentV;
        else if (action == ui->actAlignVCenter) return AligmentV;
        else if (action == ui->actAlignBottom) return AligmentV;
        else if (action == ui->actFrameStyle) return FrameStyle;
        else if (action == ui->actBorderColor) return BorderColor;
        else if (action == ui->actBackgroundColor) return BackgroundColor;
        else if (action == ui->actFontColor) return FontColor;
        else return None;
    }
    if (cmb != 0) {
        if (cmb == cbFontSize)
            return FontSize;
        if (cmb == cbFontName)
            return FontName;
        if (cmb == cbFrameWidth)
            return FrameWidth;
        return None;
    }
    return None;
}

//Get from container signal about geometry/position changes
void MainWindow::contGeomChanging(QRect oldRect, QRect newRect) {
    ui->actSaveReport->setEnabled(true);
    shiftToDelta(oldRect,newRect,sender(), true);
}

void MainWindow::shiftToDelta(QRect oldRect, QRect newRect, QObject *sender, bool change) {
    TContainerLine *contLine = qobject_cast<TContainerLine *>(widgetInFocus);
    TContainerField *contField = qobject_cast<TContainerField *>(widgetInFocus);
    if (contLine != 0)
        setParamTree(Length, (int)contLine->getLength());
    if (contField != 0) {
        setParamTree(Left, newRect.x());
        setParamTree(Top, newRect.y());
        setParamTree(Width, newRect.width());
        setParamTree(Height, newRect.height());
    }

    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    foreach(RptContainer *cont, repPage->findChildren<RptContainer *>()) {
        if (cont->isSelected() && cont != sender) {
            int delta;
            if (oldRect.x() != newRect.x()) {
                //двигаем по гор
                delta = newRect.x() - oldRect.x();

                if (change) {
                    QPoint newPos(cont->x(),cont->y());
                    newPos.setX(newPos.x()+delta);
                    cont->move(newPos);
                } else {
                    QRect m_oldRect = cont->getOldGeom();
                    m_oldRect.moveLeft(m_oldRect.x() + delta);
                    cont->setOldGeom(m_oldRect);
                }
            }
            if (oldRect.y() != newRect.y()) {
                //двигаем по верт
                delta = newRect.y() - oldRect.y();

                if (change) {
                    QPoint newPos(cont->x(),cont->y());
                    newPos.setY(newPos.y()+delta);
                    cont->move(newPos);
                } else {
                    QRect m_oldRect = cont->getOldGeom();
                    m_oldRect.moveTop(m_oldRect.y()+delta);
                    cont->setOldGeom(m_oldRect);
                }
            }
            if (oldRect.width() != newRect.width()) {
                //изменяем ширину
                delta = newRect.width() - oldRect.width();

                if (change) {
                    cont->resize(cont->width()+delta, cont->height());
                } else {
                    QRect m_oldRect = cont->getOldGeom();
                    m_oldRect.setWidth(m_oldRect.width()+delta);
                    cont->setOldGeom(m_oldRect);
                }
            }
            if (oldRect.height() != newRect.height()) {
                //изменяем высоту
                delta = newRect.height() - oldRect.height();

                if (change) {
                    cont->resize(cont->width(), cont->height()+delta);
                } else {
                    QRect m_oldRect = cont->getOldGeom();
                    m_oldRect.setHeight(m_oldRect.height()+delta);
                    cont->setOldGeom(m_oldRect);
                }
            }
        }
    }
    if (change)
        repPage->overlay->repaint();
}

void MainWindow::contGeomChanged(QRect oldRect, QRect newRect) {
    m_undoStack->push(new GeometryContainerCommand( getSelectedContainer() ));
    shiftToDelta(oldRect,newRect,sender(),false);
}

QList<RptContainer *> MainWindow::getSelectedContainer() {
    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    RptContainerList selContList;
    foreach (RptContainer *cont, repPage->findChildren<RptContainer *>()) {
        if (cont->isSelected()) {
            selContList.append(cont);
        }
    }
    if (selContList.isEmpty()) {
        RptContainer *cont = qobject_cast<RptContainer *>(widgetInFocus);
        if (cont != 0)
            selContList.append(cont);
    }
    return selContList;
}

void MainWindow::undo() {
    qDebug()<<tr("Going to make undo: ")<<m_undoStack->undoText();
    m_undoStack->undo();
    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    repPage->overlay->repaint();
    showParamState();
}

void MainWindow::redo() {
    qDebug()<<tr("Going to make redo: ")<<m_undoStack->undoText();
    m_undoStack->redo();
    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    repPage->overlay->repaint();
    showParamState();
}

void MainWindow::setGroupingField() {
    if (widgetInFocus == 0) return;
    QList<RptContainer *> list = getSelectedContainer();
    QString groupName = "";
    if (sender() == ui->actGroup) {
        groupName = "group%1";
        bool good = false;
        int cf = 1;
        QList<RptContainer *> lst = this->findChildren<RptContainer *>();
        while (!good) {
            bool fnd = false;
            QList<RptContainer*>::const_iterator it = lst.constBegin();
            while (it != lst.end()) {
                if ((*it)->getGroupName() == QString(groupName).arg(cf)) {
                    fnd = true;
                    break;
                }
                ++it;
            }

            if (fnd)
                cf += 1;
            else {
                groupName = QString(groupName).arg(cf);
                break;
            }
        }
    }

    RptContainer *contF = qobject_cast<RptContainer *>(widgetInFocus);    
    foreach (RptContainer *cont, list) {
        if (sender() == ui->actUngroup && cont->getGroupName() == contF->getGroupName()) {
            cont->setGroupName("");
            cont->setSelected(false,false);
        }
        if (sender() == ui->actGroup && cont->isSelected())
            cont->setGroupName(groupName);
    }

    if (sender() == ui->actUngroup) {
        RptContainer *contF = qobject_cast<RptContainer *>(widgetInFocus);
        contF->parentWidget()->repaint();
    }
}

//Container's selection
void MainWindow::setWidgetInFocus(bool inFocus) {
    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    if (inFocus && sender() != 0) {
        if (QApplication::keyboardModifiers() != Qt::ShiftModifier) {
            widgetInFocus = qobject_cast<QWidget *>(sender());
            RptContainer *contF = qobject_cast<RptContainer *>(widgetInFocus);
            if (contF != 0) {
                contF->setSelected(true,false);

                foreach (RptContainer *cont, repPage->findChildren<RptContainer *>()) {
                    if (!cont->getGroupName().isEmpty() && cont->getGroupName() == contF->getGroupName() && cont != sender()) {
                        cont->setSelected(true,false);
                    }

                    //Un-select containters
                    if (cont->isSelected() && cont != sender()) {
                        if (!contF->getGroupName().isEmpty() && cont->getGroupName() != contF->getGroupName())
                            cont->setSelected(false,false);
                        if (contF->getGroupName().isEmpty())
                            cont->setSelected(false,false);
                    }
                }

                //contF->parentWidget()->repaint();
            } else {
                //Un-select containters
                foreach (RptContainer *cont, repPage->findChildren<RptContainer *>()) {
                    if (cont->isSelected() && cont != sender()) {
                        cont->setSelected(false,false);
                    }
                }
            }

            ui->treeParams->clear();
            showParamState();            
        }
        repPage->overlay->repaint();  //???
    }
}

void MainWindow::alignFields() {
    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    TContainerField *cont = qobject_cast<TContainerField *>(widgetInFocus);
    foreach (RptContainer *container, repPage->findChildren<TContainerField *>()) {
        if (container->isSelected()) {
            QPoint newPos(container->x(),container->y());
            QRect rect1(cont->geometry());
            QRect rect2(container->geometry());

            if (sender() == ui->actFieldRight) {
                newPos.setX(rect1.x()+rect1.width()-rect2.width());
                container->move(newPos);
            }
            if (sender() == ui->actFieldLeft) {
                newPos.setX(cont->x());
                container->move(newPos);
            }
            if (sender() == ui->actFieldMiddle) {
                newPos.setX(rect1.x()+rect1.width()/2-rect2.width()/2);
                container->move(newPos);
            }
            if (sender() == ui->actFieldTop) {
                newPos.setY(cont->y());
                container->move(newPos);
            }
            if (sender() == ui->actFieldBottom) {
                newPos.setY(rect1.y()+rect1.height()-rect2.height());
                container->move(newPos);
            }
            if (sender() == ui->actFieldCenter) {
                newPos.setY(rect1.y()+rect1.height()/2-rect2.height()/2);
                container->move(newPos);
            }
            if (sender() == ui->actFieldSameHeight) {
                container->resize(container->width(), rect1.height());
            }
            if (sender() == ui->actFieldSameWidth) {
                container->resize(rect1.width(), container->height());
            }
        }
    }
    cont->parentWidget()->repaint();
}

void MainWindow::saveReport() {
    xmlDoc->clear();
    QDomElement docElem = xmlDoc->createElement("Reports");
    docElem.setAttribute("lib","QtRPT");
    docElem.setAttribute("programmer","Aleksey Osipov");
    docElem.setAttribute("email","aliks-os@ukr.net");
    xmlDoc->appendChild(docElem);

    //before saving, we are deleting all nodes
    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        n.parentNode().removeChild(n);
        n = docElem.firstChild();
    }

    for (int rp=0; rp<ui->tabWidget->count(); rp++) {
        RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->widget(rp));
        QDomElement repElem = xmlDoc->createElement("Report");
        repElem.setAttribute("pageNo",QString::number(rp+1));
        repElem.setAttribute("pageHeight",repPage->pageSetting.pageHeight);
        repElem.setAttribute("pageWidth",repPage->pageSetting.pageWidth);
        repElem.setAttribute("marginsLeft",repPage->pageSetting.marginsLeft);
        repElem.setAttribute("marginsRight",repPage->pageSetting.marginsRight);
        repElem.setAttribute("marginsTop",repPage->pageSetting.marginsTop);
        repElem.setAttribute("marginsBottom",repPage->pageSetting.marginsBottom);
        repElem.setAttribute("orientation",repPage->pageSetting.pageOrientation);
        repElem.setAttribute("border",repPage->pageSetting.border);
        repElem.setAttribute("borderWidth",repPage->pageSetting.borderWidth);
        repElem.setAttribute("borderColor",repPage->pageSetting.borderColor);
        repElem.setAttribute("borderStyle",repPage->pageSetting.borderStyle);
        docElem.appendChild(repElem);

        foreach (QWidget *widget, repPage->getReportItems()) {
            RptContainer *cont = qobject_cast<RptContainer *>(widget);
            ReportBand *band = qobject_cast<ReportBand *>(widget);
            if (cont == 0 && band == 0) {
                continue;
            } else {
                setXMLProperty(&repElem, widget);
            }
        }
        setXMLProperty(&repElem, sqlDesigner); //Set XML for DataSource
    }

    if (fileName.isEmpty() || fileName.isNull() || sender() == ui->actSaveAs) {
        QString tmpfileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("XML Files (*.xml)"));
        if (tmpfileName.isEmpty() || tmpfileName.isNull() ) return;
        fileName = tmpfileName;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)){
        setCurrentFile(fileName);
        QTextStream stream(&file);
        xmlDoc->save(stream, 2, QDomNode::EncodingFromTextStream);
        file.close();
        ui->actSaveReport->setEnabled(false);
        this->setWindowTitle("QtRPT Designer "+fileName);
    } else {
        QMessageBox::warning(this, tr("Error"), file.errorString());
    }
}

bool MainWindow::setXMLProperty(QDomElement *repElem, QWidget *widget) {
    RptContainer *cont = qobject_cast<RptContainer *>(widget);
    ReportBand *band = qobject_cast<ReportBand *>(widget);
    SqlDesigner *sqlDesigner = qobject_cast<SqlDesigner *>(widget);
    QDomElement elem; // здесь будет сохранен нужный нам узел
    if (sqlDesigner != 0) {
        elem = sqlDesigner->saveParamToXML(xmlDoc);
        if (!elem.isNull())
            repElem->appendChild(elem);
    }
    if (band != 0) {
        elem = xmlDoc->createElement(band->metaObject()->className());
        QString type;
        if (band->bandType == ReportTitle)
            type = "ReportTitle";
        if (band->bandType == ReportSummary)
            type = "ReportSummary";
        if (band->bandType == PageHeader)
            type = "PageHeader";
        if (band->bandType == PageFooter)
            type = "PageFooter";
        if (band->bandType == MasterData)
            type = "MasterData";
        if (band->bandType == MasterFooter) {
            type = "MasterFooter";
            elem.setAttribute("showInGroup",band->getShowInGroup());
        }
        if (band->bandType == MasterHeader) {
            type = "MasterHeader";
            elem.setAttribute("showInGroup",band->getShowInGroup());
        }
        if (band->bandType == DataGroupHeader) {
            type = "DataGroupHeader";
            elem.setAttribute("groupingField",band->getGroupingField());
            elem.setAttribute("startNewNumeration",band->getStartNewNumertaion());
            elem.setAttribute("startNewPage",band->getStartNewPage());
        }
        if (band->bandType == DataGroupFooter) {
            type = "DataGroupFooter";
            //elem.setAttribute("groupingField",band->getGroupingField());
            elem.setAttribute("showInGroup","1");
        }
        elem.setAttribute("name",band->objectName());
        elem.setAttribute("type",type);
        elem.setAttribute("top",widget->geometry().y());
        elem.setAttribute("left",widget->geometry().x());
        elem.setAttribute("width",band->baseSize().width());
        elem.setAttribute("height",band->baseSize().height()-band->titleHeight);
        repElem->appendChild(elem);
    }
    if (cont != 0) {
        QString parent = cont->parent()->parent()->objectName();        
        QDomNodeList nodelist = repElem->elementsByTagName("ReportBand");
        for (int i=0; i != nodelist.count(); i++) {
            QDomNode prn = nodelist.item(i).toElement();
            if (prn.toElement().attribute("name") == parent) {
                elem = cont->saveParamToXML(xmlDoc);
                prn.appendChild(elem);
            }
        }
    }
    return false;
}

//Show param of the container
void MainWindow::showParamState() {
    if (widgetInFocus == 0) return;
    ui->treeParams->clear();
    ReportBand *rep = qobject_cast<ReportBand *>(widgetInFocus);
    TContainerField *contField = qobject_cast<TContainerField *>(widgetInFocus);
    TContainerLine *contLine = qobject_cast<TContainerLine *>(widgetInFocus);
    bool enbl1 = false;
    bool enbl2 = false;

    ui->actBackgroundColor->setEnabled(false);
    this->cbFontName->setEnabled(false);
    this->cbFontSize->setEnabled(false);
    this->cbFrameWidth->setEnabled(false);
    ui->actionBold->setEnabled(false);
    ui->actionItalic->setEnabled(false);
    ui->actionUnderline->setEnabled(false);
    ui->actionStrikeout->setEnabled(false);
    ui->actFontColor->setEnabled(false);

    if (rep != 0) {
        ui->actBackgroundColor->setEnabled(false);
        ui->actAlignBottom->setChecked(false);
        ui->actAlignCenter->setChecked(false);
        ui->actAlignJustify->setChecked(false);
        ui->actAlignLeft->setChecked(false);
        ui->actAlignRight->setChecked(false);
        ui->actAlignTop->setChecked(false);
        ui->actAlignVCenter->setChecked(false);
        ui->actionBold->setChecked(false);
        ui->actionItalic->setChecked(false);
        ui->actionUnderline->setChecked(false);
        ui->actionStrikeout->setChecked(false);
        ui->actLineAll->setChecked(false);
        ui->actLineNo->setChecked(false);
        ui->actLineBottom->setChecked(false);
        ui->actLineLeft->setChecked(false);
        ui->actLineRight->setChecked(false);
        ui->actLineTop->setChecked(false);

        selectItemInTree(rep->itemInTree);
        setParamTree(Name, rep->objectName());
        setParamTree(Height, rep->geometry().height() - rep->titleHeight);
        switch(rep->bandType) {
            case DataGroupHeader: {
                setParamTree(StartNewNumeration, rep->getStartNewNumertaion());
                setParamTree(StartNewPage, rep->getStartNewPage());
                break;
            }
            case MasterHeader:
            case MasterFooter: {
                setParamTree(ShowInGroup, rep->getShowInGroup());
                break;
            }
            default:
                break;
        }
    } else if (contField != 0) {
        enbl1 = true;
        enbl2 = true;

        QLabel *label = qobject_cast< QLabel * >(contField->childWidget);
        int al = 0;
        int alV = 0;
        if (label->alignment() & Qt::AlignLeft) al = 0;
        else if (label->alignment() & Qt::AlignHCenter) al = 1;
        else if (label->alignment() & Qt::AlignRight) al = 2;
        else if (label->alignment() & Qt::AlignJustify) al = 3;

        if (label->alignment() & Qt::AlignTop) alV = 0;
        else if (label->alignment() & Qt::AlignVCenter) alV = 1;
        else if (label->alignment() & Qt::AlignBottom) alV = 2;

        bool top = contField->borderIsCheck(FrameTop);
        bool bottom = contField->borderIsCheck(FrameBottom);
        bool left = contField->borderIsCheck(FrameLeft);
        bool right = contField->borderIsCheck(FrameRight);

        //Sets params in the tree
        selectItemInTree(contField->itemInTree);        
        setParamTree(Name, contField->objectName());
        setParamTree(Height, contField->geometry().height());
        setParamTree(Width, contField->geometry().width());
        setParamTree(Left, contField->geometry().x());
        setParamTree(Top, contField->geometry().y());
        setParamTree(Printing, contField->getPrinting());

        switch (contField->getType()) {
            case Circle:
            case Triangle:
            case Rhombus:
            case RoundedReactangle:
            case Reactangle: {
                ui->actBackgroundColor->setEnabled(true);
                ui->actBorderColor->setEnabled(true);
                this->cbFrameWidth->setEnabled(true);

                setParamTree(BackgroundColor, contField->getColorValue(BackgroundColor));
                setParamTree(BorderColor, contField->getColorValue(BorderColor));
                setParamTree(FrameWidth, contField->getBorderWidth());
                break;
            }
            case Diagram: {
                break;
            }
            case Barcode: {
                setParamTree(BarcodeType, contField->getBarcodeType());
                setParamTree(BarcodeFrameType, contField->getBarcodeFrameType());
                break;
            }
            case Image: {
                ui->actBackgroundColor->setEnabled(false);
                setParamTree(AligmentH, al);
                setParamTree(AligmentV, alV);
                setParamTree(IgnoreRatioAspect, contField->getIgnoreAspectRatio());
                break;
            }
            case TextImage: {
                ui->actBackgroundColor->setEnabled(false);
                setParamTree(AligmentH, al);
                setParamTree(AligmentV, alV);
                break;
            }
            case DatabaseImage: {
                ui->actBackgroundColor->setEnabled(false);
                setParamTree(AligmentH, al);
                setParamTree(AligmentV, alV);
                setParamTree(AutoHeight, contField->getAutoHeight());
                break;
            }
            case Text: {
                ui->actBackgroundColor->setEnabled(true);
                ui->actBorderColor->setEnabled(true);
                this->cbFontName->setEnabled(true);
                this->cbFontSize->setEnabled(true);
                this->cbFrameWidth->setEnabled(true);
                ui->actionBold->setEnabled(true);
                ui->actionItalic->setEnabled(true);
                ui->actionUnderline->setEnabled(true);
                ui->actionStrikeout->setEnabled(true);
                ui->actFontColor->setEnabled(true);

                setParamTree(AligmentH, al);
                setParamTree(AligmentV, alV);
                setParamTree(AutoHeight, contField->getAutoHeight());
                setParamTree(BackgroundColor, contField->getColorValue(BackgroundColor));
                setParamTree(BorderColor, contField->getColorValue(BorderColor));
                setParamTree(Font, tr("Font"));
                setParamTree(FontName, label->font().family(), true);
                setParamTree(FontSize, label->font().pointSize(), true);
                setParamTree(Bold, label->font().bold(), true);
                setParamTree(Italic, label->font().italic(), true);
                setParamTree(Underline, label->font().underline(), true);
                setParamTree(Strikeout, label->font().strikeOut(), true);
                setParamTree(FontColor, contField->getColorValue(FontColor), true);
                setParamTree(Frame, tr("Frame"));
                setParamTree(FrameLeft, left, true);
                setParamTree(FrameRight, right, true);
                setParamTree(FrameTop, top, true);
                setParamTree(FrameBottom, bottom, true);
                setParamTree(FrameWidth, contField->getBorderWidth());
                setParamTree(TextWrap, contField->getTextWrap());
                break;
            }
            case TextRich: {
                enbl2 = false;
                ui->actBackgroundColor->setEnabled(false);
                this->cbFontName->setEnabled(false);
                this->cbFontSize->setEnabled(false);
                ui->actionBold->setEnabled(false);
                ui->actionItalic->setEnabled(false);
                ui->actionUnderline->setEnabled(false);
                ui->actionStrikeout->setEnabled(false);
                ui->actFontColor->setEnabled(false);
                break;
            }
            default: {
            }
        }
    } else if (contLine != 0) {
        selectItemInTree(contLine->itemInTree);
        setParamTree(BorderColor, contLine->getColorValue(BorderColor));
        setParamTree(Name, contLine->objectName());
        setParamTree(Length, (int)contLine->getLength());
        setParamTree(Printing, contLine->getPrinting());
        setParamTree(ArrowStart, contLine->getArrow(ArrowStart));
        setParamTree(ArrowEnd, contLine->getArrow(ArrowEnd));
    }

    ui->actAlignBottom->setEnabled(enbl2);
    ui->actAlignCenter->setEnabled(enbl2);
    ui->actAlignJustify->setEnabled(enbl2);
    ui->actAlignLeft->setEnabled(enbl2);
    ui->actAlignRight->setEnabled(enbl2);
    ui->actAlignTop->setEnabled(enbl2);
    ui->actAlignVCenter->setEnabled(enbl2);

    ui->actLineAll->setEnabled(enbl1);
    ui->actLineNo->setEnabled(enbl1);
    ui->actLineBottom->setEnabled(enbl1);
    ui->actLineLeft->setEnabled(enbl1);
    ui->actLineRight->setEnabled(enbl1);
    ui->actLineTop->setEnabled(enbl1);
}

//Поиск ветки в дереве параметра
QTreeWidgetItem *MainWindow::findItemInTree(Command command) {
    QTreeWidgetItemIterator it(ui->treeParams);
    while (*it) {
        QTreeWidgetItem *item = (*it);
        if (item->data(1,Qt::UserRole) == command)
            return item;
        ++it;
    }
    return 0;
}

void MainWindow::newReport() {
    if (ui->actSaveReport->isEnabled()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Saving"),tr("The report was changed.\nSave the report?"),
                                         QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
            saveReport();
    }
    while (ui->tabWidget->count() > 1) {
        ui->tabWidget->removeTab(ui->tabWidget->count()-1);
    }

    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->widget(0));
    repPage->clearReport();
    cbZoom->setEditText("100%");
    qDeleteAll(rootItem->takeChildren());
    ui->actSaveReport->setEnabled(false);
    enableAdding();
    fileName = "";
    this->setWindowTitle("QtRPT Designer "+fileName);
    QDomElement dsElement;
    sqlDesigner->showDataSource(dsElement);
    enableAdding();
    m_undoStack->clear();
}

void MainWindow::selectItemInTree(QTreeWidgetItem *item) {
    QTreeWidgetItemIterator it(rootItem);
    while (*it) {
        (*it)->setSelected(false);
        if ((*it) == item)
            (*it)->setSelected(true);
        ++it;
    }
}

void MainWindow::execButtonCommand(Command command, QVariant value) {
    if (command == None) return;
    if (widgetInFocus == 0) return;

    //before changing params gets params
    BArrayList oldList = ParamsContainerCommand::getBArrayFromContList(getSelectedContainer());

    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    foreach (RptContainer *cont, repPage->findChildren<RptContainer *>()) {
        if (cont->isSelected())
            processCommand(command, value, cont);
    }

    processCommand(command, value, widgetInFocus);
    setParamTree(command,value);
    ui->actSaveReport->setEnabled(true);

    //gets new params
    BArrayList newList = ParamsContainerCommand::getBArrayFromContList(getSelectedContainer());
    QList<PairCont> lst = ParamsContainerCommand::compoundArrays(oldList,newList);
    m_undoStack->push(new ParamsContainerCommand( lst ));
}

void MainWindow::processCommand(Command command, QVariant value, QWidget *widget) {
    RptContainer *cont = qobject_cast<RptContainer *>(widget);
    TContainerField *contField = qobject_cast<TContainerField *>(widget);
    TContainerLine *contLine = qobject_cast<TContainerLine *>(widget);
    ReportBand *rep = qobject_cast<ReportBand *>(widget);

    QLabel *label = 0;
    QFont fnt;
    if (contField != 0) {
        label = qobject_cast<QLabel *>(cont->childWidget);
        fnt = label->font();
    }

    switch(command) {
        case None: {
            return;
            break;
        }
        case Name: {
            widgetInFocus->setObjectName(value.toString());
            cont->itemInTree->setText(0,value.toString());
            break;
        }
        case Bold: {
            fnt.setBold(value.toBool());
            break;
        }
        case Italic: {
            fnt.setItalic(value.toBool());
            break;
        }
        case Underline: {
            fnt.setUnderline(value.toBool());
            break;
        }
        case Strikeout: {
            fnt.setStrikeOut(value.toBool());
            break;
        }
        case FontSize: {
            contField->setFontSize(value.toInt());
            fnt = label->font();
            break;
        }
        case FontName: {
            fnt.setFamily(value.toString());
            break;
        }
        case FrameLeft:
        case FrameRight:
        case FrameTop:
        case FrameBottom: {
            cont->setBorder(command,QColor(),value.toBool());
            break;
        }
        case FrameNo: {
            cont->setBorder(command,0);
            break;
        }
        case FrameAll: {
            cont->setBorder(command,0);
            break;
        }
        case FrameWidth: {
            cont->setBorder(command,value);
            break;
        }
        case AligmentH: {
            Qt::Alignment vAl;
            if (label->alignment() & Qt::AlignTop)
                vAl = Qt::AlignTop;
            else if (label->alignment() & Qt::AlignVCenter)
                vAl = Qt::AlignVCenter;
            else if (label->alignment() & Qt::AlignBottom)
                vAl = Qt::AlignBottom;
            else
                vAl = Qt::AlignVCenter;

            if (value.toInt() == 0)
                label->setAlignment(Qt::AlignLeft | vAl);
            if (value.toInt() == 1)
                label->setAlignment(Qt::AlignHCenter | vAl);
            if (value.toInt() == 2)
                label->setAlignment(Qt::AlignRight | vAl);
            if (value.toInt() == 3)
                label->setAlignment(Qt::AlignJustify | vAl);
            break;
        }
        case BarcodeType: {
            contField->setBarcodeType((BarCode::BarcodeTypes)value.toInt());
            break;
        }
        case BarcodeFrameType: {
            contField->setBarcodeFrameType((BarCode::FrameTypes)value.toInt());
            break;
        }
        case AligmentV: {
            Qt::Alignment hAl;
            if (label->alignment() & Qt::AlignLeft)
                hAl = Qt::AlignLeft;
            else if (label->alignment() & Qt::AlignRight)
                hAl = Qt::AlignRight;
            else if (label->alignment() & Qt::AlignHCenter)
                hAl = Qt::AlignHCenter;
            else if (label->alignment() & Qt::AlignJustify)
                hAl = Qt::AlignJustify;
            else
                hAl = Qt::AlignLeft;

            if (value.toInt() == 0)
                label->setAlignment(hAl | Qt::AlignTop);
            if (value.toInt() == 1)
                label->setAlignment(hAl | Qt::AlignVCenter);
            if (value.toInt() == 2)
                label->setAlignment(hAl | Qt::AlignBottom);
            break;
        }
        case Left: {
            QPoint r = cont->pos();
            r.setX(value.toInt());
            cont->move(r);
            break;
        }
        case Top: {
            QPoint r = cont->pos();
            r.setY(value.toInt());
            cont->move(r);
            break;
        }
        case Width: {
            QRect r = cont->geometry();
            r.setWidth(value.toInt());
            cont->setGeometry(r);
            break;
        }
        case Height: {
            QWidget *widget;
            if (rep != 0) widget = rep;
            else widget = cont;
            QRect r = widget->geometry();
            r.setHeight(value.toInt());
            widget->setGeometry(r);
            if (rep != 0) {
                value = value.toInt()-rep->titleHeight;
                rep->setBaseSize(rep->baseSize().width(),value.toInt()+rep->titleHeight);
            }
            break;
        }
        case Length: {
            contLine->line.setLength(value.toInt());
            break;
        }
        case Printing: {
            cont->setPrinting( value.toBool() == true ? "1" : "0" );
            //value = cont->printing;
            break;
        }
        case IgnoreRatioAspect: {
            contField->setIgnoreAspectRatio(value.toBool());
            break;
        }
        case ArrowStart: {
            contLine->setArrow(ArrowStart,value.toBool());
            break;
        }
        case ArrowEnd: {
            contLine->setArrow(ArrowEnd,value.toBool());
            break;
        }
        case StartNewNumeration: {
            rep->setStartNewNumeration(value.toBool());
            break;
        }
        case ShowInGroup: {
            rep->setShowInGroup(value.toBool());
            break;
        }
        case StartNewPage: {
            rep->setStartNewPage(value.toBool());
            break;
        }
        case AutoHeight: {
            contField->setAutoHeight(value.toBool());
            break;
        }
        case TextWrap: {
            contField->setTextWrap(value.toBool());
            break;
        }
        case FontColor: {
            cont->setSheetValue(FontColor,colorToString(value.toString()));
            break;
        }
        case BackgroundColor: {
            cont->setSheetValue(BackgroundColor,colorToString(value.toString()));
            break;
        }
        case BorderColor: {
            cont->setBorder(BorderColor,value.toString());
            break;
        }
        default: break;
    }

    if (contField != 0) {
        label->setFont(fnt);
    }

    if (contLine != 0) {
        contLine->parentWidget()->repaint();
    }
}

//Sets params in the tree
void MainWindow::setParamTree(Command command, QVariant value, bool child) {
    if (command == None) return;
    TContainerField *cont = qobject_cast<TContainerField *>(widgetInFocus);
    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    QTreeWidgetItem *item = findItemInTree(command);
    QTreeWidgetItem *parentNode = 0;
    if (item == 0 && !child) {
        item = new QTreeWidgetItem(ui->treeParams);
    }

    switch (command) {
        case Name: {
            item->setText(0,tr("Name"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            item->setText(1,value.toString());
            break;
        }
        case AligmentH: {
            item->setText(0,tr("Aligment hor"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            switch(value.toInt()) {
                case 0: {
                    item->setText(1,tr("Left"));
                    ui->actAlignLeft->setChecked(true);
                    break;
                }
                case 1: {
                    item->setText(1,tr("Center"));
                    ui->actAlignCenter->setChecked(true);
                    break;
                }
                case 2: {
                    item->setText(1,tr("Right"));
                    ui->actAlignRight->setChecked(true);
                    break;
                }
                case 3: {
                    item->setText(1,tr("Justify"));
                    ui->actAlignJustify->setChecked(true);
                    break;
                }
                default: item->setText(1,"");
            }
            break;
        }
        case AligmentV: {
            item->setText(0,tr("Aligment ver"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            switch(value.toInt()) {
                case 0: {
                    item->setText(1,tr("Top"));
                    ui->actAlignTop->setChecked(true);
                    break;
                }
                case 1: {
                    item->setText(1,tr("Center"));
                    ui->actAlignVCenter->setChecked(true);
                    break;
                }
                case 2: {
                    item->setText(1,tr("Bottom"));
                    ui->actAlignBottom->setChecked(true);
                    break;
                }
                default: item->setText(1,"");
            }
            break;
        }
        case Height: {
            item->setText(0,tr("Height"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);            
            item->setText(1, value.toString());
            //item->setText(1, QString::number(qRound(value.toDouble()/repPage->getScale())));
            break;
        }
        case Width: {
            item->setText(0,tr("Width"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            item->setText(1,QString::number(value.toInt()-1));
            break;
        }
        case Left: {
            item->setText(0,tr("Left"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            item->setText(1, value.toString());
            //item->setText(1, QString::number(qRound(value.toDouble()/repPage->getScale())));
            break;
        }
        case Length: {
            item->setText(0,tr("Length"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            item->setText(1, value.toString());
            break;
        }
        case Top: {
            item->setText(0,tr("Top"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            item->setText(1, value.toString());
            //item->setText(1, QString::number(qRound(value.toDouble()/repPage->getScale())));
            break;
        }
        case Frame:
        case Font: {
            item->setText(0,value.toString());
            item->setText(1,"");
            break;
        }
        case FrameLeft: {
            parentNode = findItemInTree(Frame);
            if (item == 0)
                item = new QTreeWidgetItem(parentNode);
            item->setText(0,tr("Left"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            ui->actLineLeft->setChecked(value.toBool());
            break;
        }
        case FrameRight: {
            parentNode = findItemInTree(Frame);
            if (item == 0)
                item = new QTreeWidgetItem(parentNode);
            item->setText(0,tr("Right"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            ui->actLineRight->setChecked(value.toBool());
            break;
        }
        case FrameTop: {
            parentNode = findItemInTree(Frame);
            if (item == 0)
                item = new QTreeWidgetItem(parentNode);
            item->setText(0,tr("Top"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            ui->actLineTop->setChecked(value.toBool());
            break;
        }
        case FrameBottom: {
            parentNode = findItemInTree(Frame);
            if (item == 0)
                item = new QTreeWidgetItem(parentNode);
            item->setText(0,tr("Bottom"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            ui->actLineBottom->setChecked(value.toBool());
            break;
        }
        case FrameNo: {
            setParamTree(FrameLeft,false,true);
            setParamTree(FrameRight,false,true);
            setParamTree(FrameTop,false,true);
            setParamTree(FrameBottom,false,true);
            ui->actLineAll->setChecked(false);
            ui->actLineNo->setChecked(true);
            break;
        }
        case FrameAll: {
            setParamTree(FrameLeft,true,true);
            setParamTree(FrameRight,true,true);
            setParamTree(FrameTop,true,true);
            setParamTree(FrameBottom,true,true);
            ui->actLineAll->setChecked(true);
            break;
        }
        case FrameWidth: {
            item->setText(0,tr("FrameWidth"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            item->setText(1, value.toString());
            this->cbFrameWidth->setCurrentIndex(this->cbFrameWidth->findText(value.toString()));
            break;
        }
        case FontName: {
            parentNode = findItemInTree(Font);
            if (item == 0)
                item = new QTreeWidgetItem(parentNode);
            item->setText(0,tr("Name"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            item->setText(1,value.toString());
            cbFontName->setCurrentFont(QFont(value.toString()));
            break;
        }
        case FontSize: {
            parentNode = findItemInTree(Font);
            if (item == 0)
                item = new QTreeWidgetItem(parentNode);
            item->setText(0,tr("Size"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            value = value.toInt() - (repPage->getScale()*100-100)/100*8 ;
            //if (value.toInt() < 2) return;
            item->setText(1,value.toString());
            cbFontSize->setEditText(value.toString());
            break;
        }
        case Bold: {
            parentNode = findItemInTree(Font);
            if (item == 0)
                item = new QTreeWidgetItem(parentNode);
            item->setText(0,tr("Bold"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            ui->actionBold->setChecked(value.toBool());
            break;
        }
        case Italic: {
            parentNode = findItemInTree(Font);
            if (item == 0)
                item = new QTreeWidgetItem(parentNode);
            item->setText(0,tr("Italic"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            ui->actionItalic->setChecked(value.toBool());
            break;
        }
        case Underline: {
            parentNode = findItemInTree(Font);
            if (item == 0)
                item = new QTreeWidgetItem(parentNode);
            item->setText(0,tr("Underline"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            ui->actionUnderline->setChecked(value.toBool());
            break;
        }
        case Strikeout: {
            parentNode = findItemInTree(Font);
            if (item == 0)
                item = new QTreeWidgetItem(parentNode);
            item->setText(0,tr("Strikeout"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            ui->actionStrikeout->setChecked(value.toBool());
            break;
        }
        case Printing: {
            item->setText(0,tr("Printing"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            break;
        }
        case StartNewNumeration: {
            item->setText(0,tr("Start New Numeration"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            break;
        }
        case ShowInGroup: {
            item->setText(0,tr("Show In Group"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            break;
        }
        case StartNewPage: {
            item->setText(0,tr("Start New Page"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            break;
        }
        case AutoHeight: {
            item->setText(0,tr("AutoHeight"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            break;
        }
        case IgnoreRatioAspect: {
            item->setText(0,tr("IgnoreRatioAspect"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            break;
        }
        case ArrowStart: {
            item->setText(0,tr("ArrowStart"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            break;
        }
        case ArrowEnd: {
            item->setText(0,tr("ArrowEnd"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            break;
        }
        case TextWrap: {
            item->setText(0,tr("TextWrap"));
            if (value.toBool()) item->setCheckState(1,Qt::Checked);
            else item->setCheckState(1,Qt::Unchecked);
            break;
        }
        case BackgroundColor: {
            item->setText(0,tr("BackgroundColor"));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
            item->setText(1,colorToString(value.toString()));
            break;
        }
        case BorderColor: {
            item->setText(0,tr("BorderColor"));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
            item->setText(1,colorToString(value.toString()));
            break;
        }
        case FontColor: {
            parentNode = findItemInTree(Font);
            if (item == 0)
                item = new QTreeWidgetItem(parentNode);
            item->setText(0,tr("FontColor"));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
            item->setText(1,colorToString(value.toString()));
            break;
        }
        case BarcodeType: {
            item->setText(0,tr("BarcodeType"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            item->setText(1,BarCode::getTypeName((BarCode::BarcodeTypes)value.toInt() ));
            break;
        }
        case BarcodeFrameType: {
            item->setText(0,tr("BarcodeFrameType"));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
            item->setText(1,BarCode::getFrameTypeName((BarCode::FrameTypes)value.toInt() ));
            break;
        }
        default: {
            return;
        }
    }

    if (cont != 0) {
        bool top = cont->borderIsCheck(FrameTop);
        bool bottom = cont->borderIsCheck(FrameBottom);
        bool left = cont->borderIsCheck(FrameLeft);
        bool right = cont->borderIsCheck(FrameRight);

        if (top && bottom && left && right) {
            ui->actLineAll->setChecked(true);
            ui->actLineNo->setChecked(false);
        } else
            ui->actLineAll->setChecked(false);

        if (!top && !bottom && !left && !right) {
            ui->actLineAll->setChecked(false);
            ui->actLineNo->setChecked(true);
        } else
            ui->actLineNo->setChecked(false);
    }

    item->setData(1,Qt::UserRole,command);

    if (parentNode == 0)
        ui->treeParams->addTopLevelItem(item);
    else
        parentNode->addChild(item);
}

void MainWindow::selTree(QTreeWidgetItem *item, int) {
    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    foreach (QWidget *widget, repPage->getReportItems()) {
        RptContainer *cont = qobject_cast<RptContainer *>(widget);
        ReportBand *band = qobject_cast<ReportBand *>(widget);
        if (cont == 0 && band == 0) {
            continue;
        } else {
            if (cont != 0 && cont->itemInTree == item) {
                widgetInFocus = cont;
                cont->setVisible(true);
                if (!cont->hasFocus()) cont->setFocus();
            }
            if (band != 0 && band->itemInTree == item) {
                widgetInFocus = band;
                if (!band->hasFocus()) band->setFocus();
            }
        }
    }
}

void MainWindow::enableAdding() {
    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    ui->actAddField->setEnabled(repPage->allowField());
    ui->actAddRichText->setEnabled(repPage->allowField());
    ui->actAddPicture->setEnabled(repPage->allowField());
    ui->actAddDiagram->setEnabled(repPage->allowField());
    ui->actAddBarcode->setEnabled(repPage->allowField());
    ui->actAddDrawing->setEnabled(repPage->allowField());

    ui->actAddCrossTab->setEnabled(repPage->allowField());
    //ui->actAddCrossTabBD->setEnabled(repPage->allowField());
}

void MainWindow::addBand() {
    BandType type;
    QString bandName;
    if (sender()->objectName() == "actRepTitle") {
        type = ReportTitle;
        bandName = tr("Report title");
    }
    if (sender()->objectName() == "actReportSummary") {
        type = ReportSummary;
        bandName = tr("Report summary");
    }
    if (sender()->objectName() == "actPageHeader") {
        type = PageHeader;
        bandName = tr("Page header");
    }
    if (sender()->objectName() == "actPageFooter") {
        type = PageFooter;
        bandName = tr("Page footer");
    }
    if (sender()->objectName() == "actMasterData") {
        type = MasterData;
        bandName = tr("Master data");
    }
    if (sender()->objectName() == "actMasterFooter") {
        type = MasterFooter;
        bandName = tr("Master Footer");
    }
    if (sender()->objectName() == "actMasterHeader") {
        type = MasterHeader;
        bandName = tr("Master Header");
    }
    if (sender()->objectName() == "actDataGroupingHeader") {
        type = DataGroupHeader;
        bandName = tr("Data Group Header");
    }
    if (sender()->objectName() == "actDataGroupingFooter") {
        type = DataGroupFooter;
        bandName = tr("Data Group Footer");
    }
    ui->actSelect_tool->setChecked(true);
    QAction *action = qobject_cast<QAction *>(sender());
    action->setEnabled(false);
    ui->actSaveReport->setEnabled(true);

    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    if (repPage != 0) {
        repPage->m_addBand(bandName, type, bandMenu);
        enableAdding();
    }
}

void MainWindow::addContainer(RptContainer *container) {
    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    container->setScale(repPage->getScale());
    generateName(container);
    container->setSelected(true);
    container->setCursor(QCursor(Qt::CrossCursor));
    m_newContainer = container;
    newContMoving = true;
    setContainerConnections(container);
    ui->actSaveReport->setEnabled(true);
    ui->toolBar_3->repaint();
}

void MainWindow::addField(FieldType type) {
    TContainerField *newContainer  = new TContainerField(this,QCursor::pos());
    newContainer->setType(type, xmlDoc);
    addContainer(newContainer);
    newContainer->setFontSize(10);
    newContainer->setMenu(contMenu);
}

void MainWindow::addDraw() {
    FieldType fieldType = Text;
    if (sender()->objectName() == "actDrawRectangle") {
        fieldType = Reactangle;
    }
    if (sender()->objectName() == "actDrawRoundedRectangle") {
        fieldType = RoundedReactangle;
    }
    if (sender()->objectName() == "actDrawEllipse") {
        fieldType = Circle;
    }
    if (sender()->objectName() == "actDrawTriangle") {        
        fieldType = Triangle;
    }
    if (sender()->objectName() == "actDrawRhombus") {
        fieldType = Rhombus;
    }

    if (QtRPT::getDrawingFields().contains(fieldType)) {
        addField(fieldType);
        m_newContainer->resize(m_newContainer->width(),m_newContainer->width());
    } else {
        TContainerLine *newContainer = new TContainerLine(this, QCursor::pos());
        newContainer->setType(QtRptName::Line);
        if (sender()->objectName() == "actDrawLine2") {
            newContainer->setArrow(ArrowStart, false);
            newContainer->setArrow(ArrowEnd, false);
        }
        if (sender()->objectName() == "actDrawLine3") {
            newContainer->setArrow(ArrowStart, false);
            newContainer->setArrow(ArrowEnd, true);
        }
        if (sender()->objectName() == "actDrawLine4") {
            newContainer->setArrow(ArrowStart, true);
            newContainer->setArrow(ArrowEnd, false);
        }
        if (sender()->objectName() == "actDrawLine5") {
            newContainer->setArrow(ArrowStart, true);
            newContainer->setArrow(ArrowEnd, true);
        }
        addContainer(newContainer);
        newContMoving = true;
    }

    ui->actSelect_tool->setChecked(true);
}

void MainWindow::showPreview() {
    QtRPT *report = new QtRPT(this);
    if (fileName.isEmpty()) {
        report->loadReport(*xmlDoc);
    } else {
        report->loadReport(fileName);
    }
    report->printExec();
}

void MainWindow::addFieldText() {
    addField(Text);
}

void MainWindow::addFieldTextRich() {
    addField(TextRich);
}

void MainWindow::addFieldCrossTab() {
    addField(CrossTab);
}

void MainWindow::addBarcode() {
    addField(Barcode);
}

void MainWindow::AddPicture() {
    addField(Image);
}

void MainWindow::addDiagram() {
    addField(Diagram);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event);
    if (ui->actSaveReport->isEnabled()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Saving"),tr("The report was changed.\nSave the report?"),
                                         QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
            saveReport();
    }
 }

void MainWindow::changeZoom() {
    RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
    bool tmp = ui->actSaveReport->isEnabled();
    double scale = repPage->setPaperSize(cbZoom->currentText().replace("%","").toDouble());
    cbZoom->setEditText(QString::number(scale*100)+"%");
    ui->actSaveReport->setEnabled(tmp);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e) {
    if (e->type() == QEvent::Wheel ) {
        QWheelEvent *m = static_cast< QWheelEvent * >( e );
        RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
        if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
            qreal scale;
            if (m->delta() > 0 ) scale = 1;
            else scale = -1;
            bool tmp = ui->actSaveReport->isEnabled();
            scale = repPage->setPaperSize(scale);
            if (scale == -1) return true;

            cbZoom->setEditText(QString::number(scale*100)+"%");
            ui->actSaveReport->setEnabled(tmp);
            RptContainer *cont = qobject_cast<RptContainer *>(widgetInFocus);
            if (cont != 0 ){
                cont->setFocus();
                cont->setSelected(true);
            }
            return true;
        }
    }
    if (e->type() == QEvent::Paint) {
        if (obj->objectName() == "repWidget") {
            RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
            repPage->paintGrid();
            return true;
        }
    }
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *m = static_cast< QKeyEvent * >( e );
        if (ui->actMagnifying->isChecked()) {
            if (m->key() == Qt::Key_Shift)
                setCursor(QCursor(QPixmap(QString::fromUtf8(":/new/prefix1/images/zoom_out.png"))));
        }
    }
    if (e->type() == QEvent::KeyRelease) {
        if (ui->actMagnifying->isChecked()) {
            QKeyEvent *m = static_cast< QKeyEvent * >( e );
            if (m->key() == Qt::Key_Shift)
                setCursor(QCursor(QPixmap(QString::fromUtf8(":/new/prefix1/images/zoom_in.png"))));
        }
    }
    if (e->type() == QEvent::Enter) {
        if (obj->objectName() == "repWidget") {
            if (ui->actMagnifying->isChecked()) {
                if (QApplication::keyboardModifiers() == Qt::ShiftModifier)
                    setCursor(QCursor(QPixmap(QString::fromUtf8(":/new/prefix1/images/zoom_out.png"))));
                else
                    setCursor(QCursor(QPixmap(QString::fromUtf8(":/new/prefix1/images/zoom_in.png"))));
            }
        }
    }
    if (e->type() == QEvent::Leave) {
        if (obj->objectName() == "repWidget") {
            setCursor(Qt::ArrowCursor);
        }
    }
    if (e->type() == QMouseEvent::MouseButtonPress) {
        if (ui->actMagnifying->isChecked()) {
            qreal scale;
            if (QApplication::keyboardModifiers() != Qt::ShiftModifier) scale = 1;
            else scale = -1;
            RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
            bool tmp = ui->actSaveReport->isEnabled();
            scale = repPage->setPaperSize(scale);
            cbZoom->setEditText(QString::number(scale*100)+"%");
            ui->actSaveReport->setEnabled(tmp);
        } else {
            QToolButton *tb = qobject_cast<QToolButton*>(obj);
            if (tb != 0 && tb->actions().at(0) == ui->actFrameStyle)
                showFrameStyle(tb->pos());
        }
    }
    if (e->type() == QMouseEvent::MouseMove) {
        QMouseEvent *m = static_cast< QMouseEvent * >( e );
        if (m_newContainer && newContMoving) {
            QPoint p1 = QPoint(m->globalPos().x()-1, m->globalPos().y());
            QWidget *pw = QApplication::widgetAt(p1);            
            ReportBand *widget = qobject_cast<ReportBand *>(pw->parent());
            if (widget != 0) {
                m_newContainer->setVisible(true);
            }
            m_newContainer->allowEditing(false);
            m_newContainer->setCursor(QCursor(Qt::CrossCursor));
            //QPoint position = QPoint(m->globalPos().x(), m->globalPos().y()-19);
            QPoint p = QPoint(m->globalPos().x(), m->globalPos().y()/*-19*/);
            QPoint position = QPoint(this->mapFromGlobal(p));
            m_newContainer->move(position);
        }
    }
    if (e->type() == QMouseEvent::MouseButtonRelease) {
        QMouseEvent *m = static_cast< QMouseEvent * >( e );
        if (m_newContainer && newContMoving) {
            ui->actSelect_tool->setChecked(true);
            QPoint position = QPoint(m->globalPos().x()-1, m->globalPos().y());
            QWidget *pw = QApplication::widgetAt(position); //Dragable container
            ReportBand *repBand = qobject_cast<ReportBand *>(pw->parent());
            TContainerLine *contLine = qobject_cast<TContainerLine *>(m_newContainer);
            TContainerField *contField = qobject_cast<TContainerField *>(m_newContainer);

            if (repBand == 0) repBand = qobject_cast<ReportBand *>(pw->parent()->parent());
            if (repBand != 0) {
                QPoint p = QPoint(repBand->contWidget->mapFromGlobal(m->globalPos()));
                m_newContainer->setSelected(true);
                m_newContainer->setFocus();

                if (contLine != 0) { //Processing of the TContainerLine                    
                    contLine->setParent(repBand->contWidget);
                    contLine->move(QPoint(-10,-10));
                    contLine->installEventFilter(repBand);
                    contLine->line.setP1( QPoint(p.x(),  p.y()) );

                    repBand->newContLine = contLine;
                    contLine->cs->move(QPoint(p.x()-3,  p.y()-3));
                    contLine->cs->setVisible(true);
                }
                if (contField != 0) { //Processing TContainerField
                    m_newContainer->setParent(repBand->contWidget);
                    m_newContainer->move(QPoint(p.x()+1,  p.y()+0));
                    m_newContainer->setVisible(true);
                    m_newContainer->allowEditing(true);
                    //emit m_newContainer->inFocus(true);
                    m_newContainer->emitInFocus(true);
                    m_undoStack->push(new AddContainerCommand( getSelectedContainer() ));
                    //m_newContainer->edit();
                }
                m_newContainer->setOldGeom(m_newContainer->geometry());
                ui->actSaveReport->setEnabled(true);
            } else {
                QMessageBox::warning(this, tr("Error"), tr("This object %1 can't be a parent for %2").arg(pw->objectName()).arg(m_newContainer->objectName()), QMessageBox::Ok);
                return QWidget::eventFilter(obj,e);
            }
        }
        newContMoving = false;
    }
    return QWidget::eventFilter(obj,e);
}

//Process click on CheckBox
void MainWindow::itemChanged(QTreeWidgetItem *item, int column) {
    if (column == 1 ) {
        if (widgetInFocus == 0) return;
            QVariant v;
            Command command = (Command)item->data(1,Qt::UserRole).toInt();
            switch (command) {
                case TextWrap:
                case IgnoreRatioAspect:
                case ArrowStart:
                case ArrowEnd:
                case AutoHeight:
                case StartNewPage:
                case ShowInGroup:
                case StartNewNumeration:
                case Printing:
                case Bold:
                case Italic:
                case Underline:
                case Strikeout:
                case FrameLeft:
                case FrameRight:
                case FrameTop:
                case FrameBottom: {
                    if (item->checkState(1) == Qt::Checked) v = true;
                    else v = false;
                    break;
                }
                default: {
                    return;
                }
            }
            execButtonCommand(command,v);
    }
}

//Change param in paramTree
void MainWindow::closeEditor() {
    QTreeWidgetItem *item = ui->treeParams->currentItem();
    if (item == 0) return;
    if (widgetInFocus == 0) return;
        QVariant v;
        Command command = (Command)item->data(1,Qt::UserRole).toInt();
        switch (command) {
            case Name:
            case Left:
            case Top:            
            case Height:
            case Length:
            case AligmentH:
            case AligmentV:
            case BarcodeType:
            case BarcodeFrameType:
            case FrameWidth:
            case FontName:
            case FontSize: {
                v = item->text(1);
                ReportBand *rep = qobject_cast<ReportBand *>(widgetInFocus);
                if (command == Height && rep != 0)
                    v = item->text(1).toInt()+rep->titleHeight;
                break;
            }
            case Width: {
                v = item->text(1).toInt()+1;
                break;
            }
            default: {
                return;
            }
        }
    execButtonCommand(command,v);
}

void MainWindow::clipBoard() {
    if (sender() == ui->actCopy) {
        cloneContList->clear();
        RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
        RptContainerList contList = repPage->findChildren<RptContainer *>();
        foreach(RptContainer *cont, contList) {
            if (cont->isSelected()  ) {
                cloneContList->append(cont);
            }
        }

        ui->actPaste->setEnabled(true);
    }
    if (sender() == ui->actCut) {
        if (widgetInFocus == 0) return;
        cloneContList->clear();
        RepScrollArea *repPage = qobject_cast<RepScrollArea *>(ui->tabWidget->currentWidget());
        RptContainerList contList = repPage->findChildren<RptContainer *>();
        foreach(RptContainer *cont, contList) {
            if (cont->isSelected()  ) {
                cloneContList->append(cont);
                cont->setVisible(false);
            }
        }
        ui->actPaste->setEnabled(true);
    }
    if (sender() == ui->actPaste) {
        if (cloneContList->isEmpty()) return;

        ReportBand *band = qobject_cast<ReportBand *>(widgetInFocus);
        RptContainer *cont = qobject_cast<RptContainer *>(widgetInFocus);
        if (cont != 0)
            band = qobject_cast<ReportBand *>(cont->parentWidget()->parentWidget());
        newContList->clear();

        for (int i=0; i<cloneContList->size(); i++) {
            cloneContList->at(i)->setSelected(false);

            m_newContainer = qobject_cast<RptContainer *>(cloneContList->at(i)->clone());
            generateName(m_newContainer);
            newContList->append(m_newContainer);

            setContainerConnections(m_newContainer);
            band->newFieldTreeItem(m_newContainer);

            m_newContainer->setParent(band->contWidget);
            m_newContainer->setMenu(contMenu);
            //m_newContainer->setFocus();
            //m_newContainer->setSelected(false);
            m_newContainer->setVisible(true);
            widgetInFocus = m_newContainer;
        }
        for (int i=0; i<cloneContList->size(); i++) {
            newContList->at(i)->setSelected(true);
            newContList->at(i)->setPasted(true);
        }
        showParamState();
        ui->actSaveReport->setEnabled(true);

        m_undoStack->push(new AddContainerCommand( getSelectedContainer() ));
    }
}

void MainWindow::setContainerConnections(RptContainer *cont) {
    QObject::connect(cont, SIGNAL(delCont(QTreeWidgetItem *)), this, SLOT(delItemInTree(QTreeWidgetItem *)));
    QObject::connect(cont, SIGNAL(deleteByUser()), this, SLOT(deleteByUser()));
    QObject::connect(cont, SIGNAL(contChanged(bool)), ui->actSaveReport, SLOT(setEnabled(bool)));
    QObject::connect(cont, SIGNAL(inFocus(bool)), this, SLOT(setWidgetInFocus(bool)));
    QObject::connect(cont, SIGNAL(newGeometry(QRect, QRect)), this, SLOT(contGeomChanging(QRect, QRect)));
    QObject::connect(cont, SIGNAL(geomChanged(QRect, QRect)), this, SLOT(contGeomChanged(QRect, QRect)));
}

void MainWindow::deleteByUser() {
    m_undoStack->push(new DelContainerCommand( getSelectedContainer() ));
    foreach (RptContainer *cont, getSelectedContainer()) {
        if (cont != sender())
            cont->RptContainer::~RptContainer();
    }
}

MainWindow::~MainWindow() {
    delete xmlDoc;
    xmlDoc = 0;
    delete ui;
}

MainWindow *getMW(){
    MainWindow *mw = nullptr;
    foreach(QWidget *widget, qApp->topLevelWidgets())
        if(widget->inherits("QMainWindow")) {
            mw = qobject_cast<MainWindow *>(widget);
        }
    return mw;
}
