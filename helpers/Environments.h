#ifndef ENVIRONMENTS_H
#define ENVIRONMENTS_H
#include <QString>

namespace AppEnv{
const QString APP_HOMEDIR_NAME =  ".crudMahasiswa";
const QString APP_DATABASE_NAME = "dataMahasiswa.db";
}

struct StudentsDataStruct{
    int id;
    QString nama;
    QString npm;
    QString kelas;
};

#endif // ENVIRONMENTS_H
