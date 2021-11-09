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


const int SPAWN_INTERVAL = 170;
const int RETOURNEMENT_INTERVAL = 10000;
const int SCENE_WIDTH = 1280;

//! Initialise le contrôleur de jeu.
//! \param pGameCanvas  GameCanvas pour lequel cet objet travaille.
//! \param pParent      Pointeur sur le parent (afin d'obtenir une destruction automatique de cet objet).
GameCore::GameCore(GameCanvas* pGameCanvas, QObject* pParent) : QObject(pParent) {
    nombreObstacle = 0;
    m_tickTimerObstacle.setSingleShot(false);
    m_tickTimerObstacle.setInterval(SPAWN_INTERVAL);
    m_tickTimerObstacle.setTimerType(Qt::PreciseTimer); // Important pour avoir un précision suffisante sous Windows
    connect(&m_tickTimerObstacle, SIGNAL(timeout()), this, SLOT(setupObstacle()));

    m_tickTimerRetournement.setSingleShot(false);
    m_tickTimerRetournement.setInterval(RETOURNEMENT_INTERVAL);
    m_tickTimerRetournement.setTimerType(Qt::PreciseTimer); // Important pour avoir un précision suffisante sous Windows

    connect(&m_tickTimerRetournement, SIGNAL(timeout()), this, SLOT(rotateScreen()));

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
    startSpawnObstacleTimer();
    startRetournerEcran();

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
}

void GameCore::setupObstacle(){
    nbGen=rand()%LARGEUR_MAX+LARGEUR_MINIMUM;    //génère un chiffre aléatoire entre 1 et 1150
    nombreObstacle++;

    if(nombreObstacle == APPARITION_BONUS){
        setupBonus();
        nombreObstacle = 0;
    }else{
        Sprite* pObstacle = new Sprite(GameFramework::imagesPath() + "obstacle.png");
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

void GameCore::setupBonus(){
    Sprite* pBonus = new Sprite(GameFramework::imagesPath() + "Bonus1.png");
    pBonus->setPos(nbGen,0);
    pBonus->setScale(0.25);

    // Déplace le sprite aléatoirement, en évitant les collisions.
    // Si une collision à quand-même lieu, le tickhandler se charge
    // de détruire son sprite.GameFramework::imagesPath() + "Bonus1.png"
    RandomMoveTickHandler* pbTickHandler = new RandomMoveTickHandler;
    pbTickHandler->setDestroyOnCollisionEnabled(true);
    pBonus->setTickHandler(pbTickHandler);

    m_pScene->addSpriteToScene(pBonus);
    pBonus->registerForTick();
}

//!
//! Démarre la génération d'un tick sur une base de temps régulière,
//! donnée en paramètre.
//! \param tickInterval  Intervalle de temps (en millisecondes) entre chaque tick. Si cette valeur est
//! inférieure à zéro, l'intervalle de temps précédent est utilisé.
//!
void GameCore::startSpawnObstacleTimer(int tickInterval)  {

    if (tickInterval != KEEP_PREVIOUS_TICK_INTERVAL)
        m_tickTimerObstacle.setInterval(tickInterval);

    m_keepTicking = true;
    m_lastUpdateTime.start();
    m_tickTimerObstacle.start();
}

void GameCore::startRetournerEcran(int tickInterval)  {

    if (tickInterval != KEEP_PREVIOUS_TICK_INTERVAL)
        m_tickTimerRetournement.setInterval(tickInterval);

    m_keepTicking = true;
    m_lastUpdateTime.start();
    m_tickTimerRetournement.start();
}



//! Met en place la démo de la balle bleue.
void GameCore::setupPlayer() {
    pPlayer = new Player;
    int ajustementHauteur = 80;
    pPlayer->setPos(m_pScene->width()/2, m_pScene->height() - ajustementHauteur);
    pPlayer->setZValue(1);          // Passe devant tous les autres sprites (sauf la sphère bleue)
    pPlayer->setScale(0.4);
    m_pScene->addSpriteToScene(pPlayer);
    pPlayer->registerForTick();
    connect(this, &GameCore::notifyKeyPressed, pPlayer, &Player::onKeyPressed);
    connect(this, &GameCore::notifyKeyReleased, pPlayer, &Player::onKeyReleased);
    connect(pPlayer,&Player::onplayerDestroyed, this, &GameCore::stopGame);
    m_pPlayer = pPlayer;
}

void GameCore::stopGame(){
    pPlayer->stopAnimation();
    pPlayer->deathAnimation();

    m_tickTimerObstacle.stop();
    m_tickTimerRetournement.stop();
    keyboardDisabled= true;
    m_pGameCanvas->stopTick();
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

void GameCore::rotateScreen() {
    m_pGameCanvas->rotateView();
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
}

//! Traite le relâchement d'un bouton de la souris.
void GameCore::mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonReleased(mousePosition, buttons);
}



