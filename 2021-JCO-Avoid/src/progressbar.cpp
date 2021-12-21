#include "progressbar.h"

progressBar::progressBar()
{

}

/**
 * @brief retourne la progression de la barre
 * @return le pourcentage de progression
 */
double progressBar::getProgressbarProcent(){
       return progressBarPercentage;
}

/**
 * @brief Modifie la progresssion de la barre
 * @param pourcentage de progression
 */
void progressBar::setProgressBarProcent(double progressProcent){
    if (progressProcent > 100 ) progressProcent = 100;
    else if (progressProcent < 0) progressProcent = 0;
    progressBarPercentage = progressProcent;

}

/**
 * @brief augmente de 30% la barre de progression
 */

void progressBar::upEndurance(){
    setProgressBarProcent(getProgressbarProcent() + 30);
}

/**
 * @brief Diminue de 0,5% la barre de progression
 */
void progressBar::loseEndurance(){
    setProgressBarProcent(getProgressbarProcent()-0.5);
}

