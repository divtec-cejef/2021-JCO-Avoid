#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QObject>

/**
 * Classe qui gère le pourcentage de la barre de progression
 * @brief The progressBar class
 * Augmente upEndurance() ou diminiue loseEndurance() le pourcentage de la barre de progression
 */
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
