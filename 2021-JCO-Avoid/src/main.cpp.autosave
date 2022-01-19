/*! \mainpage Avoid
 *
 * \author PAPEFAB
 * \section intro_1 Description
 *
 * Avoid est un jeu où il n’y a pas vraiment de version officielle du jeu car c’est un jeu que je me suis
 * imaginé. Le but est d’évité les obstacles qui tombe du ciel le plus longtemps possible, ensuite il faut
 * essayer de prendre les bonus qui eux aussi tombent du ciel pour rependre de l’endurance,
 * l’endurance qui elle est représenté par la barre de progression, si la barre de progression atteint son
 * minimum ou que le personnage touche un obstacle alors il meurt.
 * \image html images/Jeu.png "Image du jeu en fonctionnement" width=50%
 *
 * \section intro_3 Diagramme de classe
 * \image html images/Diagramme_UML-Avoid.png "Diagramme de classe du jeu" width=70%
 *
 * \section intro_4 Convention de nommage
 * - Variable simple \ref : Les variables commencent par une minuscule et sont camélisées.
 * - Variable membre : Les variables membres on un m souligné devant le nom de la variable.
 * - Variable pointeur : Les variables pointeurs ont un p minuscule devant le nom de la variable.
 * - Variable pointeur membre : Les variables pointeur membre sont deux types de variable combinées.
 * - Constante : Les constantes sont toujours en majuscule.
 * - Fonction : Les fonctions commencent par une minuscule et sont camélisées.
 * - Type énuméré : Les types énumérés commencent par une majuscule et sont camélisées.
 *
 */

#include "mainfrm.h"

#include <QApplication>

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainFrm w;
    w.show();

    // Pour un mode d'affichage fenêtré, plein écran
    // w.showMaximized();

    // Pour un mode d'affichage non-fenêtré, plein écran
    w.showFullScreen();

    return a.exec();
}
