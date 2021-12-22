/**
  \file
  \brief    Déclaration de la classe Player.
  \author   PAPEFAB
  \date     décembre 2021
*/
#ifndef PLAYER_H
#define PLAYER_H

#include "sprite.h"

#include <QGraphicsTransform>
#include <QList>

//! \brief Classe qui gère le joueur.
//!
//! Cette classe démontre les fonctionnalités suivantes :
//!
//! - Spécialiser la classe Sprite pour y ajouter une gestion des touches du clavier.
//! - Réagir aux événements clavier pour mémoriser l'état des touches, afin de
//!   déplacer correctement le joueur.
//!   Pour que les fonctions onKeyPressed() et onKeyReleased() soient appelées lorsqu'une
//!   touche est appuyée, il faut que l'objet qui crée une instance de player se
//!   charge de les connecter aux signaux GameCore::notifyKeyPressed() et
//!   GameCore::notifyKeyReleased().
//! - Lors du tick(), déterminer la nouvelle position du joueur selon les touches
//!   du clavier qui sont appuyées.
//! - Récupérer la bounding box (boîte d'encombrement) du sprite dans le système de
//!   coordonnées global avec globalBoundingBox() et déplacement à la future position
//!   du joueur (avec translated()) pour vérifier qu'elle soit toujours
//!   incluse dans les limites de la surface de la scène.
//! - Animer le personnage lorsqu'il se déplace avec configureAnimation(),
//!   en fonction de la direction du personnage une martice de droit est appliqué avec updateRotation(),
//!   lorsque le personnage meurt la fonction deathAnimation() est lancé.
//!
class Player : public Sprite
{
    // Nécessaire de déclarer cette macro pour que la classe puisse
    // bénéficier du mécanisme de signaux/slots.
    Q_OBJECT

public:
    const bool ON_DESTROYED = false;

    Player(QGraphicsItem* pParent = nullptr);

    enum WalkingDirection {
        WALKING_RIGHT = 1,
        WALKING_LEFT = -1
    };

    void deathAnimation();

    virtual void tick(long long elapsedTimeInMilliseconds);

signals:
    void onplayerDestroyed();
    void onBonusHit();


private:
    void updatePlayerVelocity();
    void configureAnimation();
    void changeWalkingDirection();
    void setWalkingDirection(WalkingDirection newWalkingDirection);
    void updateRotation();

    WalkingDirection m_walkingDirection;
    QList<QGraphicsTransform*> m_transformsForFlip;
    int m_walkingSpeed;


    QPointF m_playerVelocity;
    bool m_keyLeftPressed;
    bool m_keyRightPressed;
    bool keyPressed = true;

    bool playerAlive = true;

    const int PLAYER_VELOCITY = 600;

public slots:
    void onKeyPressed(int key);
    void onKeyReleased(int key);
    void onDeathAnimationEnd();
};

#endif // PLAYER_H
