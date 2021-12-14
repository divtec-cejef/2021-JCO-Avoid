/**
  \file
  \brief    Déclaration de la classe GameCore.
  \author   JCO
  \date     septembre 2018
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

#include "player.h"
#include "randommovetickhandler.h"
#include "mainfrm.h"
#include "ui_mainfrm.h"
#include "bouton.h"

#include<QFontDatabase>
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

    void setProgressBarPercentage(double percentage);
    double getProgressBarPercentage();

    void keyPressed(int key);
    void keyReleased(int key);

    void mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons);
    void mouseMoved(QPointF newMousePosition);
    void mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons);


    void startSpawnObstacleTimer(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);
    void startRetournerEcran(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);
    void startGameTimer(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);

    void startTimerPartie(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);

    const int SPAWN_INTERVAL = 170;
    const int RETOURNEMENT_INTERVAL = 10000;
    const int LOSE_ENDURANCE_INTERVAL = 100;
    const int TIMER_BEFORE_START = 3000;

    const int PROGRESSBAR_WIDTH = 500;
    const int ACTUALISATION_TEMPS = 100;

    void tick(long long elapsedTimeInMilliseconds);

signals:
    void notifyKeyPressed(int key);
    void notifyKeyReleased(int key);

    void notifyMouseMoved(QPointF newMousePosition);
    void notifyMouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons);
    void notifyMouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons);

private:

    const int SCENE_WIDTH = 1920;

    void deleteAllSprite();
    void setupPlayer();
    void setupTimerPartie();
    void setupWalkingMen();
    void configureAnimation();
    void setupBonus();
    void btnAppuye();
    void onSpriteDestroyed(QObject* pSprite);
    GameCanvas* m_pGameCanvas;
    GameScene* m_pScene;

    void setupProgressBar();
    void updateProgressBar();
    void gainEndurance();
    void setupBouton();

    RandomMoveTickHandler* pTickHandler;

    void setupResultat();
    double progressBarPercentage = 100;
    QGraphicsRectItem* m_ProgressBarBorder;
    QGraphicsRectItem* m_ProgressBarFill;

    QGraphicsSimpleTextItem* m_objetTimer;
    QGraphicsSimpleTextItem* m_objetResultat;
    double tempsPartie = 0;

    QString m_textTimer;
    QString m_textResultat = "Voici votre temps : ";


    Sprite* pSprite;
    Sprite* m_pPlayer;
    Sprite* m_pAnimMort;
    Sprite* pBonus;
    Sprite* pObstacle;

    Player* pPlayer;
    Bouton* pBouton;
    Bouton* pBoutonVise;
    MainFrm* pMainFrm;

    bool keyboardDisabled = false;
    bool jeuTermine = true;

    QTimer m_tickTimerObstacle;
    QTimer m_tickTimerRetournement;
    QTimer m_tickTimerRestartGame;
    QTimer m_tickTimerLoseEndurance;
    QTimer m_tickTimerPartie;

    bool m_keepTicking;
    int nombreObstacle = 0;
    int nbGen;
    int nbGenObstacle;

    Ui::MainFrm *ui;

    QElapsedTimer m_lastUpdateTime;

    const int LARGEUR_MINIMUM = - SCENE_WIDTH / 50;
    const int LARGEUR_MAX = SCENE_WIDTH - SCENE_WIDTH / 50;
    const int APPARITION_BONUS = 10;


private slots:
    void setupObstacle();
    void rotateScreen();
    void upProgressBar();
    void loseEndurance();
    void timerPartie();

public slots:
    void stopGame();
    void restartGame();

};



#endif // GAMECORE_H
