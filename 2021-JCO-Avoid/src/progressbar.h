/**
  \file
  \brief    Déclaration de la classe ProgressBar.
  \author   PAPEFAB
  \date     décembre 2021
*/

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QObject>

//!
//! @brief Classe qui gère le remplissage de la barre de progression
//! Augmente upEndurance() ou diminiue loseEndurance() le pourcentage de la barre de progression
//!
class progressBar : public QObject
{

    Q_OBJECT

public:

    progressBar();

    void setProgressBarProcent(double progressBarProcent);
    double getProgressbarProcent();

public slots:

    void upEndurance();
    void loseEndurance();

private:

    double progressBarPercentage = 100;

};

#endif // PROGRESSBAR_H
