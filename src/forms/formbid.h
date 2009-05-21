#ifndef FORMBID_H
#define FORMBID_H

#include <QDialog>
#include <QList>
#include <QPushButton>
#include <QWidget>

#include "prfconst.h"
#include "qbidbutton.h"


class FormBid : public QDialog  {
 Q_OBJECT

public:
  FormBid (QWidget *parent=0);
  ~FormBid ();

  void EnableAll (void);
  void DisalbeAll (void);
  void DisableLessThan (tGameBid);
  void DisalbeGames (tGameBid);
  void EnableGames (tGameBid);

public slots:
  void onBidClick (); // universal handler
  void slotPushButtonClick86 ();
  void slotPushButtonClickPass ();
  void slotPushButtonClickVist ();
  void slotGetBackSnos ();
  void slotShowBullet ();

protected:
  void initDialog ();
  QList<QPushButton *> buttonList ();
  QList<QBidButton *> bidButtonList ();

  void showEvent (QShowEvent *event);

public:
  tGameBid _GamesType;
  QPushButton *bgetback;
  QPushButton *showbullet;

protected:
  QPushButton *b86;
  QPushButton *bpass;
  QPushButton *bvist;
};

extern FormBid *formBid;


#endif
