/**
  \file
  \brief    Déclaration de la classe BlueBall.
  \author   JCO
  \date     septembre 2018
*/
#include "obstacle.h"
#include "gamescene.h"
#include "resources.h"

#include <QDebug>

const int BALL_VELOCITY = 200; // pixels par seconde

//! Construit et initialise une balle bleue.
//! \param pParent  Objet propiétaire de cet objet.
Obstacle::Obstacle(QGraphicsItem* pParent) : Sprite(GameFramework::imagesPath() + "obastacle.png", pParent) {
    configureAnimation();
    m_keyUpPressed    = false;
    m_keyDownPressed  = false;
    m_keyLeftPressed  = false;
    m_keyRightPressed = false;
    m_ballVelocity = QPointF(0,0);

}

//! Cadence.
//! \param elapsedTimeInMilliseconds  Temps écoulé depuis le dernier appel.
void Obstacle::tick(long long elapsedTimeInMilliseconds) {


}

//! Une touche a été appuyée.
//! \param key Code de la touche appuyée.
void Obstacle::onKeyPressed(int key) {
    switch (key)  {
    //case Qt::Key_Up:    m_keyUpPressed    = true;  updateBallVelocity(); break;
    //case Qt::Key_Down:  m_keyDownPressed  = true;  updateBallVelocity(); break;
    case Qt::Key_Right: m_keyRightPressed = true;  updateBallVelocity(); break;
    case Qt::Key_Left:  m_keyLeftPressed  = true;  updateBallVelocity(); break;
    }
}

//! Une touche a été relâchée.
//! \param key Code de la touche relâchée.
void Obstacle::onKeyReleased(int key) {
    switch (key)  {
    //case Qt::Key_Up:    m_keyUpPressed    = false;  updateBallVelocity(); break;
    //case Qt::Key_Down:  m_keyDownPressed  = false;  updateBallVelocity(); break;
    case Qt::Key_Right: m_keyRightPressed = false;  updateBallVelocity(); break;
    case Qt::Key_Left:  m_keyLeftPressed  = false;  updateBallVelocity(); break;
    }

}

//! Met à jour le vecteur de vitesse de la balle en fonction des touches appuyées.
void Obstacle::updateBallVelocity()  {
    int XVelocity = 0;
    int YVelocity = 0;
    if (m_keyUpPressed)    YVelocity = -BALL_VELOCITY;
    if (m_keyDownPressed)  YVelocity = BALL_VELOCITY;
    if (m_keyRightPressed) XVelocity = BALL_VELOCITY;
    if (m_keyLeftPressed)  XVelocity = -BALL_VELOCITY;

    m_ballVelocity = QPoint(XVelocity, YVelocity);
}

void Obstacle::configureAnimation() {
    for (int FrameNumber = 1; FrameNumber <= 7; ++FrameNumber)  {
        this->addAnimationFrame(QString(GameFramework::imagesPath() + "perso%1.png").arg(FrameNumber));
    }
    this->setAnimationSpeed(100);  // Passe à la prochaine image de la marche toutes les 100 ms
    this->startAnimation();
}


