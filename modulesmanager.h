#ifndef MODULESMANAGER_H
#define MODULESMANAGER_H

#include <QDialog>

namespace Ui {
class ModulesManager;
}

class ModulesManager : public QDialog
{
    Q_OBJECT

public:
    explicit ModulesManager(QWidget *parent = nullptr);
    ~ModulesManager();
signals:
    void sigReLoadModule(QString path);
public slots:
    void slotRefressTableWidget();
private:
    void AddModule();
    void DeleteModule();
    void InitTableWidget();
    void ReloadModule();

private:
    Ui::ModulesManager *ui;
    QStringList m_strMoudulePathName;
protected:
    virtual void closeEvent(QCloseEvent *e);

};

#endif // MODULESMANAGER_H
