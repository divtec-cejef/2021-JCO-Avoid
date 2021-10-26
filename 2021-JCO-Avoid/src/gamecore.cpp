/**
  Fichier qui contient toute la logique du jeu.
  
  @author   JCO
  @date     Février 2014
 */
#include "gamecore.h"

#include <cmath>

#include <QDebug>
#include <QGraphicsBlurEffect>
#include <QGraphicsScale>
#include <QGraphicsSvgItem>
#include <QParallelAnimationGroup>
#include <QPainter>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QSettings>

#include "automaticwalkinghandler.h"
#include "blueball.h"
#include "bouncingspritehandler.h"
#include "gamescene.h"
#include "gamecanvas.h"
#include "manualwalkinghandler.h"
#include "randommovetickhandler.h"
#include "player.h"
#include "resources.h"
#include "spinningpinwheel.h"
#include "sprite.h"
#include "utilities.h"
#include "walkingman.h"
#include "playertickhandler.h"

const int SCENE_WIDTH = 1280;
const int SCENE_HEIGHT = 1080;

const QPointF BOUNCING_AREA_POS(700,300);
const int BOUNCING_AREA_SIZE = 20;

const QString HEADSHOOT_TYPE = "headshot";

const int EXPLOSION_SHEET_WIDTH = 128;
const int EXPLOSION_SHEET_HEIGHT = 128;
const int EXPLOSION_SHEET_COUNT = 14;
const int EXPLOSION_SHEET_COLUMN_COUNT = 4;

const int PLAYER_SPEED = 150;

enum SpriteDataKey {
    SPRITE_TYPE_KEY = 0,
    EARTH_ANGLE_KEY,
    EARTH_CENTER_KEY,
    EARTH_RADIUS_KEY
};

const double EARTH_SPEED = 2; // radian/seconde

//! Initialise le contrôleur de jeu.
//! \param pGameCanvas  GameCanvas pour lequel cet objet travaille.
//! \param pParent      Pointeur sur le parent (afin d'obtenir une destruction automatique de cet objet).
GameCore::GameCore(GameCanvas* pGameCanvas, QObject* pParent) : QObject(pParent) {
    
    // Mémorise l'accès au canvas (qui gère le tick et l'affichage d'une scène)
    m_pGameCanvas = pGameCanvas;
    
    // Initialisation des variables membres de la classe.
    m_footballClipRunning = false;
    
    m_pBall = nullptr;
    m_pEarth = nullptr;
    m_pPlayer = nullptr;
    
    // Créé la scène de base et indique au canvas qu'il faut l'afficher.
    m_pScene = pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_WIDTH / GameFramework::screenRatio());
    pGameCanvas->setCurrentScene(m_pScene);
    
    // Trace un rectangle blanc tout autour des limites de la scène.
    m_pScene->addRect(m_pScene->sceneRect(), QPen(Qt::white));
    
    // Créé une scène supplémentaire, pour l'instant invisible.
    createAlternativeScene();
    
    // Création des marcheurs
    setupWalkingMen();


    // Tutoriel
    pSprite = new Sprite(GameFramework::imagesPath() + "test.png");
    pSprite->addAnimationFrame(GameFramework::imagesPath() + "test2.png");
    pSprite->setPos(200, 20);

    m_pScene->addSpriteToScene(pSprite);

    m_pPlayer = pSprite;
    m_pPlayer->setTickHandler(new PlayerTickHandler);

    m_pScene->registerSpriteForTick(m_pPlayer);

    pSprite->startAnimation(500);
    // Tutoriel

    // Création de la zone de rebond
    setupBouncingArea();

    // Création de la balle
    setupBlueBall();

    // Création de la spinning wheel
    setupSpinningWheel();

    // Création de la terre tournante
    setupCirclingEarth();

    // Création de la tête à tirer
    setupHeadToShoot();
    // setupPlayer();

    // Création des murs
    Sprite* pRightWall = new Sprite(GameFramework::imagesPath() + "brique.jpg");
    pRightWall->setScale(0.5);
    pRightWall->setPos(400, 300);
    
    Sprite* pLeftWall = new Sprite(GameFramework::imagesPath() + "brique.jpg");
    pLeftWall->setScale(0.5);
    pLeftWall->setPos(-250, 300);

    m_pScene->addSpriteToScene(pRightWall);
    m_pScene->addSpriteToScene(pLeftWall);

    // Affichage d'un texte
    QGraphicsSimpleTextItem* pText = m_pScene->createText(QPointF(300,600), "L'homme qui marche", 70);
    pText->setOpacity(0.5);

    // Affichage des touches actives :
    QString texte = "<space> : change l'opacité de l'homme qui marche\n"
                    "q : supprime la balle bleue\n"
                    "x : change de scène\n"
                    "m : démarre le suivi de souris\n"
                    "flèches : déplace la balle bleue\n"
                    "clic droit : animation de balle\n"
                    "clic gauche : shoot sur la tête à tirer\n"
                    "t : crée une nouvelle tête à tirer\n"
                    "i : affiche dans la console des informations sur le sprite sous la souris\n"
                    "ctrl+shift+i : affiche des informations sur la cadence.";
    m_pScene->createText(QPointF(10,600), texte, 10);
    
    // Démarre le tick pour que les animations qui en dépendent fonctionnent correctement.
    // Attention : il est important que l'enclenchement du tick soit fait vers la fin de cette fonction,
    // sinon le temps passé jusqu'au premier tick (ElapsedTime) peut être élevé et provoquer de gros
    // déplacements, surtout si le déboggueur est démarré.
    m_pGameCanvas->startTick();
}

//! Destructeur de GameCore : efface les scènes
GameCore::~GameCore() {
    delete m_pScene;
    m_pScene = nullptr;
    
    delete m_pAlternativeScene;
    m_pAlternativeScene = nullptr;
}

//! Traite la pression d'une touche.
//! \param key Numéro de la touche (voir les constantes Qt)
//!
void GameCore::keyPressed(int key) {
    emit notifyKeyPressed(key);

    switch (key)  {
    case Qt::Key_Space:
        reduceWalkingManOpacity();
        break;
    case Qt::Key_Q:
        removeBall();
        break;
    case Qt::Key_X:
        swapScenes();
        break;
    case Qt::Key_I:
        displaySpriteDebugInfos();
        break;
    case Qt::Key_M:
        toggleSpinningWheel();
        break;
    case Qt::Key_T:
        setupHeadToShoot();
        break;

    case Qt::Key_D:
        m_pPlayer->setX(m_pPlayer->x() + 20);
        break;
    case Qt::Key_A:
        m_pPlayer->setX(m_pPlayer->x() - 20);
        break;
    case Qt::Key_F:
        m_pPlayer->setX(m_pPlayer->x() - 20);
        break;
    }
}

//! Traite le relâchement d'une touche.
//! \param key Numéro de la touche (voir les constantes Qt)
void GameCore::keyReleased(int key) {
    emit notifyKeyReleased(key);

}

//! Cadence.
//! Gère le déplacement de la Terre qui tourne en cercle.
//! \param elapsedTimeInMilliseconds  Temps écoulé depuis le dernier appel.
void GameCore::tick(long long elapsedTimeInMilliseconds) {
    Q_ASSERT(m_pEarth != nullptr);

    double radian = EARTH_SPEED * elapsedTimeInMilliseconds / 1000.;

    // Tutoriel
    /*
    float distance = PLAYER_SPEED * elapsedTimeInMilliseconds / 1000.0F * m_playerDirection;
    m_pPlayer->setX(m_pPlayer->x() + distance);

    if (m_pPlayer->right() > m_pScene->width() ||
        m_pPlayer->left() < 0){
        m_playerDirection *= -1;
    }
    */
    // Tutoriel

    double newangle = m_pEarth->data(EARTH_ANGLE_KEY).toDouble() + radian;
    double radius = m_pEarth->data(EARTH_RADIUS_KEY).toDouble();
    QPointF newPos(std::cos(newangle)*radius, std::sin(newangle)*radius);

    m_pEarth->setPos(m_pEarth->data(EARTH_CENTER_KEY).toPointF() + newPos);
    m_pEarth->setData(EARTH_ANGLE_KEY, newangle);
}

//! Met en place la démo des deux marcheurs.
//! Un marcheur (WalkingMan) hérite de Sprite afin d'y placer la responsabilité
//! de gérer les images d'animation.
//! Le déplacement se fait au moyen d'un gestionnaire de tick (SpriteTickHandler).
//! Deux gestionnaires de tick ont été créés :
//! - ManualWalkingHandler : déplacement classique en se basant sur la cadence.
//! - AutomatiqueWalkingHandler : déplacement en préprogrammant une séquence d'animation
//!   avec QPropertyAnimation.
void GameCore::setupWalkingMen() {
    WalkingMan* pAutoWalkingMan = new WalkingMan;
    pAutoWalkingMan->setPos(10, 100);
    m_pScene->addSpriteToScene(pAutoWalkingMan);
    pAutoWalkingMan->setTickHandler(new AutomaticWalkingHandler);

    WalkingMan* pManualWalkingMan = new WalkingMan;
    pManualWalkingMan ->setPos(30,300);
    m_pScene->addSpriteToScene(pManualWalkingMan );
    pManualWalkingMan->setTickHandler(new ManualWalkingHandler);
    pManualWalkingMan->registerForTick();
}

//! La souris a été déplacée.
//! Pour que cet événement soit pris en compte, la propriété MouseTracking de GameView
//! doit être enclenchée avec GameCanvas::startMouseTracking().
void GameCore::mouseMoved(QPointF newMousePosition) {
    emit notifyMouseMoved(newMousePosition);
}

//! Traite l'appui sur un bouton de la souris.
//! Fait apparaître le clip de la balle de foot qui tombe et rebondi.
void GameCore::mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonPressed(mousePosition, buttons);

    // Si le bouton droite est cliqué et que le clip n'a pas déjà démarré,
    // on démarre le clip de la balle qui tombe et rebondit sur le sol.
    if (buttons.testFlag(Qt::RightButton) && !m_footballClipRunning) {
        m_footballClipRunning = true;
        createFootBallClip(mousePosition);
    } else if (buttons.testFlag(Qt::LeftButton)) {

        // bouton gauche cliqué : on vérifie si le sprite en dessous peut être shooté.
        Sprite* pTargetSprite = m_pScene->spriteAt(mousePosition);
        if (pTargetSprite->data(SPRITE_TYPE_KEY).toString() == HEADSHOOT_TYPE) {
            // Le sprite est une tête à tirer : elle est détruite et explose
            pTargetSprite->deleteLater();
        }
    }
}

//! Traite le relâchement d'un bouton de la souris.
void GameCore::mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonReleased(mousePosition, buttons);
}

//! Construit la zone de rebond, ainsi que la balle de tennis qui va s'y déplacer.
void GameCore::setupBouncingArea() {
    
    const int BRICK_SIZE = 15;

    // Création des briques de délimitation de la zone et placement
    QPixmap smallBrick(GameFramework::imagesPath() + "brique_small.png");
    smallBrick = smallBrick.scaled(BRICK_SIZE,BRICK_SIZE);

    // Création d'une image faite d'une suite horizontale de briques
    QPixmap horizontalWall(BRICK_SIZE * BOUNCING_AREA_SIZE, BRICK_SIZE);
    QPainter painterHW(&horizontalWall);
    for (int col = 0; col < BOUNCING_AREA_SIZE; col++)
        painterHW.drawPixmap(col * BRICK_SIZE,0, smallBrick);

    // Création d'une image faite d'une suite verticale de briques
    QPixmap verticalWall(BRICK_SIZE, BRICK_SIZE * BOUNCING_AREA_SIZE);
    QPainter painterVW(&verticalWall);
    for (int col = 0; col < BOUNCING_AREA_SIZE; col++)
        painterVW.drawPixmap(0, col * BRICK_SIZE, smallBrick);

    // Ajout de 4 sprites (utilisant les murs horizontaux et verticaux) pour délimiter
    // une zone de rebond.
    m_pScene->addSpriteToScene(new Sprite(horizontalWall), BOUNCING_AREA_POS.x(), BOUNCING_AREA_POS.y() - BRICK_SIZE);
    m_pScene->addSpriteToScene(new Sprite(horizontalWall), BOUNCING_AREA_POS.x(), BOUNCING_AREA_POS.y()+ BOUNCING_AREA_SIZE * BRICK_SIZE);

    m_pScene->addSpriteToScene(new Sprite(verticalWall), BOUNCING_AREA_POS.x(), BOUNCING_AREA_POS.y());
    m_pScene->addSpriteToScene(new Sprite(verticalWall), BOUNCING_AREA_POS.x() + BOUNCING_AREA_SIZE * BRICK_SIZE - BRICK_SIZE, BOUNCING_AREA_POS.y());

    // Création de la balle de tennis qui rebondi
    Sprite* pTennisBall = new Sprite(GameFramework::imagesPath() + "tennisball.png");
    pTennisBall->setTickHandler(new BouncingSpriteHandler);
    pTennisBall->setPos(BOUNCING_AREA_POS + QPointF(25,50));
    m_pScene->addSpriteToScene(pTennisBall);
    pTennisBall->registerForTick();
}

//! Met en place la démo de la balle bleue.
void GameCore::setupBlueBall() {
    BlueBall* pBall = new BlueBall;
    pBall->setPos(0, 470);
    pBall->setZValue(1);          // Passe devant tous les autres sprites (sauf la sphère bleue)
    m_pScene->addSpriteToScene(pBall);
    pBall->registerForTick();
    connect(this, &GameCore::notifyKeyPressed, pBall, &BlueBall::onKeyPressed);
    connect(this, &GameCore::notifyKeyReleased, pBall, &BlueBall::onKeyReleased);
    m_pBall = pBall;
}

//! Met en place la roue tournante.
void GameCore::setupSpinningWheel() {
    SpinningPinwheel* pSpinningWheel = new SpinningPinwheel;
    pSpinningWheel->setVisible(false);
    pSpinningWheel->setZValue(2);          // Passe devant tous les autres sprites
    m_pScene->addSpriteToScene(pSpinningWheel);
    connect(this, &GameCore::notifyMouseMoved, pSpinningWheel, &SpinningPinwheel::onMouseMove);
    m_pSpinningWheel = pSpinningWheel;
}

//! Met en place la Terre qui tourne en cercle.
//! Démontre comment un sprite peut être mis en mouvement en le déplaçant dans
//! la fonction tick().
//! Démontre également comment mémoriser des données spécifiques à un sprite
//! en utilisant QGraphicsItem::setData() et QGraphicsItem::data().
void GameCore::setupCirclingEarth() {
    QImage earthImage(GameFramework::imagesPath() + "earth.png");

    Sprite* pEarth = new Sprite(QPixmap::fromImage(earthImage.scaledToHeight(50,
                                                                             Qt::SmoothTransformation)));
    pEarth->setData(EARTH_ANGLE_KEY, 0);
    pEarth->setData(EARTH_CENTER_KEY, QPointF(100,100));
    pEarth->setData(EARTH_RADIUS_KEY, 50);
    m_pScene->addSpriteToScene(pEarth);
    m_pEarth = pEarth;
}

//! Ajoute à la scène une *tête à tirer*.
//! Démontre comment détruire un sprite sans provoquer de crash, et comment
//! réagir à sa destruction, par exemple en affichant une explosion.
void GameCore::setupHeadToShoot() {
    Sprite* pHeadSprite = new Sprite(GameFramework::imagesPath() + "player_m1.png");
    pHeadSprite->addAnimationFrame(GameFramework::imagesPath() + "player_m2.png");
    pHeadSprite->startAnimation(100);

    // Déplace le sprite aléatoirement, en évitant les collisions.
    // Si une collision à quand-même lieu, le tickhandler se charge
    // de détruire son sprite.
    RandomMoveTickHandler* pTickHandler = new RandomMoveTickHandler;
    pTickHandler->setDestroyOnCollisionEnabled(true);

    pHeadSprite->setTickHandler(pTickHandler);
    pHeadSprite->setPos(QPointF(500,100));
    pHeadSprite->setData(SPRITE_TYPE_KEY, HEADSHOOT_TYPE); // Pour repérer ce sprite

    connect(pHeadSprite, &Sprite::destroyed, this, &GameCore::onSpriteDestroyed);
    m_pScene->addSpriteToScene(pHeadSprite);
    pHeadSprite->registerForTick();
}

/*
   void GameCore::setupPlayer() {
    Player* pPlayer = new Player;
    pPlayer->setPos(0, 470);
    pPlayer->setZValue(1);          // Passe devant tous les autres sprites
    m_pScene->addSpriteToScene(pPlayer);
    pPlayer->registerForTick();
    connect(this, &GameCore::notifyKeyPressed, pPlayer, &Player::onKeyPressed);
    connect(this, &GameCore::notifyKeyReleased, pPlayer, &Player::onKeyReleased);
    m_pPlayer = pPlayer;

}
*/

//! Construit une scène alternative qui contiendra un élément SVG.
void GameCore::createAlternativeScene() {
    // Création de la scène alternative
    m_pAlternativeScene = m_pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_WIDTH / GameFramework::screenRatio());
    
    // Création d'un élément affichange une image SVG (image verctorielle, incluant une animation)
    QGraphicsSvgItem* pSvgItem = new QGraphicsSvgItem(GameFramework::imagesPath() + "bubbles.svg");
    QRectF ItemRect = pSvgItem->boundingRect();
    
    // Ajoute l'image à la scène et la positionne au centre
    m_pAlternativeScene->addItem(pSvgItem);
    pSvgItem->setPos(m_pAlternativeScene->sceneRect().center().x() - ItemRect.width()/2, m_pAlternativeScene->sceneRect().center().y() - ItemRect.height()/2);
}

//! Réduit l'opacité du marcheur.
//! Si celle-ci devient négative, le marcheur redevient opaque.
//! Cette fonction démontre la façon de retrouver des sprites
//! d'une classe spécifique.
//! Pour que qobject_cast fonctionne, la classe dérivée
//! recherchée *doit* implémenter la macro Q_OBJECT.
void GameCore::reduceWalkingManOpacity()  {
    for(Sprite* pSprite : m_pScene->sprites()) {
        WalkingMan* pWalkingMan = qobject_cast<WalkingMan*>(pSprite);
        if (pWalkingMan) {
            double opacity = pWalkingMan->opacity() - 0.1;
            if (opacity < 0) opacity = 1;

            pWalkingMan->setOpacity(opacity);
        }
    }
}

//! Détruit la balle et affiche un texte qui indique que cela a été fait.
void GameCore::removeBall()  {
    if (!m_pBall)
        return;
    
    delete m_pBall;
    m_pBall = nullptr;
    m_pScene->createText(QPointF(40,40), "La balle est détruite");
}

//! Echange la scène affichée avec l'autre scène.
void GameCore::swapScenes() {
    if (m_pGameCanvas->currentScene() == m_pScene)
        m_pGameCanvas->setCurrentScene(m_pAlternativeScene);
    else
        m_pGameCanvas->setCurrentScene(m_pScene);
}

//! Affiche en tant qu'informations de déboguage des informations sur le sprite
//! pointé par la souris.
void GameCore::displaySpriteDebugInfos() {
    QPointF MousePos(m_pGameCanvas->currentMousePosition());
    qDebug() << "Current mouse pos : " << MousePos;
    Sprite* pHoverSprite = m_pGameCanvas->currentScene()->spriteAt(MousePos);
    if (pHoverSprite) {
        qDebug() << "Sprite class : " << pHoverSprite->metaObject()->className();
        qDebug() << "Pos : " << pHoverSprite->pos();
        qDebug() << "X, Y : " << pHoverSprite->x() << pHoverSprite->y();
    }
}

//! Créé le petit clip de la balle de football, qui démontre l'utilisation d'une animation
//! (QPropertyAnimation) avec courbes de vitesse, ainsi que le Z-Order.
//! \param position  Position où le clip doit être créé.
void GameCore::createFootBallClip(QPointF position) {
    // Créé la balle de foot
    Sprite* pFootBall = new Sprite(GameFramework::imagesPath() + "football.png");
    pFootBall->setScale(0.1);
    pFootBall->setZValue(0.2);
    
    // Créé l'herbe
    Sprite* pGrass = new Sprite(GameFramework::imagesPath() + "grass.png");
    pGrass->setScale(0.1);
    pGrass->setZValue(0.4);
    
    // Met en place le clip
    QRectF ClipRect(position, QSizeF(pGrass->width(), pGrass->width()*2));
    
    QPixmap SkyPixmap(static_cast<int>(ClipRect.width()), static_cast<int>(ClipRect.height()));
    SkyPixmap.fill(Qt::cyan);
    Sprite* pSky = new Sprite(SkyPixmap);
    pSky->setPos(ClipRect.topLeft());

    QPointF BallStartPos(ClipRect.center().x() - pFootBall->width()/2, ClipRect.top());
    
    pFootBall->setPos(BallStartPos);
    pGrass->setPos(position.x(), ClipRect.bottom() - pGrass->height()/2);
    
    m_footballClipSpriteList << pFootBall << pGrass << pSky;
    
    QPropertyAnimation* pBallFallDownAnimation = new QPropertyAnimation(pFootBall, "y"); // La propriété à animer est y (la position y)
    pBallFallDownAnimation->setDuration(1500);
    pBallFallDownAnimation->setStartValue(BallStartPos.y());
    pBallFallDownAnimation->setEndValue(BallStartPos.y() + ClipRect.height() - pFootBall->height());
    pBallFallDownAnimation->setEasingCurve(QEasingCurve::OutBounce);
    
    QParallelAnimationGroup* pAllVanishAnimation = new QParallelAnimationGroup;
    pAllVanishAnimation->addAnimation(createVanishAnimation(pFootBall));
    pAllVanishAnimation->addAnimation(createVanishAnimation(pGrass));
    pAllVanishAnimation->addAnimation(createVanishAnimation(pSky));
    
    QSequentialAnimationGroup* pClipAnimation = new QSequentialAnimationGroup;
    pClipAnimation->addAnimation(pBallFallDownAnimation);
    pClipAnimation->addAnimation(pAllVanishAnimation);
    
    m_pScene->addSpriteToScene(pFootBall);
    m_pScene->addSpriteToScene(pGrass);
    m_pScene->addSpriteToScene(pSky);

    connect(pClipAnimation, SIGNAL(finished()), this, SLOT(onClipVanishFinished()));
    
    pClipAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

//! Construit une animation qui fait disparaitre le sprite donné, en jouant
//! sur son opacité et un flou de plus en plus marqué.
QAbstractAnimation* GameCore::createVanishAnimation(Sprite* pSprite) {
    
    QGraphicsBlurEffect* pBlurEffect = new QGraphicsBlurEffect;
    pSprite->setGraphicsEffect(pBlurEffect);
    pBlurEffect->setBlurRadius(0.);
    QPropertyAnimation* pClipVanishBlurAnimation = new QPropertyAnimation(pBlurEffect, "blurRadius");
    pClipVanishBlurAnimation->setDuration(1000);
    pClipVanishBlurAnimation->setStartValue(0.);
    pClipVanishBlurAnimation->setEndValue(10);
    
    QPropertyAnimation* pClipVanishOpacityAnimation = new QPropertyAnimation(pSprite, "opacity");
    pClipVanishOpacityAnimation->setDuration(1000);
    pClipVanishOpacityAnimation->setStartValue(1.);
    pClipVanishOpacityAnimation->setEndValue(0.);
    
    QParallelAnimationGroup* pClipVanishAnimation = new QParallelAnimationGroup;
    pClipVanishAnimation->addAnimation(pClipVanishBlurAnimation);
    pClipVanishAnimation->addAnimation(pClipVanishOpacityAnimation);
    
    return pClipVanishAnimation;
}

//! Montre ou cache la roue tournant qui suit la souris.
//! Si elle est cachée, le suivi de la souris est stoppé (GameCanvas::stopMouseTracking(),
//! afin de limiter l'impact sur les performances.
//! Si elle est montrée, le suivi de la souris est enclenché (GameCanvas::startMouseTracking())
//! et un signal de suivi de souris est émis, afin de pouvoir mettra à jour la position
//! de la roue tournante par rapport à la position actuelle de la souris.
void GameCore::toggleSpinningWheel() {
    bool isSpinningWheelVisible = m_pSpinningWheel->isVisible();

    if (isSpinningWheelVisible) {
        m_pSpinningWheel->setVisible(false);
        m_pGameCanvas->stopMouseTracking();
    } else {
        m_pSpinningWheel->setVisible(true);
        m_pGameCanvas->startMouseTracking();
        emit notifyMouseMoved(m_pGameCanvas->currentMousePosition());
    }
}

//! Affiche une explosion à la position donnée.
//! Cette méthode démontre comment créer un sprite qui affiche une animation et
//! qui se détruit automatiquement dès que l'animation est terminée.
//! \param pos  Position de l'explosion.
void GameCore::createExplosion(QPointF pos) {
    Sprite* pExplosion = new Sprite;

    // Chargement de la spritesheet
    QImage spriteSheet(GameFramework::imagesPath() + "explosion.png");

    // Découpage de la spritesheet
    for (int frameIndex = 0; frameIndex < EXPLOSION_SHEET_COUNT; frameIndex++) {
        QImage sprite = spriteSheet.copy((frameIndex % EXPLOSION_SHEET_COLUMN_COUNT) * EXPLOSION_SHEET_WIDTH,
                                         (frameIndex / EXPLOSION_SHEET_COLUMN_COUNT) * EXPLOSION_SHEET_HEIGHT,
                                         EXPLOSION_SHEET_WIDTH, EXPLOSION_SHEET_HEIGHT);

        pExplosion->addAnimationFrame(QPixmap::fromImage(sprite.scaled(EXPLOSION_SHEET_WIDTH * 0.5,
                                                                       EXPLOSION_SHEET_HEIGHT * 0.5,
                                                                       Qt::IgnoreAspectRatio,
                                                                       Qt::SmoothTransformation)));
    }

    pExplosion->setAnimationSpeed(25);
    pExplosion->setPos(pos);

    // Faire en sorte qu'à la fin de l'animation, le sprite est automatiquement détruit.
    pExplosion->setEmitSignalEndOfAnimationEnabled(true);
    connect(pExplosion, &Sprite::animationFinished, pExplosion, &Sprite::deleteLater);

    // Démarrage de l'animation
    m_pScene->addSpriteToScene(pExplosion);
    pExplosion->startAnimation();

}

//! Le clip de la balle de foot qui tombe est terminé : les sprites sont effacés.
void GameCore::onClipVanishFinished() {
    qDeleteAll(m_footballClipSpriteList);
    m_footballClipSpriteList.clear();
    m_footballClipRunning = false;
}

//! Un sprite a été détruit (en l'occurrence, une *tête à tirer*). Une explosion
//! est affichée à sa position.
void GameCore::onSpriteDestroyed(QObject* pSprite) {
    Sprite* pSpriteDestroyed = static_cast<Sprite*>(pSprite);
    createExplosion(pSpriteDestroyed->pos());
}
