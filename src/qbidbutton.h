#ifndef QBIDBUTTON_H
#define QBIDBUTTON_H

#include <QWidget>
#include <QPushButton>

#include "prfconst.h"


class QBidButton : public QPushButton {
 Q_OBJECT

public:
  QBidButton (tGameBid aBid, int x, int y, QWidget *parent=0);
  ~QBidButton ();

  inline tGameBid bid () const { return mBid; }

protected:
  tGameBid mBid;
};


#endif
