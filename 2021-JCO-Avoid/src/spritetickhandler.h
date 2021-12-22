/**
  \file
  \brief    Déclaration de la classe SpriteTickHandler.
  \author   PAPEFAB
  \date     décembre 2021
*/
#ifndef SPRITETICKHANDLER_H
#define SPRITETICKHANDLER_H

class Sprite;

//! \brief Classe abstraite qui représente un gestionnaire de tick pour un sprite.
//!
//! Cette classe a pour but d'être spécialisée afin de donner à un sprite un comportement
//! à effectuer lors de chaque tick émis par la cadence.
//!
//! Lorsqu'un gestionnaire est affecté à un sprite (Sprite::setTickHandler), la
//! fonction init() du gestionnaire est appelée une unique fois, puis, lorsque
//! la cadence démarre, la fonction tick() du gestionnaire est automatiquement appelée.
//!
//! Depuis le gestionnaire, il est possible d'accéder au Sprite en question avec
//! l'attribut m_pParentSprite.
//!
class SpriteTickHandler
{
public:
    SpriteTickHandler(Sprite* pParentSprite = nullptr);
    virtual ~SpriteTickHandler();

public:
    virtual void setParentSprite(Sprite* pParentSprite);

    virtual void init() {}
    virtual void tick(long long elapsedTimeInMilliseconds) = 0;

protected:
    Sprite* m_pParentSprite;
};

#endif // SPRITETICKHANDLER_H
