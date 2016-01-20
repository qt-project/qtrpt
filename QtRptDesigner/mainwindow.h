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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGlobal>
#if QT_VERSION >= 0x50000
    #include <QtWidgets>
#else
    #include <QtGui>
#endif
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QDomDocument>
#include <QAction>
#include "aboutDlg.h"
#include "RepScrollArea.h"
#include "SettingDlg.h"
#include <XYZ_DownloadManager.h>
#include "SqlDesigner.h"
#include "TContainerLine.h"
#include "UndoCommands.h"

namespace Ui {
    class MainWindow;
}

class EditorDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    EditorDelegate(QObject *parent);
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const {
        Q_UNUSED(option);
        Q_UNUSED(index);
        return QSize(50,20);
    }
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

protected:
    void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

private slots:
    void commitAndCloseEditor();
    void editorClose_(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);

signals:
    void editorClose(QItemDelegate *item);
    void btnClicked();
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setContainerConnections(RptContainer *cont);
    QUndoStack *m_undoStack;
    QWidget *widgetInFocus;

protected:
    bool eventFilter(QObject *obj, QEvent *e);
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QListWidget *listFrameStyle;
    QDomDocument *xmlDoc;    
    QTreeWidgetItem *rootItem;    
    RptContainer *m_newContainer;
    RptContainer *cloneCont;
    RptContainerList *cloneContList;
    RptContainerList *newContList;
    QFontComboBox *cbFontName;
    QComboBox *cbZoom;
    QComboBox *cbFontSize;
    QComboBox *cbFrameWidth;
    bool newContMoving;
    QIcon icon;
    QString fileName;
    QMenu *contMenu;
    QMenu *bandMenu;

    QAction *actRepTitle;
    QAction *actReportSummary;
    QAction *actPageHeader;
    QAction *actPageFooter;
    QAction *actMasterData;
    QAction *actMasterFooter;
    QAction *actMasterHeader;
    QAction *separatorAct;
    QAction *actDataGroupingHeader;
    QAction *actDataGroupingFooter;

    SqlDesigner *sqlDesigner;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];

    bool setXMLProperty(QDomElement *repElem, QWidget *widget);
    void selectItemInTree(QTreeWidgetItem *item);
    void showParamState();
    Command getCommand(QObject *obj);
    void execButtonCommand(Command command, QVariant value);
    void processCommand(Command command, QVariant value, QWidget *widget);
    QTreeWidgetItem *findItemInTree(Command command);
    void generateName(RptContainer *cont);
    void setParamTree(Command command, QVariant value = 0, bool child = false);
    void updateRecentFileActions();
    void setCurrentFile(const QString &fileName);
    QDomElement getDataSourceElement(QDomNode n);
    void addContainer(RptContainer *container);
    void shiftToDelta(QRect oldRect, QRect newRect, QObject *sender, bool change);
    void enableAdding();
    QList<RptContainer *> getSelectedContainer();

private slots:
    void showAbout();
    void openFile();
    void newReport();
    void saveReport();
    void showPageSetting();
    void clickOnTBtn();
    void addBand();
    void addFieldText();
    void addFieldTextRich();
    void addFieldCrossTab();
    void addField(FieldType type);
    void AddPicture();
    void addDiagram();
    void addDraw();
    void addBarcode();
    void setWidgetInFocus(bool inFocus);
    void delItemInTree(QTreeWidgetItem *);
    void selTree(QTreeWidgetItem *item, int);
    void itemChanged(QTreeWidgetItem *item, int column);
    void closeEditor();
    void changeTextFont();
    void contGeomChanging(QRect oldRect, QRect newRect);
    void contGeomChanged(QRect oldRect, QRect newRect);
    void setGroupingField();
    void clipBoard();
    void chooseColor();
    void showFrameStyle(QPoint pos);
    void setFrameStyle(QListWidgetItem * item);
    void changeFrameWidth();
    void bandResing(int value);
    void reportPageChanged(int index);
    void newReportPage();
    void deleteReportPage();
    void alignFields();
    void openRecentFile();
    void closeProgram();
    void showSetting();
    void changeZoom();
    void openDBGroupProperty();
    void checkUpdates();
    void showPreview();
    void showDataSource();
    void undo();
    void redo();
    void openReadme();
    void deleteByUser();
};

MainWindow *getMW();

#endif // MAINWINDOW_H
