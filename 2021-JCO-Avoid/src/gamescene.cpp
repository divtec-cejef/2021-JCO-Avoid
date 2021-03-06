/**
  \file
  \brief    Définition de la classe GameScene.
  \author   PAPEFAB
  \date     décembre 2021
*/
#include "gamescene.h"

#include <cstdlib>
#include <QApplication>
#include <QBrush>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPen>

#include "gamecore.h"
#include "resources.h"
#include "sprite.h"

//! Construit la scène de jeu avec une taille par défaut et un fond noir.
//! \param pParent  Objet propriétaire de cette scène.
GameScene::GameScene(QObject* pParent) : QGraphicsScene(pParent) {
    init();
}

//! Construit la scène de jeu avec la taille donnée et un fond noir.
//! \param rSceneRect   Taille de la scène.
//! \param pParent      Objet propriétaire de cette scène.
GameScene::GameScene(const QRectF& rSceneRect, QObject* pParent) : QGraphicsScene(rSceneRect, pParent) {
    init();
}

//! Construit la scène de jeu avec la taille donnée et un fond noir.
//! \param X        Position X de la scène.
//! \param Y        Position Y de la scène.
//! \param width    Largeur de la scène.
//! \param height   Hauteur de la scène.
//! \param pParent  Objet propriétaire de cette scène.
GameScene::GameScene(qreal X, qreal Y, qreal width, qreal height, QObject* pParent) : QGraphicsScene(X, Y, width, height, pParent) {
    init();
}

//! Destruction de la scène.
GameScene::~GameScene()  {
    delete m_pBackgroundImage;
    m_pBackgroundImage = nullptr;
}

//! Ajoute le sprite à la scène.
//! La scène prend possession du sprite et se chargera de l'effacer.
//! \param pSprite Pointeur sur le sprite à ajouter à la scène.
void GameScene::addSpriteToScene(Sprite* pSprite)
{
    Q_ASSERT(pSprite != nullptr);

    this->addItem(pSprite);
    pSprite->setParentScene(this);

    connect(pSprite, &Sprite::destroyed, this, &GameScene::onSpriteDestroyed);

    emit spriteAddedToScene(pSprite);
}

//! Retire le sprite de la scène.
//! La scène n'est plus propriétaire du sprite et ne se chargera pas de l'effacer.
//! \param pSprite Pointeur sur le sprite à enlever de la scène.
void GameScene::removeSpriteFromScene(Sprite* pSprite)
{
    removeItem(pSprite);

    disconnect(pSprite, &Sprite::destroyed, this, &GameScene::onSpriteDestroyed);

    m_registeredForTickSpriteList.removeAll(pSprite);

    emit spriteRemovedFromScene(pSprite);

}

//! Construit la liste de tous les sprites en collision avec le rectangle donné
//! en paramètre.
//! Si la scène contient de nombreux sprites, cette méthode peut prendre du temps.
//! \param rRect Rectangle avec lequel il faut tester les collisions.
//! \return une liste de sprites en collision.
QList<Sprite*> GameScene::collidingSprites(const QRectF &rRect) const  {
    QList<Sprite*> collidingSpriteList;
    for(Sprite* pSprite : sprites())  {
        QRectF globalBBox = pSprite->globalBoundingBox();
        if (globalBBox.intersects(rRect)) {
            collidingSpriteList << pSprite;
        }
    }
    return collidingSpriteList;
}


//!
//! \return la liste des sprites de cette scène (y compris ceux qui ne sont pas visibles).
//!
QList<Sprite*> GameScene::sprites() const  {
    QList<Sprite*> spriteList;
    auto allItems = this->items();
    for(QGraphicsItem* pItem : allItems) {
        if (pItem->type() == Sprite::SpriteItemType)
            spriteList << static_cast<Sprite*>(pItem);
    }
    return spriteList;
}


//! Récupère le sprite visible le plus en avant se trouvant à la position donnée.
//! \return un pointeur sur le sprite trouvé, ou null si aucun sprite ne se trouve à cette position.
Sprite* GameScene::spriteAt(const QPointF& rPosition) const {
    QGraphicsItem* pTopMostVisibleItem = this->itemAt(rPosition, QTransform());
    if (pTopMostVisibleItem && pTopMostVisibleItem->type() == Sprite::SpriteItemType)
        return static_cast<Sprite*>(pTopMostVisibleItem);

    return nullptr;
}

//!
//! Affiche un texte sur la scène.
//! Si ce texte n'est pas détruit manuellement, il le sera automatiquement
//! lorsque la scène sera détruite.
//! \param initialPosition  Position initiale du texte.
//! \param rText            Texte à afficher.
//! \param size             Taille (en pixels) du texte.
//! \param color            Couleur du texte.
//! \return un pointeur sur l'élément graphique textuel.
//!
QGraphicsSimpleTextItem* GameScene::createText(QPointF initialPosition, const QString& rText, int size, QColor color) {
    QGraphicsSimpleTextItem* pText = this->addSimpleText(rText);
    pText->setPos(initialPosition);
    QFont textFont = pText->font();
    textFont.setPixelSize(size);
    pText->setFont(textFont);
    pText->setBrush(QBrush(color));
    return pText;
}

//! Défini l'image de fond à utiliser pour cette scène.
void GameScene::setBackgroundImage(const QImage& rImage)  {
    if (m_pBackgroundImage)
        delete m_pBackgroundImage;
    m_pBackgroundImage = new QImage(rImage);
}

//! Le sprite donné sera informé du tick.
//! \param pSprite Sprite qui s'enregistre pour le tick.
void GameScene::registerSpriteForTick(Sprite* pSprite) {
    m_registeredForTickSpriteList.append(pSprite);
}

//! Le sprite donné se va plus être informé du tick.
//! \param pSprite Sprite qui démissionne du tick.
void GameScene::unregisterSpriteFromTick(Sprite* pSprite) {
    m_registeredForTickSpriteList.removeAll(pSprite);
}

//! Vérifie si le rectangle donné fait complètement partie de la scène.
//! \param rRect Rectangle à vérifier
//! \return un booléen à vrai si le rectangle fait partie de la scène, sinon
//! faux.
bool GameScene::isInsideScene(const QRectF& rRect) const {
    return sceneRect().contains(rRect);
}

//! Cadence.
//! \param elapsedTimeInMilliseconds  Temps écoulé depuis le tick précédent.
void GameScene::tick(long long elapsedTimeInMilliseconds) {
    auto spriteListCopy = m_registeredForTickSpriteList; // On travaille sur une copie au cas où
    // la liste originale serait modifiée
    // lors de l'appel de tick auprès d'un sprite.
    for(Sprite* pSprite : spriteListCopy) {
        pSprite->tick(elapsedTimeInMilliseconds);
    }
}


//! Dessine le fond d'écran de la scène.
//! Si une image à été définie avec setBackgroundImage(), celle-ci est affichée.
//! Une autre méthode permet de définir une image de fond :
//! QGraphicsScene::setBackgroundBrush(QBrush(QPixmap(...))).
//! Cette deuxième méthode affiche cependant l'image comme un motif de tuile.
//! \see setBackgroundImage()
void GameScene::drawBackground(QPainter* pPainter, const QRectF& rRect)  {
    QGraphicsScene::drawBackground(pPainter, rRect);
    if (m_pBackgroundImage)
        pPainter->drawImage(0,0, *m_pBackgroundImage);
}


//! Initialise la scène
void GameScene::init() {
    m_pBackgroundImage = nullptr;

    this->setBackgroundBrush(QBrush(Qt::black));
    //setBackgroundImage(QImage(GameFramework::imagesPath() + "space.jpg"));

}

//! Retire de la liste des sprite le sprite qui va être détruit.
void GameScene::onSpriteDestroyed(QObject* pSprite) {
    Sprite* pSpriteDestroyed = static_cast<Sprite*>(pSprite);
    m_registeredForTickSpriteList.removeAll(pSpriteDestroyed);
}
