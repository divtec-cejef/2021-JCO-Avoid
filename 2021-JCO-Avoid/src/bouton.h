#ifndef BOUTON_H
#define BOUTON_H

#include "sprite.h"


class Bouton : public Sprite
{

    Q_OBJECT

public:
    Bouton();

    Bouton(QGraphicsItem* pParent = nullptr);

    onRealesed();


};

#endif // BOUTON_H
