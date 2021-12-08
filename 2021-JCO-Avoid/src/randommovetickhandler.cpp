/**
  \file
  \brief    Définition de la classe RandomMoveTickHandler.
  \author   JCO
  \date     août 2020
*/
#include "randommovetickhandler.h"

#include <cstdlib>
#include <ctime>
#include <QtMath>
#include <QPointF>
#include <QRectF>
#include <QTime>
#include <QTimer>
#include <time.h>

#include <chrono>
#include <thread>

#include "sprite.h"
#include "gamescene.h"
#include "player.h"
#include "gamecore.h"
#include "resources.h"
bool RandomMoveTickHandler::s_seeded = false;
qreal RandomMoveTickHandler::s_vitesseObtsacle = 7;

const double DEFAULT_SPRITE_VELOCITY = 250.0;
const int MOVE_MINIMAL_DURATION = 400;
const int MOVE_MAXIMAL_DURATION = 2000;
const int MAXIMAL_ANGLE_CHANGE = 120; // Changement de direction maximal autorisé
const int AUGMENTATION_VITESSE_INTERVAL = 1000;

//! Constructeur.
//! \param pParentSprite Sprite dont le déplacement doit être géré.
RandomMoveTickHandler::RandomMoveTickHandler(Sprite* pParentSprite) : SpriteTickHandler (pParentSprite)
{
    // Initialisation, si pas encore fait, du générateur pseudo-aléatoire
    if (!s_seeded) {
        std::srand(std::time(nullptr));
        s_seeded = true;
    }

    //Démarre le timer pour l'augmentation de la vitesse des obstacles
    m_tickTimerAugmentationObstacle.setSingleShot(false);
    m_tickTimerAugmentationObstacle.setInterval(AUGMENTATION_VITESSE_INTERVAL);
    m_tickTimerAugmentationObstacle.setTimerType(Qt::PreciseTimer); // Important pour avoir un précision suffisante sous Windows
    connect(&m_tickTimerAugmentationObstacle, SIGNAL(timeout()), this, SLOT(augmentationVitesseObstacle()));
    m_tickTimerAugmentationObstacle.start();
    m_pPlayer = new Player;

    m_spriteVelocity = DEFAULT_SPRITE_VELOCITY;
}


//! Constructeur.
//! \param ignoreCollision      Indique si le déplacement n'a pas à éviter les collisions.
//! \param destroyOnCollision   Indique si le sprite géré doit être détruit en cas de collision.
//! \param pParentSprite Sprite dont le déplacement doit être géré.
RandomMoveTickHandler::RandomMoveTickHandler(bool ignoreCollision, bool destroyOnCollision, Sprite* pParentSprite) : RandomMoveTickHandler(pParentSprite) {
    m_ignoreCollision = ignoreCollision;
    m_destroyOnCollision = destroyOnCollision;
}

//! Initialisation du gestionnaire.
void RandomMoveTickHandler::init() {

}

//! Cadence : détermine le mouvement que fait le sprite durant le temps écoulé.
void RandomMoveTickHandler::tick(long long elapsedTimeInMilliseconds) {
    // Création d'un vecteur de déplacement du sprite.
    QPointF spriteMovement(0, s_vitesseObtsacle);
    // Détermine la prochaine position du sprite
    QRectF nextSpriteRect = m_pParentSprite->globalBoundingBox().translated(spriteMovement);
    // Récupère tous les sprites de la scène que toucherait ce sprite à sa prochaine position
    auto collidingSprites = m_pParentSprite->parentScene()->collidingSprites(nextSpriteRect);
    // Supprimer le sprite lui-même, qui collisionne toujours avec sa boundingbox
    collidingSprites.removeAll(m_pParentSprite);
    bool collision = !collidingSprites.isEmpty();
    // Si la prochaine position du sprite n'est pas comprise au sein de la scène,
    // ou s’il y a collision, le sprite n’est pas déplacé et change de direction
    if (!m_pParentSprite->parentScene()->isInsideScene(nextSpriteRect))
        m_pParentSprite->deleteLater();
    else
        // S'il n'y a pas de collision et que le sprite ne sort pas de la scène, on le déplace
        // (en lui appliquant le vecteur de déplacement)
        m_pParentSprite->setPos(m_pParentSprite->pos() + spriteMovement);


    // Si la destruction sur collision est activée, on vérifie si le sprite
    // est en collision. Si c'est le cas, on le détruit.
    if (m_destroyOnCollision && !m_pParentSprite->parentScene()->collidingSprites(m_pParentSprite).isEmpty()) {
        // Il ne faut en aucun cas effacer immédiatement le sprite avec delete, car l'effacement immédiat du
        // sprite provoque la destruction immédiate de ce handler, alors qu'il est en cours d'exécution. Cela
        // provoquerait alors un crash.
        // deleteLater() permet d'efface le sprite plus tard, une fois que le code qui le concerne a été
        // complètement exécuté.

        //m_pParentSprite->parentScene()->sprites().first()->deleteLater();
        //emit onplayerDestroyed();
    }
}

/**
 * Configuration timer pour gérer la vitesse de spawn des obtsacles
 * @brief GameCore::startSpawnObstacleTimer
 * @param tickInterval
 */
void RandomMoveTickHandler::startTimerVitesseObstacle(int tickInterval)  {

    if (tickInterval != KEEP_PREVIOUS_TICK_INTERVAL)
        m_tickTimerAugmentationObstacle.setInterval(tickInterval);

    m_keepTicking = true;
    m_lastUpdateTime.start();
    m_tickTimerAugmentationObstacle.start();
}

void RandomMoveTickHandler::augmentationVitesseObstacle(){
    qDebug() << s_vitesseObtsacle;
    s_vitesseObtsacle+=0.1;
}

//! Charge les différentes images qui composent l'animation du marcheur et
//! les ajoute à ce sprite.
void RandomMoveTickHandler::configureAnimation() {
    for (int FrameNumber = 1; FrameNumber <= 15; ++FrameNumber)  {
        m_pParentSprite->parentScene()->sprites().first()->addAnimationFrame(QString(GameFramework::imagesPath() + "mort/pngegg_%1.png").arg(FrameNumber));
    }
    m_pParentSprite->parentScene()->sprites().first()->setAnimationSpeed(100);  // Passe à la prochaine image de la marche toutes les 100 ms
    m_pParentSprite->parentScene()->sprites().first()->startAnimation();
}
/**
void RandomMoveTickHandler::onplayerDestroyed(){
}
**/
//! Change la vitesse de déplacement du sprite.
//! \param velocityInPixelPerSecond  Nouvelle vitesse de déplacement du sprite.
void RandomMoveTickHandler::setSpriteVelocity(double velocityInPixelPerSecond) {
    m_spriteVelocity = velocityInPixelPerSecond;
}

//! Détermine si ce sprite ignore les collisions lors de son déplacement.
//! Si c'est le cas, il ne change pas sa trajectoire en cas de collision.
//! Sinon, en cas de collision, il change sa trajectoire.
//! \param ignoreCollision Indique si les collisions doivent être ignorées.
void RandomMoveTickHandler::setIgnoreCollisionEnabled(bool ignoreCollision) {
    m_ignoreCollision = ignoreCollision;
}

//! \return un booléen qui indique si ce sprite ignore les collisions.
bool RandomMoveTickHandler::isIgnoreCollisionEnable() const {
    return m_ignoreCollision;
}

//! Indique si le sprite piloté par ce gestionnaire doit être détruit en cas de collision.
//! \param destroyOnCollision  Indique si une collision provoque la destruction du sprite.
void RandomMoveTickHandler::setDestroyOnCollisionEnabled(bool destroyOnCollision) {
    m_destroyOnCollision = destroyOnCollision;
}

//! \return un booléen qui indique si une collision provoque la destruction du sprite
//! piloté par ce gestionnaire.
bool RandomMoveTickHandler::isDestroyedOnCollision() const {
    return m_destroyOnCollision;
}

//! Détermine les paramètres du prochain mouvement aléatoire que va réaliser ce sprite :
//! - Durée du mouvement
//! - Angle de déplacement
void RandomMoveTickHandler::initNextMove() {
    m_moveDuration = std::rand() % (MOVE_MAXIMAL_DURATION - MOVE_MINIMAL_DURATION) + MOVE_MINIMAL_DURATION;

    int newAngleDelta = std::rand() % MAXIMAL_ANGLE_CHANGE - (MAXIMAL_ANGLE_CHANGE / 2);
    m_moveAngle = m_moveAngle + qDegreesToRadians(static_cast<double>(newAngleDelta));
}
