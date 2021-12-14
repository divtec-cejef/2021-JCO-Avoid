#ifndef BOUTON_H
#define BOUTON_H

#include "sprite.h"


class Bouton : public Sprite
{

    Q_OBJECT

public:

    Bouton(QGraphicsItem* pParent = nullptr);

    void mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons);
    void mouseMoved(QPointF newMousePosition);
    void mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons);

    int btnEtat = false;

signals:
    void notifyMouseMoved(QPointF newMousePosition);
    void notifyMouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons);
    void notifyMouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons);
};

#endif // BOUTON_H
