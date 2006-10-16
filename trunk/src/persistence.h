/*************************************************************
** License: GPL.
** Copyright (C) 2006 Mark Pictor
*************************************************************/

//persistence.h

/***************************************************************************
** class name: Persistence
** Manages saving and loading data.
**
**  things that will need saving:
**    -faces selected on the model
**    -parameters in use for each face
**    -description for each tool
**    -filename for solid model
**    -timestamp (when solid model was last modified)
**       -use date -r <file>
**    -sha1 to fingerprint the solid model
**       -use sha1sum <file>
****************************************************************************/

#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "pathAlgo.h"


//#include <syscall.h>
#include <time.h>



class persist : public QObject
{
    Q_OBJECT

public:
    persist();
    ~persist();
protected:
    time_t last_saved;
    time_t model_modified;
    char sha1OfModel[41];  //sha1sum for model's file

};

#endif //PERSISTENCE_H
