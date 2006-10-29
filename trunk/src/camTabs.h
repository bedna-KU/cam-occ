
#ifndef CAMTABS_H
#define CAMTABS_H

class camTabs : public QObject
{
public:
    camTabs();
    ~camTabs();
    void init();
protected:
    enum {FinishTl,SpecialTl,RoughTl} toolType;
    const char* toolTypeName[3];
    enum {FinishOp,Special_OtherOp,IntermediateOp,RoughOp} opType;
    const char* opTypeName[4];
	
private:
    QVBox *frame;

    QTabWidget *tabWidget;
      QListView *featureTab;
        QListViewItem *faceFeatHeader;
        QListViewItem *edgeFeatHeader;
      QListView *opTab;
        QListViewItem *roughOpHeader;
        QListViewItem *intermediateOpHeader;
        QListViewItem *specialOpHeader;
        QListViewItem *finishOpHeader;
      QListView *toolTab;
        QListViewItem *finishTlHeader;
        QListViewItem *specTlHeader;
        QListViewItem *roughTlHeader;

    QHGroupBox *tabControl;
      QToolButton *buttonAdd;
      QToolButton *buttonDelete;
      QToolButton *buttonProperties;
 
};

#endif //CAMTABS_H
