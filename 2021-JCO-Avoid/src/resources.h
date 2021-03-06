/**
  \file
  \brief    Méthodes utilitaires permettant d'accéder aux ressources.
  \author   PAPEFAB
  \date     décembre 2021
  \version  1.0
*/
#ifndef RESOURCES_H
#define RESOURCES_H

#include <QString>

//!
//! Espace de noms contenant les fonctions utilitaires pour les ressources.
//!
namespace GameFramework {
    QString resourcesPath();
    QString imagesPath();
    QString fontsPath();
}
#endif // RESOURCES_H
