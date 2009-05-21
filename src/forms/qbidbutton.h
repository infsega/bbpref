#ifndef QBIDBUTTON_H
#define QBIDBUTTON_H

#include <QWidget>
#include <QPushButton>

#include "prfconst.h"


class QBidButton : public QPushButton {
 Q_OBJECT

public:
  QBidButton (eGameBid aBid, int x, int y, QWidget *parent=0);
  ~QBidButton ();

  inline eGameBid bid () const { return mBid; }

protected:
  eGameBid mBid;
};


#endif
