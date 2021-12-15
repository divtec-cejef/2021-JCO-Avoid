#include "progressbar.h"

progressBar::progressBar()
{

}

double progressBar::getProgressbarProcent(){
       return progressBarPercentage;
}

void progressBar::setProgressBarProcent(double progressProcent){
    if (progressProcent > 100 ) progressProcent = 100;
    else if (progressProcent < 0) progressProcent = 0;
    progressBarPercentage = progressProcent;

}

void progressBar::upEndurance(){
    setProgressBarProcent(getProgressbarProcent() + 30);
}

void progressBar::loseEndurance(){
    setProgressBarProcent(getProgressbarProcent()-0.5);
}

