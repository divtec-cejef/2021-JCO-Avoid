/**
  \file
  \brief    Déclaration de la classe GameCore.
  \author   PAPEFAB
  \date     décembre 2021
*/
#ifndef GAMECORE_H
#define GAMECORE_H

#include <QObject>
#include <QPointF>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <QTime>
#include <QTimer>
#include <QElapsedTimer>
#include <QFontDatabase>

#include "player.h"
#include "objettickhandler.h"
#include "mainfrm.h"
#include "ui_mainfrm.h"
#include "progressbar.h"

class GameCanvas;
class GameScene;
class Sprite;

//! \brief Classe qui gère la logique du jeu.
//!
//! Dans son état actuel, cette classe crée une scène vide, délimite
//! sa surface avec une ligne blanche puis démarre la cadence du jeu.
class GameCore : public QObject
{
    Q_OBJECT

public:
    enum { KEEP_PREVIOUS_TICK_INTERVAL = -1  };

    explicit GameCore(GameCanvas* pGameCanvas, QObject *parent = nullptr);
    ~GameCore();

    void keyPressed(int key);
    void keyReleased(int key);

    void mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons);
    void mouseMoved(QPointF newMousePosition);
    void mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons);

    void tick(long long elapsedTimeInMilliseconds);

signals:
    void notifyKeyPressed(int key);
    void notifyKeyReleased(int key);

    void notifyMouseMoved(QPointF newMousePosition);
    void notifyMouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons);
    void notifyMouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons);

private:

    void startSpawnObstacleTimer(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);
    void startRetournerEcran(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);
    void startGameTimer(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);
    void startTimerPartie(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);

    const int SPAWN_INTERVAL = 170;
    const int RETOURNEMENT_INTERVAL = 10000;
    const int LOSE_ENDURANCE_INTERVAL = 100;
    const int PROGRESSBAR_WIDTH = 500;
    const int ACTUALISATION_TEMPS = 100;
    const int INTERVAL_APPARITION_BONUS = 10;

    const int INDEX_IMAGE_MAX = 6;
    const int INDEX_IMAGE_MIN = 1;

    const int SCENE_WIDTH = 1920;
    const int LARGEUR_MINIMUM = - SCENE_WIDTH / 50;
    const int LARGEUR_MAX = SCENE_WIDTH - SCENE_WIDTH / 50;

    void setupPlayer();
    void setupTimerPartie();
    void setupBonus();
    void setupProgressBar();
    void setupBouton();
    void setupResultat();

    void deleteAllSprite();
    void configureAnimation();
    void btnAppuye();
    void onSpriteDestroyed(QObject* pSprite);
    void updateProgressBar();
    void gainEndurance();

    GameCanvas* m_pGameCanvas;
    GameScene* m_pScene;

    ObjetTickHandler* pTickHandler;

    QTimer m_tickTimerObstacle;
    QTimer m_tickTimerRetournement;
    QTimer m_tickTimerRestartGame;
    QTimer m_tickTimerLoseEndurance;
    QTimer m_tickTimerPartie;

    Sprite* pSprite;
    Sprite* m_pPlayer;
    Sprite* m_pAnimMort;
    Sprite* pBonus;
    Sprite* pBouton;

    Player* pPlayer;

    progressBar* pProgressBar;

    QGraphicsRectItem* m_pProgressBarBorder;
    QGraphicsRectItem* m_pProgressBarFill;
    QGraphicsSimpleTextItem* m_pObjetTimer;
    QGraphicsSimpleTextItem* m_pObjetResultat;

    QString m_textTimer;
    QString m_textResultat = "Voici votre temps : ";

    bool keyboardDisabled = false;
    bool jeuTermine = false;
    bool m_keepTicking;

    int nombreObstacle = 0;
    int nbGen;
    int nbGenObstacle;

    double tempsPartie = 0;

    QElapsedTimer m_lastUpdateTime;

private slots:
    void setupObstacle();
    void rotateScreen();
    void timerPartie();
    void stopGame();
    void restartGame();
};



#endif // GAMECORE_H
