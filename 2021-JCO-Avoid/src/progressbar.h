#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QObject>

/**
 * @brief classe qui gère le pourcentage de la barre de progression
 *
 * Augmente ou diminiue la barre de progression
 */
class progressBar : public QObject
{

    Q_OBJECT

public:
    progressBar();


    double progressBarPercentage = 100;

    void setProgressBarProcent(double progressBarProcent);
    double getProgressbarProcent();

public slots:
    void upEndurance();
    void loseEndurance();

};

#endif // PROGRESSBAR_H
