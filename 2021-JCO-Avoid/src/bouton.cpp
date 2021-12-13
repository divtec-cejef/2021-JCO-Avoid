#include "bouton.h"

#include "resources.h"
#include "gamescene.h"
#include <QDebug>



Bouton::Bouton(QGraphicsItem* pParent) : Sprite(GameFramework::imagesPath() + "replay.png", pParent)
{

}


//! La souris a été déplacée.
//! Pour que cet événement soit pris en compte, la propriété MouseTracking de GameView
//! doit être enclenchée avec GameCanvas::startMouseTracking().
void Bouton::mouseMoved(QPointF newMousePosition) {
    emit notifyMouseMoved(newMousePosition);
}

//! Traite le relâchement d'un bouton de la souris.
void Bouton::mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonReleased(mousePosition, buttons);
}


//! Traite l'appui sur un bouton de la souris.
void Bouton::mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonPressed(mousePosition, buttons);

    // Si le bouton droite est cliqué et que le clip n'a pas déjà démarré,
    // on démarre le clip de la balle qui tombe et rebondit sur le sol.
    if (buttons.testFlag(Qt::LeftButton)) {

    }
}








