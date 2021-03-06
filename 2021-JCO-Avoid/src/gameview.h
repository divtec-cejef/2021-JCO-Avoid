/**
  Déclaration de la classe GameView, qui affiche une scène.

  @author   PAPEFAB
  @date     décembre 2021
 */
#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>
#include <QTime>
#include <QTimer>
#include <QElapsedTimer>

//! \brief Classe de visualisation d'un espace 2D de jeu.
//!
//! GameView représente la vue utilisée par l'application pour afficher une scène (QGraphicsScene), généralement plus
//! spécifiquement la scène de jeu (GameScene)..
//!
//! Cette classe implémente les fonctionnalités suivantes :
//! - Possibilité d'adapter l'affichage de la scène à la taille de la fenêtre. Cette possibilité est déclanchée par défaut
//!   et peut être enclenchée avec setFitToScreenEnabled().
class GameView : public QGraphicsView
{
public:

    enum { KEEP_PREVIOUS_TICK_INTERVAL = -1  };
    GameView(QWidget* pParent = nullptr);
    GameView(QGraphicsScene* pScene, QWidget* pParent = nullptr);

    void setFitToScreenEnabled(bool fitToScreenEnabled);
    bool isFitToScreenEnabled() const;

    void rotateScreen();

protected:
    virtual void resizeEvent(QResizeEvent* pEvent);
    virtual void drawForeground(QPainter* pPainter, const QRectF& rRect);

private:
    void init();

    bool m_fitToScreen;
    bool m_clipScene;

    bool m_clippingRectUpToDate;
    QRectF m_clippingRect[4];
    QTimer m_tickTimer;
    bool m_keepTicking;

    QElapsedTimer m_lastUpdateTime;
};



#endif // GAMEVIEW_H
