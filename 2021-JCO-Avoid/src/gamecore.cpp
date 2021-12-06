/**
  Fichier qui contient toute la logique du jeu.

  @author   PAPEFAB
  @date     novembre 2021
 */
#include "gamecore.h"

#include <cmath>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>

#include <QDebug>
#include <QGraphicsBlurEffect>
#include <QGraphicsScale>
#include <QGraphicsSvgItem>
#include <QParallelAnimationGroup>
#include <QPainter>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QSettings>
#include <QTime>
#include <QTimer>

#include "automaticwalkinghandler.h"
#include "player.h"
#include "bouncingspritehandler.h"
#include "gamescene.h"
#include "gamecanvas.h"
#include "manualwalkinghandler.h"
#include "randommovetickhandler.h"
#include "resources.h"
#include "spinningpinwheel.h"
#include "sprite.h"
#include "utilities.h"
#include "walkingman.h"
#include "playertickhandler.h"
#include "obstacle.h"
#include "mainfrm.h"
#include "ui_mainfrm.h"
#include "bouton.h"

const int SPAWN_INTERVAL = 170;
const int RETOURNEMENT_INTERVAL = 10000;
const int LOSE_ENDURANCE_INTERVAL = 100;
const int TIMER_BEFORE_START = 3000;
const int SCENE_WIDTH = 1280;
const int PROGRESSBAR_WIDTH = 500;
const int ACTUALISATION_TEMPS = 100;

//! Initialise le contrôleur de jeu.
//! \param pGameCanvas  GameCanvas pour lequel cet objet travaille.
//! \param pParent      Pointeur sur le parent (afin d'obtenir une destruction automatique de cet objet).
GameCore::GameCore(GameCanvas* pGameCanvas, QObject* pParent) : QObject(pParent) {

    //Démarre le timer pour les obstacles
    m_tickTimerObstacle.setSingleShot(false);
    m_tickTimerObstacle.setInterval(SPAWN_INTERVAL);
    m_tickTimerObstacle.setTimerType(Qt::PreciseTimer); // Important pour avoir un précision suffisante sous Windows
    connect(&m_tickTimerObstacle, SIGNAL(timeout()), this, SLOT(setupObstacle()));
    //Démarre le timer pour le retournement de l'écran
    m_tickTimerRetournement.setSingleShot(false);
    m_tickTimerRetournement.setInterval(RETOURNEMENT_INTERVAL);
    m_tickTimerRetournement.setTimerType(Qt::PreciseTimer); // Important pour avoir un précision suffisante sous Windows
    connect(&m_tickTimerRetournement, SIGNAL(timeout()), this, SLOT(rotateScreen()));
    //Démarre le timer pour diminution de la barre de progression
    m_tickTimerLoseEndurance.setSingleShot(false);
    m_tickTimerLoseEndurance.setInterval(LOSE_ENDURANCE_INTERVAL);
    m_tickTimerLoseEndurance.setTimerType(Qt::PreciseTimer); // Important pour avoir un précision suffisante sous Windows
    connect(&m_tickTimerLoseEndurance, SIGNAL(timeout()), this, SLOT(loseEndurance()));
    //Démarre le timer de la partie
    m_tickTimerPartie.setSingleShot(false);
    m_tickTimerPartie.setInterval(ACTUALISATION_TEMPS);
    m_tickTimerPartie.setTimerType(Qt::PreciseTimer); // Important pour avoir une précision suffisante sous Windows
    connect(&m_tickTimerPartie, SIGNAL(timeout()), this, SLOT(timerPartie()));

    // Mémorise l'accès au canvas (qui gère le tick et l'affichage d'une scène)
    m_pGameCanvas = pGameCanvas;

    // Créé la scène de base et indique au canvas qu'il faut l'afficher.
    m_pScene = pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_WIDTH / GameFramework::screenRatio());
    pGameCanvas->setCurrentScene(m_pScene);

    // Trace un rectangle blanc tout autour des limites de la scène.
    m_pScene->addRect(m_pScene->sceneRect(), QPen(Qt::white));

    std::srand(std::time(nullptr));

    // Instancier et initialiser les sprite ici :
    setupPlayer();
    setupProgressBar();
    setupTimerPartie();
    startSpawnObstacleTimer();
    startRetournerEcran();

    // Démarre le tick pour que les animations qui en dépendent fonctionnent correctement.
    // Attention : il est important que l'enclenchement du tick soit fait vers la fin de cette fonction,
    // sinon le temps passé jusqu'au premier tick (ElapsedTime) peut être élevé et provoquer de gros
    // déplacements, surtout si le déboggueur est démarré

    //Démarre la partie et son tick
    m_tickTimerPartie.start();
    m_pGameCanvas->startTick();
}

//! Destructeur de GameCore : efface les scènes
GameCore::~GameCore() {
    delete m_pScene;
    m_pScene = nullptr;


}
/**
 * @brief GameCore::setupObstacle
 * Met en place un obtsacle
 */
void GameCore::setupObstacle(){
    nbGen=rand()%LARGEUR_MAX+LARGEUR_MINIMUM;   //génère un chiffre aléatoire entre 1 et 1150
    nombreObstacle++;

    if(nombreObstacle == APPARITION_BONUS){
        setupBonus();
        nombreObstacle = 0;
    }else{
        pObstacle = new Sprite(GameFramework::imagesPath() + "obstacle.png");
        pObstacle->setPos(nbGen,0);
        pObstacle->setScale(0.1);

        // Déplace le sprite aléatoirement, en évitant les collisions.
        // Si une collision à quand-même lieu, le tickhandler se charge
        // de détruire son sprite.
        RandomMoveTickHandler* pTickHandler = new RandomMoveTickHandler;
        pTickHandler->setDestroyOnCollisionEnabled(true);
        pObstacle->setTickHandler(pTickHandler);
        //connect(&pTickHandler,SIGNAL(&RandomMoveTickHandler::onplayerDestroyed()),this,SLOT(GameCore::playerDestroyed()));
        m_pScene->addSpriteToScene(pObstacle);

        //connect(pTickHandler,&RandomMoveTickHandler::onplayerDestroyed() ,this, GameCore::playerDestroyed());

        pObstacle->registerForTick();
    }

}
//!
//! \brief GameCore::setupBonus
//! met en place le bonus
//!
void GameCore::setupBonus(){
    pBonus = new Sprite(GameFramework::imagesPath() + "Bonus.png");
    pBonus->setPos(nbGen,0);
    pBonus->setScale(0.25);
    pBonus->setData(0, "bonus");


    //déplace l'obstacle vers le bas
    //détruit l'obstacle lorsqu'il touche le sol
    RandomMoveTickHandler* pbTickHandler = new RandomMoveTickHandler;
    pbTickHandler->setDestroyOnCollisionEnabled(true);
    pBonus->setTickHandler(pbTickHandler);

    m_pScene->addSpriteToScene(pBonus);
    pBonus->registerForTick();
}

/**
 * Met en place le timer et son text
 * @brief GameCore::setupTimerPartie
 */
void GameCore::setupTimerPartie(){
    m_textTimer = "0";
    m_objetTimer = m_pScene->createText(QPointF(0,0), m_textTimer, 70);
    m_objetTimer->setOpacity(0.5);
}

void GameCore::setupResultat(){
    m_objetResultat = m_pScene->createText(QPointF((m_pScene->width() / 2) - 315,m_pScene->height() / 2), m_textResultat, 70);
    m_objetTimer->setOpacity(0.5);
    m_objetTimer->setPos(m_pScene->width() / 2 + 300,m_pScene->height() / 2);
    m_objetTimer->setOpacity(1);
    m_objetTimer->setZValue(1);
}

/**
 * Met en place la progression de la barre
 * @brief GameCore::setupProgressBar
 */
void GameCore::setupProgressBar() {
    double screenXCenter = m_pScene->width() / 2;
    m_ProgressBarBorder = m_pScene->addRect(QRectF(screenXCenter - PROGRESSBAR_WIDTH/2, 10,PROGRESSBAR_WIDTH, 50), QPen(Qt::white), QBrush(Qt::black));
    m_ProgressBarBorder->setZValue(1);
    m_ProgressBarFill = m_pScene->addRect(QRectF(screenXCenter - PROGRESSBAR_WIDTH/2, 10,PROGRESSBAR_WIDTH, 50), QPen(Qt::transparent), QBrush(Qt::green));
    m_ProgressBarFill->setZValue(1);

    connect(m_pPlayer, SIGNAL(onBonusHit()), this, SLOT(upProgressBar()));
    m_tickTimerLoseEndurance.start();
}

/**
 * Met la barre a 100%
 * @brief GameCore::fillProgressBar
 */
void GameCore::upProgressBar() {
    setProgressBarPercentage(getProgressBarPercentage() + 30);
}

/**
 * Baisse la progression de la barre
 * @brief GameCore::loseEndurance
 */
void GameCore::loseEndurance() {
    setProgressBarPercentage(getProgressBarPercentage()-0.5);
}

/**
  Met à jour la progression de la barre
 * @brief GameCore::updateProgressBar
 */
void GameCore::updateProgressBar() {
    double newWidth = PROGRESSBAR_WIDTH / 100 * progressBarPercentage;
    double screenXCenter = m_pScene->width() / 2;
    m_ProgressBarFill->setRect(screenXCenter - PROGRESSBAR_WIDTH/2, 10, newWidth, 50);

    if (progressBarPercentage > 100/3*2) {
        m_ProgressBarFill->setBrush(QBrush(Qt::green));
    } else if (progressBarPercentage > 100 / 3) {
        m_ProgressBarFill->setBrush(QBrush(Qt::yellow));
    } else {
        m_ProgressBarFill->setBrush(QBrush(Qt::red));
    }

    if (progressBarPercentage <= 0) {
        stopGame();
    }
}
/**
 * Stop la progresson de la barre si elle dépasse 100 ou 0
 * @brief GameCore::setProgressBarPercentage
 * @param percentage
 */
void GameCore::setProgressBarPercentage(double percentage) {
    if (percentage > 100 ) percentage = 100;
    else if (percentage < 0) percentage = 0;
    progressBarPercentage = percentage;
}
/**
 * donne la progression de la bar d'avancement
 * @brief GameCore::getProgressBarPercentage
 * @return la progression de la bar
 */
double GameCore::getProgressBarPercentage() {
    return progressBarPercentage;
}


/**
 * Configuration timer spawn les obstacles
 * @brief GameCore::startSpawnObstacleTimer
 * @param tickInterval
 */
void GameCore::startSpawnObstacleTimer(int tickInterval)  {

    if (tickInterval != KEEP_PREVIOUS_TICK_INTERVAL)
        m_tickTimerObstacle.setInterval(tickInterval);

    m_keepTicking = true;
    m_lastUpdateTime.start();
    m_tickTimerObstacle.start();
}

/**
 * Configuration timer pour démarrer le timer de la partie
 * @brief GameCore::startRetournerEcran
 * @param tickInterval
 */
void GameCore::startTimerPartie(int tickInterval){
    if (tickInterval != KEEP_PREVIOUS_TICK_INTERVAL)
        m_tickTimerObstacle.setInterval(tickInterval);

    m_keepTicking = true;
    m_lastUpdateTime.start();
    m_tickTimerPartie.start();
}

/**
 * Configuration timer pour retourner l'écran
 * @brief GameCore::startRetournerEcran
 * @param tickInterval
 */
void GameCore::startRetournerEcran(int tickInterval)  {

    if (tickInterval != KEEP_PREVIOUS_TICK_INTERVAL)
        m_tickTimerRetournement.setInterval(tickInterval);

    m_keepTicking = true;
    m_lastUpdateTime.start();
    m_tickTimerRetournement.start();
}

/**
 * Configuration timer avant le recommencement du jeu
 * @brief GameCore::startGameTimer
 * @param tickInterval
 */
void GameCore::startGameTimer(int tickInterval)  {

    if (tickInterval != KEEP_PREVIOUS_TICK_INTERVAL)
        m_tickTimerRestartGame.setInterval(tickInterval);

    m_keepTicking = true;
    m_lastUpdateTime.start();
    m_tickTimerRestartGame.start();
}

/**
 * Actualise le text timer pour lui ajouter 0,1
 * @brief GameCore::timerPartie
 */
void GameCore::timerPartie(){
    tempsPartie += 0.1;
    m_objetTimer->setText(QString::number(tempsPartie));

}

//! Met en place le joueur
void GameCore::setupPlayer() {
    pPlayer = new Player;
    int ajustementHauteur = 80;
    pPlayer->setPos(m_pScene->width()/2, m_pScene->height() - ajustementHauteur);
    pPlayer->setZValue(1);      // Passe devant tous les autres sprites (sauf la sphère bleue)
    pPlayer->setScale(0.4);
    m_pScene->addSpriteToScene(pPlayer);
    pPlayer->registerForTick();
    connect(this, &GameCore::notifyKeyPressed, pPlayer, &Player::onKeyPressed);
    connect(this, &GameCore::notifyKeyReleased, pPlayer, &Player::onKeyReleased);
    connect(pPlayer,&Player::onplayerDestroyed, this, &GameCore::stopGame);
    m_pPlayer = pPlayer;
}

/**
  Arrete le jeux lors de la mort du personnage
 * @brief GameCore::stopGame
 */
void GameCore::stopGame(){
    keyboardDisabled = true;
    pPlayer->stopAnimation();
    pPlayer->deathAnimation();

    m_tickTimerPartie.stop();
    m_tickTimerObstacle.stop();
    m_tickTimerRetournement.stop();

    setProgressBarPercentage(getProgressBarPercentage() - 100);
    setupResultat();
    deleteAllSprite();
    setupBouton();

}


void GameCore::setupBouton(){
    pBouton = new Bouton;

    pBouton->setPos(m_pScene->width() / 2, m_pScene->height() / 2 + 120);
    pBouton->setScale(0.1);
    m_pScene->addSpriteToScene(pBouton);
    }

void GameCore::deleteAllSprite(){
    //supprime tous les srpites de la scène

    for (Sprite* sprite : m_pScene->sprites()) {
        if(sprite != m_pPlayer){
            m_pScene->removeSpriteFromScene(sprite);
        }
    }
}

/**
  Relance le jeux
 * @brief GameCore::restartGame
 */
void GameCore::restartGame(){
    m_pScene->removeSpriteFromScene(m_pPlayer);
    m_pPlayer->deleteLater();
    m_pGameCanvas->stopTick();
    m_pScene->removeSpriteFromScene(pBouton);
    m_pScene->removeItem(m_objetTimer);
    m_pScene->removeItem(m_objetResultat);

    m_pGameCanvas->startTick();
    setupProgressBar();
    setupPlayer();
    setupTimerPartie();
    startSpawnObstacleTimer();
    startRetournerEcran();
    keyboardDisabled = false;
}

/**
  Retourne l'écran
 * @brief GameCore::rotateScreen
 */
void GameCore::rotateScreen() {

    m_pGameCanvas->rotateView();
    m_objetTimer->setTransformOriginPoint(m_objetTimer->boundingRect().center());

    if(m_objetTimer->rotation() > 1){
        m_objetTimer->setRotation(0);
        m_objetTimer->setX(0);
    } else{
        m_objetTimer->setRotation(180);
        m_objetTimer->setX(50);
    }
}

//! Traite la pression d'une touche.
//! \param key Numéro de la touche (voir les constantes Qt)
//!
void GameCore::keyPressed(int key) {
    if (!keyboardDisabled)
        emit notifyKeyPressed(key);
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
    updateProgressBar();
}


//! La souris a été déplacée.
//! Pour que cet événement soit pris en compte, la propriété MouseTracking de GameView
//! doit être enclenchée avec GameCanvas::startMouseTracking().
void GameCore::mouseMoved(QPointF newMousePosition) {
    emit notifyMouseMoved(newMousePosition);
}

//! Traite l'appui sur un bouton de la souris.
void GameCore::mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonPressed(mousePosition, buttons);

    // Si le bouton droite est cliqué et que le clip n'a pas déjà démarré,
    // on démarre le clip de la balle qui tombe et rebondit sur le sol.
    if (buttons.testFlag(Qt::LeftButton)) {
        // bouton gauche cliqué : on vérifie si le sprite en dessous peut être shooté.
        Sprite* pTargetSprite = m_pScene->spriteAt(mousePosition);
        if (pTargetSprite == pBouton) {
            // Le sprite est une tête à tirer : elle est détruite et explose
            restartGame();
        }
    }
}



//! Traite le relâchement d'un bouton de la souris.
void GameCore::mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonReleased(mousePosition, buttons);
}



