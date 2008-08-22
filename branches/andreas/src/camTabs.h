/*************************************************************
** License: GPL.  
** Copyright (C) 2006 Mark Pictor
*************************************************************/

#ifndef CAMTABS_H
#define CAMTABS_H

#include <qobject.h>
#include <qlistview.h>
#include <qvbox.h>
#include <qhgroupbox.h>
#include <qtoolbutton.h>
#include <qtabwidget.h>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

class myListView : public QListView
{
public:
    myListView ( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    void setSorting ( int column, bool ascending = TRUE );
};

class camTabs : public QObject
{
    Q_OBJECT
public:
    camTabs(QVBox *lf);
    ~camTabs();
    void init();
    
public slots:
    void clickedAddButton();
    void clickedDelButton();
    void clickedPropButton();
    void slotAddFace(uint);
signals:
    void setProgress(int p=-1, char* status="Ready");

protected:
    enum {FinishTl,SpecialTl,RoughTl} toolType;
    static const char * toolTypeName[];
    enum {FinishOp,Special_OtherOp,IntermediateOp,RoughOp} opType;
    static const char * opTypeName[];
	
private:
    QVBox *frame;

    QTabWidget *tabWidget;
      myListView *featureTab;
        QListViewItem *faceFeatHeader;
        QListViewItem *edgeFeatHeader;
      myListView *opTab;
        QListViewItem *roughOpHeader;
        QListViewItem *intermediateOpHeader;
        QListViewItem *specialOpHeader;
        QListViewItem *finishOpHeader;
      myListView *toolTab;
        QListViewItem *finishTlHeader;
        QListViewItem *specTlHeader;
        QListViewItem *roughTlHeader;

    QHGroupBox *tabControl;
    QToolButton *buttonAdd;
    QToolButton *buttonDel;
    QToolButton *buttonProp;
 
    void setupTabsAndTrees();
    void addOperation();
    void addTool();
    void addFeature(TopoDS_Edge E);
    //void addFeature(TopoDS_Face F);
};


#endif //CAMTABS_H
