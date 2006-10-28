
#ifndef TREEVIEW_H
#define TREEVIEW_H

class treeView : public QObject
{
public:
    treeView();
    ~treeView();
    void init();
protected:
    enum {Finish,Special,Rough} toolType;
    static char* toolTypeName[3] = {"Finish","Special","Rough"}; 
    enum {Finish,Special_Other,Intermediate,Rough} opType;
    static char* opTypeName[4] = {"Finish","Special/Other","Intermediate","Rough"};
	
private:
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

#endif //TREEVIEW_H
