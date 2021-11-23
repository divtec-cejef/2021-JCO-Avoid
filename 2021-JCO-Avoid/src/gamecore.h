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


    void startSpawnObstacleTimer(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);
    void startRetournerEcran(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);
    void startGameTimer(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);
    void mouseMoved(QPointF newMousePosition);
    void mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons);
    void mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons);
    void startTimerPartie(int tickInterval = KEEP_PREVIOUS_TICK_INTERVAL);

    void tick(long long elapsedTimeInMilliseconds);

signals:
    void notifyMouseMoved(QPointF newMousePosition);
    void notifyMouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons);
    void notifyMouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons);
    void notifyKeyPressed(int key);
    void notifyKeyReleased(int key);

private:

    void setupPlayer();
    void setupTimerPartie();
    void setupWalkingMen();
    void configureAnimation();
    void setupBonus();
    void onSpriteDestroyed(QObject* pSprite);
    GameCanvas* m_pGameCanvas;
    GameScene* m_pScene;

    void setupProgressBar();
    void updateProgressBar();

    void setupResultat();
    double progressBarPercentage = 100;
    QGraphicsRectItem* m_ProgressBarBorder;
    QGraphicsRectItem* m_ProgressBarFill;

    QGraphicsSimpleTextItem* m_objetTimer;
    QGraphicsSimpleTextItem* m_objetResultat;
    double tempsPartie = 0;

    QString m_textTimer;
    QString m_textResultat = "Voici votre score : ";

    Sprite* pSprite;
    Sprite* m_pPlayer;
    Sprite* m_pAnimMort;
    Sprite* pBonus;
    Sprite* pObstacle;

    Player* pPlayer;

    bool keyboardDisabled = false;

    QTimer m_tickTimerObstacle;
    QTimer m_tickTimerRetournement;
    QTimer m_tickTimerRestartGame;
    QTimer m_tickTimerLoseEndurance;
    QTimer m_tickTimerPartie;


    bool m_keepTicking;
    int nombreObstacle = 0;
    int nbGen;

    QElapsedTimer m_lastUpdateTime;

    const int LARGEUR_MINIMUM = 1;
    const int LARGEUR_MAX = 1150;
    const int APPARITION_BONUS = 10;


private slots:
    void setupObstacle();
    void rotateScreen();
    void restartGame();
    void fillProgressBar();
    void loseEndurance();
    void timerPartie();

public slots:
    void stopGame();

};


#endif // GAMECORE_H
