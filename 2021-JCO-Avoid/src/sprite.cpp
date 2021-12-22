/**
  \file
  \brief    Définition de la classe Sprite.
  \author   PAPEFAB
  \date     décembre 2021
*/
#include "sprite.h"

#include <QDebug>
#include <QPainter>

#include "gamescene.h"
#include "spritetickhandler.h"

int Sprite::s_spriteCount = 0;

const int NO_CURRENT_FRAME = -1;

//! Construit un sprite et l'initialise.
//! Le sprite n'a pas d'apparence particulière et n'affichera rien
//! tant qu'une image ne lui sera pas fournie avec addAnimationFrame().
//! \param pParent  Pointeur sur le parent (afin d'obtenir une destruction automatique de cet objet).
Sprite::Sprite(QGraphicsItem* pParent) : QGraphicsPixmapItem(pParent) {
    init();
}

//! Construit un sprite et l'initialise.
//! Le sprite utilisera l'image fournie pour son apparence.
//! \param rPixmap   Image à utiliser pour l'apparence du sprite.
//! \param pParent   Pointeur sur le parent (afin d'obtenir une destruction automatique de cet objet).
Sprite::Sprite(const QPixmap& rPixmap, QGraphicsItem* pParent) : QGraphicsPixmapItem(pParent) {
    init();
    addAnimationFrame(rPixmap);
}

//! Destructeur.
Sprite::~Sprite() {
#ifdef DEBUG_SPRITE_COUNT
    s_spriteCount--;
    displaySpriteCount();
#endif
    if (m_pTickHandler != nullptr) {
        delete m_pTickHandler;
        m_pTickHandler = nullptr;
    }
}

//! Ajoute une image au cycle d'animation.
//! \param rPixmap  Image à ajouter.
void Sprite::addAnimationFrame(const QPixmap& rPixmap) {
    m_animationList[m_currentAnimationIndex] << rPixmap;
    onNextAnimationFrame();
}

//! Change l'image d'animation présentée par le sprite.
//! L'image doit avoir été au préalable ajoutée aux images d'animation avec addAnimationFrame().
//! \param frameIndex   Index (à partir de zéro) de l'image à utiliser.
void Sprite::setCurrentAnimationFrame(int frameIndex) {
    if (m_animationList[m_currentAnimationIndex].isEmpty())
        return;

    if (frameIndex < 0 || frameIndex >= m_animationList[m_currentAnimationIndex].count())
        frameIndex = 0;

    m_currentAnimationFrame = frameIndex;
    setPixmap(m_animationList[m_currentAnimationIndex][frameIndex]);
}

//! \return l'index de l'image d'animation actuellement affichée.
int Sprite::currentAnimationFrame() const {
    return m_currentAnimationFrame;
}

//! Efface toutes les images du sprite.
//! Le sprite devient invisible.
void Sprite::clearAnimationFrames() {
    m_animationList[m_currentAnimationIndex].clear();
    m_currentAnimationFrame = NO_CURRENT_FRAME;
    setPixmap(QPixmap()); // On enlève l'image du sprite afin d'éviter toute confusion.
}

//! Change la vitesse d'animation.
//! \param frameDuration   Durée d'une image en millisecondes.
void Sprite::setAnimationSpeed(int frameDuration) {
    if (frameDuration <= 0)
        m_animationTimer.stop();
    else if (frameDuration != m_frameDuration) {
        m_frameDuration = frameDuration;
        m_animationTimer.setInterval(frameDuration);
    }
}

//! Arrête l'animation.
void Sprite::stopAnimation() {
    m_animationTimer.stop();
}

//! Démarre l'animation.
//! La vitesse d'animation utilisée est celle qui a été
//! spécifiée avec setAnimationSpeed().
void Sprite::startAnimation() {
    m_currentAnimationFrame = NO_CURRENT_FRAME;
    onNextAnimationFrame();
    m_animationTimer.start();
}

//! Démarre l'animation à la vitesse donnée.
//! Cette vitesse donnée remplace la précédente.
//! \param frameDuration   Durée d'une image en millisecondes.
void Sprite::startAnimation(int frameDuration)  {
    setAnimationSpeed(frameDuration);
    startAnimation();
}

//! Ajoute une animation supplémentaire à ce sprite.
//! \see clearAnimations()
//! \see setActiveAnimation()
//! \see animationCount()
void Sprite::addAnimation() {
    m_animationList.append(QList<QPixmap>());

}


//! Efface toutes les animations d'un sprite mais en préserve une vide.
void Sprite::clearAnimations() {
    m_animationList.clear();
    addAnimation();
    m_currentAnimationIndex = 0;
}

//! Choisi si le signal animationFinished() doit être émis chaque fois que l'animation
//! est terminée.
//! \param enabled   Indique si le signal doit être émis (true) ou non (false).
void Sprite::setEmitSignalEndOfAnimationEnabled(bool enabled) {
    m_emitSignalEOA = enabled;
}

//! \return un booléen qui indique si le signal animationFinished() est émis
//! chaque fois que l'animation est terminée.
bool Sprite::isEmitSignalEndOfAnimationEnabled() const {
    return m_emitSignalEOA;
}

//! Mémorise la scène à laquelle appartient ce sprite.
//! \param pScene  Scène à laquelle appartient ce sprite.
void Sprite::setParentScene(GameScene* pScene) {
    m_pParentScene = pScene;
}

#if defined(DEBUG_BBOX) || defined(DEBUG_SHAPE)
//! Dessine le sprite, avec sa boundingbox qui l'entoure.
void Sprite::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) {
    QGraphicsPixmapItem::paint(pPainter, pOption, pWidget);
#ifdef DEBUG_BBOX
    pPainter->setPen(Qt::white);
    pPainter->drawRect(this->boundingRect());
#endif
#ifdef DEBUG_SHAPE
    pPainter->setPen(Qt::red);
    pPainter->drawPath(this->shape());
#endif
}
#endif

//! Enregistre ce sprite auprès de la scène afin qu'il soit informé de la
//! cadence et que la fonction tick() soit appelée en cadence.
void Sprite::registerForTick() {
    Q_ASSERT(m_pParentScene != nullptr);
    m_pParentScene->registerSpriteForTick(this);
}

//! Désolidarise ce sprite de la cadence.
void Sprite::unregisterFromTick() {
    Q_ASSERT(m_pParentScene != nullptr);
    m_pParentScene->unregisterSpriteFromTick(this);
}

//! Cadence.
//! Pour qu'un sprite soit cadencé, il doit s'enregister avec la méthode
//! registerForTick().
//! Si un gestionnaire de cadence est installé sur ce sprite, celui-ci est appelé.
//! \param elapsedTimeInMilliseconds  Temps écoulé depuis le tick précédent.
void Sprite::tick(long long elapsedTimeInMilliseconds) {
    if (m_pTickHandler)
        m_pTickHandler->tick(elapsedTimeInMilliseconds);
}

//! Attribue à ce sprite un gestionnaire de tick.
//! L'ancien gestionnaire est détruit.
//! Ce sprite prend la propriété du gestionnaire.
//! \param pTickHandler  Pointeur sur le nouveau gestionnaire.
void Sprite::setTickHandler(SpriteTickHandler* pTickHandler) {
    removeTickHandler();
    m_pTickHandler = pTickHandler;
    pTickHandler->setParentSprite(this);
    pTickHandler->init();
}

//! \return un pointeur sur le gestionnaire de tick actuel.
SpriteTickHandler*Sprite::tickHandler() const {
    return m_pTickHandler;
}

//! Supprime (et efface) le gestionnaire de tick actuel.
void Sprite::removeTickHandler() {
    if (m_pTickHandler != nullptr) {
        delete m_pTickHandler;
        m_pTickHandler = nullptr;
    }
}

//! \return un pointeur sur la scène à laquelle appartient ce sprite.
GameScene* Sprite::parentScene() const {
    return m_pParentScene;
}

//! Initialise le sprite.
void Sprite::init() {
    m_pTickHandler = nullptr;
    m_pParentScene = nullptr;
    m_emitSignalEOA = false;
    m_frameDuration = 0;
    m_currentAnimationFrame = NO_CURRENT_FRAME;
    m_currentAnimationIndex = 0;

    // Par défaut, le sprite possède une (unique) liste d'animation
    addAnimation();

    m_customType = -1;
    connect(&m_animationTimer, SIGNAL(timeout()), this, SLOT(onNextAnimationFrame()));

#ifdef DEBUG_SPRITE_COUNT
    s_spriteCount++;
    displaySpriteCount();
#endif
}

//! Affiche l'image suivante de l'animation.
//! Si la dernière image est affichée, l'animation reprend au début et,
//! selon la configuration, le signal animationFinished() est émis.
void Sprite::onNextAnimationFrame() {
    if (m_animationList[m_currentAnimationIndex].isEmpty()) {
        m_currentAnimationFrame = NO_CURRENT_FRAME;
        return;
    }

    int PreviousAnimationFrame = m_currentAnimationFrame;
    ++m_currentAnimationFrame;
    if (m_currentAnimationFrame >= m_animationList[m_currentAnimationIndex].count()) {
        if (m_emitSignalEOA)
            emit animationFinished();
        m_currentAnimationFrame = 0;
    }
    if (PreviousAnimationFrame != m_currentAnimationFrame) {
        setPixmap(m_animationList[m_currentAnimationIndex][m_currentAnimationFrame]);
        update();
    }
}
