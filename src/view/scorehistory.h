#ifndef SCOREHISTORY_H
#define SCOREHISTORY_H

#include <QtGui/QDialog>

class PrefModel;
class ScoreHistoryDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ScoreHistoryDialog(PrefModel *model, QWidget *parent = 0, Qt::WindowFlags f = 0);


signals:

public slots:

};

#endif // SCOREHISTORY_H
