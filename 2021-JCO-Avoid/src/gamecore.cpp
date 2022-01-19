//!
//! Fichier qui contient toute la logique du jeu.
//! @author   PAPEFAB
//! @date     décembre 2021
//!


#include <cmath>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>

#include <QFontDatabase>
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

#include "player.h"
#include "gamescene.h"
#include "gamecanvas.h"
#include "objettickhandler.h"
#include "resources.h"
#include "sprite.h"
#include "utilities.h"
#include "mainfrm.h"
#include "ui_mainfrm.h"
#include "progressbar.h"
#include "gamecore.h"


//! Initialise le contrôleur de jeu.
//! \param pGameCanvas  GameCanvas pour lequel cet objet travaille.
//! \param pParent      Pointeur sur le parent (afin d'obtenir une destruction automatique de cet objet).
GameCore::GameCore(GameCanvas* pGameCanvas, QObject* pParent) : QObject(pParent) {

    pProgressBar = new progressBar;

    //Mise en place et démarrage du timer pour les obstacles
    m_tickTimerObstacle.setSingleShot(false);
    m_tickTimerObstacle.setInterval(SPAWN_INTERVAL);
    m_tickTimerObstacle.setTimerType(Qt::PreciseTimer); // Important pour avoir un précision suffisante sous Windows
    connect(&m_tickTimerObstacle, SIGNAL(timeout()), this, SLOT(setupObstacle()));

    //Mise en place et démarrage du timer pour le retournement de l'écran
    m_tickTimerRetournement.setSingleShot(false);
    m_tickTimerRetournement.setInterval(RETOURNEMENT_INTERVAL);
    m_tickTimerRetournement.setTimerType(Qt::PreciseTimer); // Important pour avoir un précision suffisante sous Windows
    connect(&m_tickTimerRetournement, SIGNAL(timeout()), this, SLOT(rotateScreen()));

    //Mise en place et démarrage du timer pour la diminution de la barre de progression
    m_tickTimerLoseEndurance.setSingleShot(false);
    m_tickTimerLoseEndurance.setInterval(LOSE_ENDURANCE_INTERVAL);
    m_tickTimerLoseEndurance.setTimerType(Qt::PreciseTimer); // Important pour avoir un précision suffisante sous Windows
    connect(&m_tickTimerLoseEndurance, SIGNAL(timeout()), pProgressBar, SLOT(loseEndurance()));



    //Mise en place et démarrage du timer de la partie
    m_tickTimerPartie.setSingleShot(false);
    m_tickTimerPartie.setInterval(ACTUALISATION_TEMPS);
    m_tickTimerPartie.setTimerType(Qt::PreciseTimer); // Important pour avoir une précision suffisante sous Windows
    connect(&m_tickTimerPartie, SIGNAL(timeout()), this, SLOT(timerPartie()));

    // Mémorise l'accès au canvas (qui gère le tick et l'affichage d'une scène)
    m_pGameCanvas = pGameCanvas;

    // Créé la scène de base et indique au canvas qu'il faut l'afficher.
    m_pScene = pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_WIDTH / GameFramework::screenRatio());
    pGameCanvas->setCurrentScene(m_pScene);

    m_pScene->setBackgroundImage(QImage(GameFramework::imagesPath() + "background1.png"));

    std::srand(std::time(nullptr));

    // Instancier et initialiser les différents objets du jeux ici :
    setupProgressBar();
    setupPlayer();
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

//! Met en place un obtsacle ou un bonus à une coordonnée x aléatoire
//!
void GameCore::setupObstacle(){
    nbGen=rand()%LARGEUR_MAX+LARGEUR_MINIMUM;   //génère un chiffre aléatoire entre la largeur maximal et minimal de l'écran

    nbGenObstacle = rand()%(INDEX_IMAGE_MAX - INDEX_IMAGE_MIN) + INDEX_IMAGE_MIN;
    //Si le jeux n'est pas terminé continue de créer des obstacles et des bonus
    if(jeuTermine == false){
        //Si le nombre d'obstacle apparu après un bonus est égal à l'interval de l'apparition des bonus un bonus est générer
        if(nombreObstacle == INTERVAL_APPARITION_BONUS){
            setupBonus();
            nombreObstacle = 0;
            //sinon génére un obstacle à une position x aléatoire
        }else{
            nombreObstacle++;
            Sprite* pObstacle = new Sprite(QString(GameFramework::imagesPath() + "obstacle/%0.png").arg(nbGenObstacle));
            pObstacle->setPos(nbGen,0);
            pObstacle->setScale(0.5);

            // Déplace le sprite vers le bas.
            // Si une collision à lieu, le tickhandler se charge
            // de détruire son sprite.
            pTickHandler = new ObjetTickHandler;
            pTickHandler->setDestroyOnCollisionEnabled(true);
            pObstacle->setTickHandler(pTickHandler);

            m_pScene->addSpriteToScene(pObstacle);

            pObstacle->registerForTick();
        }
    }
}

//! Génère un bonus en le faisant descendre
//!
void GameCore::setupBonus(){
    pBonus = new Sprite(GameFramework::imagesPath() + "Bonus.png");
    //Le bonus est placé à une position x aléatoire
    pBonus->setPos(nbGen,0);
    pBonus->setScale(0.25);
    pBonus->setData(0, "bonus");


    //déplace l'obstacle vers le bas
    //détruit l'obstacle lorsqu'il touche le sol
    ObjetTickHandler* pbTickHandler = new ObjetTickHandler;
    pbTickHandler->setDestroyOnCollisionEnabled(true);
    pBonus->setTickHandler(pbTickHandler);
    m_pScene->addSpriteToScene(pBonus);
    pBonus->registerForTick();
}

//! Met en place le timer de la partie et son text
//!
void GameCore::setupTimerPartie(){
    tempsPartie = 0;
    m_textTimer = "0";
    m_pObjetTimer = m_pScene->createText(QPointF(0,0), m_textTimer, 700);
    //Applique une police d'écriture au texte du timer
    int id = QFontDatabase::addApplicationFont(GameFramework::fontsPath() + "manaspc.ttf");
    QStringList stringList = QFontDatabase::applicationFontFamilies(id);
    QString familiy = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont mana(familiy);
    mana.setPointSize(50);

    m_pObjetTimer->setFont(mana);
}

//! Met en place le résultat de la partie
//!
void GameCore::setupResultat(){
    m_pObjetResultat = m_pScene->createText(QPointF((m_pScene->width() / 2) - 620,m_pScene->height() / 2), m_textResultat, 70);
    m_pObjetTimer->setOpacity(0.5);
    m_pObjetTimer->setPos(m_pScene->width() / 2 + 300,m_pScene->height() / 2);
    m_pObjetTimer->setOpacity(1);
    m_pObjetTimer->setZValue(1);
    //Applique une police d'écriture au texte du résultat
    int id = QFontDatabase::addApplicationFont(GameFramework::fontsPath() + "manaspc.ttf");
    QString familiy = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont mana(familiy);
    mana.setPointSize(50);

    m_pObjetResultat->setFont(mana);
}

//! Met en place la barre de progression
//!
void GameCore::setupProgressBar() {
    double screenXCenter = m_pScene->width() / 2;
    m_pProgressBarBorder = m_pScene->addRect(QRectF(screenXCenter - PROGRESSBAR_WIDTH/2, 10,PROGRESSBAR_WIDTH, 50), QPen(Qt::white), QBrush(Qt::black));
    m_pProgressBarBorder->setZValue(1);
    m_pProgressBarFill = m_pScene->addRect(QRectF(screenXCenter - PROGRESSBAR_WIDTH/2, 10,PROGRESSBAR_WIDTH, 50), QPen(Qt::transparent), QBrush(Qt::green));
    m_pProgressBarFill->setZValue(1);

    m_tickTimerLoseEndurance.start();
    pProgressBar->setProgressBarProcent(100);
}

//! Met à jour la barre de progression
//! en fonction du pourcentage progression de la barre,
//! elle prend une couleur (vert, orange, rouge)
//!
void GameCore::updateProgressBar() {

    if(jeuTermine == false) {
        double newWidth = PROGRESSBAR_WIDTH / 100 * pProgressBar->getProgressbarProcent();
        double screenXCenter = m_pScene->width() / 2;
        m_pProgressBarFill->setRect(screenXCenter - PROGRESSBAR_WIDTH/2, 10, newWidth, 50);

        //Si la barre de progression est rempli au plus du 2/3 la barre est verte
        if (pProgressBar->getProgressbarProcent() > 100/3*2) {
            m_pProgressBarFill->setBrush(QBrush(Qt::green));
            //Si la barre de progression est rempli au plus du 1/3 la barre est orange
        } else if (pProgressBar->getProgressbarProcent() > 100 / 3) {
            m_pProgressBarFill->setBrush(QBrush(Qt::yellow));
        } else {
            //Sinon la barre est rouge
            m_pProgressBarFill->setBrush(QBrush(Qt::red));
        }
        //Si la barre de progression atteint les 0% le jeux est arrêté
        if (pProgressBar->getProgressbarProcent() <= 0) {
            stopGame();
        }
    }
}

//! Configuration du timer pour spawn les obstacles
//! @param tickInterval
//!
void GameCore::startSpawnObstacleTimer(int tickInterval)  {

    if (tickInterval != KEEP_PREVIOUS_TICK_INTERVAL)
        m_tickTimerObstacle.setInterval(tickInterval);

    m_keepTicking = true;
    m_lastUpdateTime.start();
    m_tickTimerObstacle.start();
}

//! Configuration timer pour démarrer le timer de la partie
//! @param tickInterval
//!
void GameCore::startTimerPartie(int tickInterval){
    if (tickInterval != KEEP_PREVIOUS_TICK_INTERVAL)
        m_tickTimerObstacle.setInterval(tickInterval);

    m_keepTicking = true;
    m_lastUpdateTime.start();
    m_tickTimerPartie.start();
}

//! Configuration timer pour retourner l'écran
//! @param tickInterval
//!
void GameCore::startRetournerEcran(int tickInterval)  {

    if (tickInterval != KEEP_PREVIOUS_TICK_INTERVAL)
        m_tickTimerRetournement.setInterval(tickInterval);

    m_keepTicking = true;
    m_lastUpdateTime.start();
    m_tickTimerRetournement.start();
}

//! Configuration timer avant le recommencement du jeu
//! @param tickInterval
//!
void GameCore::startGameTimer(int tickInterval)  {

    if (tickInterval != KEEP_PREVIOUS_TICK_INTERVAL)
        m_tickTimerRestartGame.setInterval(tickInterval);

    m_keepTicking = true;
    m_lastUpdateTime.start();
    m_tickTimerRestartGame.start();
}

//! Actualise le text du timer pour lui ajouter 0,1
//!
void GameCore::timerPartie(){
    tempsPartie += 0.1;
    m_pObjetTimer->setText(QString::number(tempsPartie));
}

//! Met en place le joueur
//!
void GameCore::setupPlayer() {
    pPlayer = new Player;
    int ajustementHauteur = 80;

    pPlayer->setPos(m_pScene->width()/2, m_pScene->height() - ajustementHauteur);
    pPlayer->setZValue(1);   // Passe devant tous les autres sprites
    pPlayer->setScale(0.4);
    m_pScene->addSpriteToScene(pPlayer);
    pPlayer->registerForTick();

    connect(this, &GameCore::notifyKeyPressed, pPlayer, &Player::onKeyPressed);
    connect(this, &GameCore::notifyKeyReleased, pPlayer, &Player::onKeyReleased);

    connect(pPlayer, &Player::onBonusHit, pProgressBar, &progressBar::upEndurance);
    connect(pPlayer,&Player::onplayerDestroyed, this, &GameCore::stopGame);

    m_pPlayer = pPlayer;
}

//! Mise en place du bouton de redémarrage
//!
void GameCore::setupBouton(){
    pBouton = new Sprite(GameFramework::imagesPath() + "replay.png");

    pBouton->setPos(m_pScene->width() / 2, m_pScene->height() / 2 + 120);
    pBouton->setScale(0.1);

    m_pScene->addSpriteToScene(pBouton);
}

//! Supprimer tout les sprites de la scène excepté le joueur
//!
void GameCore::deleteAllSprite(){

    //supprime tous les srpites de la scène
    for (Sprite* sprite : m_pScene->sprites()) {

        if(sprite != m_pPlayer){

            m_pScene->removeSpriteFromScene(sprite);

            sprite->deleteLater();
        }
    }
}

//! Met fin à la partie du jeux
//!
void GameCore::stopGame(){
    keyboardDisabled = true;
    jeuTermine = true;

    pPlayer->stopAnimation();
    pPlayer->deathAnimation();

    m_tickTimerPartie.stop();
    m_tickTimerRetournement.stop();

    //Si l'écran est retourné alors le remet en place
    if(m_pObjetTimer->rotation() > 1) {
        m_pGameCanvas->rotateView();
        m_pObjetTimer->setRotation(0);
    }

    setupResultat();
    deleteAllSprite();
    setupBouton();
}

//! lance une nouvelle partie dans le jeux
//!
void GameCore::restartGame(){
    pProgressBar->setProgressBarProcent(100);

    m_pScene->removeSpriteFromScene(m_pPlayer);
    m_pScene->removeSpriteFromScene(pBouton);

    m_pScene->removeItem(m_pObjetResultat);
    m_pScene->removeItem(m_pObjetTimer);

    m_tickTimerPartie.start();
    m_tickTimerRetournement.start();

    jeuTermine = false;
    keyboardDisabled = false;

    setupTimerPartie();
    setupPlayer();
}

//! Retourne l'écran et le timer de 180 degrés
//!
void GameCore::rotateScreen() {

    m_pGameCanvas->rotateView();
    m_pObjetTimer->setTransformOriginPoint(m_pObjetTimer->boundingRect().center());

    //Si l'écran est retourné on le remet à 0 degré
    if(m_pObjetTimer->rotation() > 1) {
        m_pObjetTimer->setRotation(0);
        m_pObjetTimer->setX(0);
    } else {
        m_pObjetTimer->setRotation(180);
        m_pObjetTimer->setX(40);
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
    if (key == Qt::Key_Escape) {
        exit(0);
    }
    emit notifyKeyReleased(key);
}

//! Cadence.
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

//! Traite le relâchement d'un bouton de la souris.
void GameCore::mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonReleased(mousePosition, buttons);
}

//! Traite l'appui sur un bouton de la souris.
void GameCore::mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonPressed(mousePosition, buttons);

    // Si le bouton droite est cliqué et que le clip n'a pas déjà démarré,
    if (buttons.testFlag(Qt::LeftButton)) {
        if (jeuTermine) {
            // bouton gauche cliqué : on vérifie si le sprite en dessous peut être shooté.
            Sprite* pTargetSprite = m_pScene->spriteAt(mousePosition);
            if (pTargetSprite == pBouton) {
                // Le sprite est une tête à tirer : elle est détruite et explose
                pTickHandler->s_vitesseObtsacle = 7;
                restartGame();
            }
        }
    }
}
