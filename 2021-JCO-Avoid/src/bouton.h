#ifndef BOUTON_H
#define BOUTON_H

#include "sprite.h"


class Bouton : public Sprite
{

    Q_OBJECT

public:

    Bouton(QGraphicsItem* pParent = nullptr);

    void onRealesed();

    void onButtonPressed();



};

#endif // BOUTON_H
