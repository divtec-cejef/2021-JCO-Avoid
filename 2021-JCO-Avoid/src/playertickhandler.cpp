#include "playertickhandler.h"

#include "sprite.h"
#include "gamescene.h"

const int PLAYER_SPEED = 200;

PlayerTickHandler::PlayerTickHandler(Sprite* pParentSprite) : SpriteTickHandler(pParentSprite)
{

}

void PlayerTickHandler::init() {
    m_initialPos = m_pParentSprite->pos();
}

void PlayerTickHandler::tick(long long elapsedTimeInMilliseconds) {
    double distance = m_playerDirection * PLAYER_SPEED * elapsedTimeInMilliseconds / 1000.;

    QPointF spriteMovement(distance, 0.0);

    // Détermine la prochaine position du sprite
    QRectF nextSpriteRect = m_pParentSprite->globalBoundingBox().translated(spriteMovement);

    // Récupère tous les sprites de la scène que toucherait ce sprite à sa prochaine position
    auto collidingSprites = m_pParentSprite->parentScene()->collidingSprites(nextSpriteRect);

    // Supprimer le sprite lui-même, qui collisionne toujours avec sa boundingbox
    collidingSprites.removeAll(m_pParentSprite);

    bool collision = !collidingSprites.isEmpty();

    // Si les bords sont atteints ou s’il y a collision : le sprite change de direction
    if (!m_pParentSprite->parentScene()->isInsideScene(nextSpriteRect) ||
        collision)
        m_playerDirection *= -1;
    else {
        m_pParentSprite->setPos(m_pParentSprite->pos() + spriteMovement);
    }
}
