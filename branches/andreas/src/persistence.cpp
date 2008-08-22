
#include "persistence.h"

persist::persist(selectPatch *Patchtest)
{
    Patch=Patchtest;
        //connect (Patch, SIGNAL(sendAddFace()), this, SLOT(slotTest()));
	  
}
persist::~persist()
{
}

void persist::slotTest()
{
      puts("slotActivateFaceTransfer, not yet implemented");	
    QMessageBox::warning( 0, "Warning", "autoSelect::slotActivateFaceTransfer - not yet implemented");
    return;
}
