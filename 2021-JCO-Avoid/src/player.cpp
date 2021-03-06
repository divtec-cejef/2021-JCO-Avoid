/**
  \file
  \brief    Déclaration de la classe Player.
  \author   PAPEFAB
  \date     décembre 2021
*/
#include "player.h"
#include "gamescene.h"
#include "resources.h"

#include <QDebug>

//! Construit et initialise le player.
//! \param pParent  Objet propiétaire de cet objet.
Player::Player(QGraphicsItem* pParent) : Sprite(GameFramework::imagesPath() + "tile19.png", pParent) {
    m_keyLeftPressed  = false;
    m_keyRightPressed = false;
    m_playerVelocity = QPointF(0,0);

    connect(this, SIGNAL(animationFinished()), this, SLOT(onDeathAnimationEnd()));
}

//! Cadence.
//! \param elapsedTimeInMilliseconds  Temps écoulé depuis le dernier appel.
void Player::tick(long long elapsedTimeInMilliseconds) {

    // Calcul de la distance parcourue par le player, selon sa vitesse et le temps écoulé.
    QPointF playerDistance = elapsedTimeInMilliseconds * m_playerVelocity / 1000;
    // Positionne la bounding box du player à sa prochaine position.
    QRectF nextRect = this->globalBoundingBox().translated(playerDistance);
    //Récupère tous les sprites de la scène
    auto collidingSprites = this->parentScene()->collidingSprites(nextRect);
    // Supprimer le sprite lui-même, qui collisionne toujours avec sa boundingbox
    collidingSprites.removeAll(this);

    //trie les bonus des obstacles
    Sprite* bonus = nullptr;
    for (Sprite* sprite : collidingSprites) {
        if (sprite->data(0).toString() == "bonus") {
            bonus = sprite;
        }
    }

    bool collision = !collidingSprites.isEmpty();

    // Si la prochaine position reste dans les limites de la scène, le player
    // y est positionnée. Sinon, il reste sur place.
    if (this->parentScene()->isInsideScene(nextRect)) {
        this->setPos(this->pos() + playerDistance);
    }

    //Si le player collisionne avec un bonus, alors le bonus est supprimé
    if (bonus != nullptr) {
        bonus->deleteLater();
        emit onBonusHit();
        //Sinon le player meurt
    } else if(collision && playerAlive){
        playerAlive = false;
        emit onplayerDestroyed();
    }
}

//! Une touche a été appuyée.
//! \param key Code de la touche appuyée.
void Player::onKeyPressed(int key) {
    switch (key)  {
    case Qt::Key_Right: m_keyRightPressed = true;  configureAnimation(); break;
    case Qt::Key_Left:  m_keyLeftPressed  = true;  configureAnimation(); break;
    }

    updatePlayerVelocity();
    updateRotation();
}

//! Une touche a été relâchée.
//! \param key Code de la touche relâchée.
void Player::onKeyReleased(int key) {

    if(keyPressed){
        switch (key)  {
        case Qt::Key_Right: m_keyRightPressed = false;  this->stopAnimation(); this->setCurrentAnimationFrame(0); break;
        case Qt::Key_Left:  m_keyLeftPressed  = false;  this->stopAnimation(); this->setCurrentAnimationFrame(0); break;
        }
    }
    updatePlayerVelocity();

}

//! Met à jour le vecteur de vitesse de la balle en fonction des touches appuyées.
void Player::updatePlayerVelocity()  {
    int XVelocity = 0;
    int YVelocity = 0;
    if (m_keyRightPressed) XVelocity = PLAYER_VELOCITY;
    if (m_keyLeftPressed)  XVelocity = -PLAYER_VELOCITY;

    XVelocity < 0 ? setWalkingDirection(WALKING_LEFT) : setWalkingDirection(WALKING_RIGHT);

    m_playerVelocity = QPoint(XVelocity, YVelocity);
}

//! \return la direction de marche.
void Player::changeWalkingDirection() {
    setWalkingDirection(m_walkingDirection == WALKING_RIGHT ? WALKING_LEFT : WALKING_RIGHT);
}

//! Change la direction de marche.
//! \param newWalkingDirection  Nouvelle direction de marche.
void Player::setWalkingDirection(WalkingDirection newWalkingDirection) {
    if (m_walkingDirection != newWalkingDirection) {
        m_walkingDirection = newWalkingDirection;
    }
}

//! Matrice de transformation pour faire miroir du player
//!
void Player::updateRotation() {
    // Préparation d'une matrice de transformation
    QGraphicsScale* pHorizontalFlip = new QGraphicsScale(this);
    pHorizontalFlip->setOrigin(QVector3D(this->width()/2,0,0));
    pHorizontalFlip->setXScale(m_walkingDirection);

    QList<QGraphicsTransform*> transformations;
    transformations << pHorizontalFlip;

    this->setTransformations(transformations);
}

//! Animation du player qui court
//!
void Player::configureAnimation() {
    for (int FrameNumber = 1; FrameNumber <= 7; ++FrameNumber)  {
        this->addAnimationFrame(QString(GameFramework::imagesPath() + "personnage/tile%1.png").arg(FrameNumber));
    }
    this->setAnimationSpeed(100);  // Passe à la prochaine image de la marche toutes les 100 ms
    this->startAnimation();
}

//! Animation du personnage qui meurt
//!
void Player::deathAnimation() {
    keyPressed = false;
    this->stopAnimation();
    this->clearAnimationFrames();
    for (int FrameNumber = 0; FrameNumber <= 24; FrameNumber++)  {
        this->addAnimationFrame(QString(GameFramework::imagesPath() + "explosion/tile%0.png").arg(FrameNumber));
    }
    setEmitSignalEndOfAnimationEnabled(true);
    this->setAnimationSpeed(50);
    this->startAnimation();

}

//! Stop l'animation du personnage
//!
void Player::onDeathAnimationEnd() {

    this->stopAnimation();
}




