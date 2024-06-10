#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>

#include <QApplication>
#include <QFileInfo>
#include <QFont>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMainWindow>
#include <QPainter>
#include <QPixmap>
#include <QRadialGradient>
#include <QScrollBar>
#include <QTest>

#include "Labyrinth2d/Algorithm/Algorithm.h"
#include "Labyrinth2d/Qt/QLabyrinth.h"
#include "Labyrinth2d/Qt/GLLabyrinth.h"
#include "Labyrinth2d/Qt/MainWindow.h"
#include "Labyrinth2d/Qt/constants.h"
#include "Labyrinth2d/Solver/AStar.h"
#include "Labyrinth2d/Solver/Blind.h"
#include "Labyrinth2d/Solver/WallHand.h"

QLabyrinth::QLabyrinth(QScrollArea *parent, Niveau n, int longueurLabyrinthe, int largeurLabyrinthe, Algorithme a, TypeLabyrinthe type, FormeLabyrinthe forme, ModeLabyrinthe *mode) : QWidget(parent)
{
    scrollArea = parent;
    glLabyrinth = 0;
    typeResolution_ = 0;

	labyrinth = new Labyrinth2d::Labyrinth(1, 1);
    qPainterRenderer_ = new Labyrinth2d::Renderer::QPainter(*labyrinth);
    qPainterRenderer_->setMargins(QMarginsF());
    playerId = labyrinth->addPlayer(0, 0, {labyrinth->grid().rows() - 1}, {labyrinth->grid().columns() - 1}, true);
    qKeyPress = new Labyrinth2d::Mover::QKeyPress(*labyrinth, playerId);
    setFixedSize(1, 1);
    //xEntree = 0;
    //yEntree = 1;
    //xSortie = 0;
    //ySortie = 0;
    //largeur = 0;//impaire
    //longueur = 0;//impaire
    //emplacementXJoueur = xEntree;
    //emplacementYJoueur = yEntree;
    enResolution = false;
    partieEnCours = false;
    partieEnPause = false;
    typeLabyrinthe = Labyrinthe2D;
    formeLabyrinthe = Rectangle;
    niveau = Facile;
    enregistre = false;
    textures[0].typeTexture = TYPETEXTUREFOND;
    textures[0].couleur = COULEURFOND;
    textures[0].motif = MOTIFFOND;
    textures[0].image = IMAGEFOND;
    textures[1].typeTexture = TYPETEXTUREMUR;
    textures[1].couleur = COULEURMUR;
    textures[1].motif = MOTIFMUR;
    textures[1].image = IMAGEMUR;
    textures[2].typeTexture = TYPETEXTUREPARCOURS;
    textures[2].couleur = COULEURPARCOURS;
    textures[2].motif = MOTIFPARCOURS;
    textures[2].image = IMAGEPARCOURS;
    //setTailleCase(TAILLECASE);
    setWallsSize(WALLSSIZE);
    setWaysSize(WAYSSIZE);
    enConstruction = false;
    resolutionProgressive = false;
    afficherTrace = true;
    setEffacerChemin(true);
    modeLabyrinthe.mode = Aucun;
    algorithme = FirstDepthSearch;
    modeLabyrinthe.couleurObscurite = COULEUROBSCURITE;
    modeLabyrinthe.angleRotation = ANGLEROTATION;
    modeLabyrinthe.angleRotationAuHasard = ANGLEROTATIONAUHASARD;
    modeLabyrinthe.intervalleRotation = INTERVALLEROTATION;
    modeLabyrinthe.intervalleRotationAuHasard = INTERVALLEROTATIONAUHASARD;
    intervalleRotation = rand() % (INTERVALLEROTATIONMAX - INTERVALLEROTATIONMIN + 1) + INTERVALLEROTATIONMIN;
    angle = 0;
    modeLabyrinthe.rotationFixe = ROTATIONFIXE;
    modeLabyrinthe.sensRotation = SENSROTATION;
    modeLabyrinthe.sensRotationAuHasard = SENSROTATIONAUHASARD;
    modeLabyrinthe.rayonObscurite = RAYONOBSCURITE;
    modeLabyrinthe.pourcentageDistorsionVerticale = POURCENTAGEDISTORSIONVERTICALE;
    modeLabyrinthe.pourcentageDistorsionHorizontale = POURCENTAGEDISTORSIONHORIZONTALE;
    modeLabyrinthe.pourcentageDistorsionVerticaleAuHasard = POURCENTAGEDISTORSIONVERTICALEAUHASARD;
    modeLabyrinthe.pourcentageDistorsionHorizontaleAuHasard = POURCENTAGEDISTORSIONHORIZONTALEAUHASARD;
    modeLabyrinthe.intervalleDistorsionVerticale = INTERVALLEDISTORSIONVERTICALE;
    modeLabyrinthe.intervalleDistorsionHorizontale = INTERVALLEDISTORSIONHORIZONTALE;
    modeLabyrinthe.intervalleDistorsionVerticaleAuHasard = INTERVALLEDISTORSIONVERTICALEAUHASARD;
    modeLabyrinthe.intervalleDistorsionHorizontaleAuHasard = INTERVALLEDISTORSIONHORIZONTALEAUHASARD;
    modeLabyrinthe.distorsionSynchronisee = DISTORSIONSYNCHRONISEE;
    modeLabyrinthe.distorsionSynchroniseeAuHasard = DISTORSIONSYNCHRONISEEAUHASARD;
    modeLabyrinthe.pourcentageCisaillementVertical = POURCENTAGECISAILLEMENTVERTICAL;
    modeLabyrinthe.pourcentageCisaillementHorizontal = POURCENTAGECISAILLEMENTHORIZONTAL;
    modeLabyrinthe.pourcentageCisaillementVerticalAuHasard = POURCENTAGECISAILLEMENTVERTICALAUHASARD;
    modeLabyrinthe.pourcentageCisaillementHorizontalAuHasard = POURCENTAGECISAILLEMENTHORIZONTALAUHASARD;
    modeLabyrinthe.intervalleCisaillementVertical = INTERVALLECISAILLEMENTVERTICAL;
    modeLabyrinthe.intervalleCisaillementHorizontal = INTERVALLECISAILLEMENTHORIZONTAL;
    modeLabyrinthe.intervalleCisaillementVerticalAuHasard = INTERVALLECISAILLEMENTVERTICALAUHASARD;
    modeLabyrinthe.intervalleCisaillementHorizontalAuHasard = INTERVALLECISAILLEMENTHORIZONTALAUHASARD;
    modeLabyrinthe.cisaillementSynchronise = CISAILLEMENTSYNCHRONISE;
    modeLabyrinthe.cisaillementSynchroniseAuHasard = CISAILLEMENTSYNCHRONISEAUHASARD;
    numeroQuart = 1;
    nombreDeplacement = 0;
    indexTimerRotation = 0;
    sensRotation = SENSROTATION;
    tempsRestantRotation = 0;
    intervalleDistorsionVerticale = rand() % (INTERVALLEDISTORSIONVERTICALEMAX - INTERVALLEDISTORSIONVERTICALEMIN + 1) + INTERVALLEDISTORSIONVERTICALEMIN;
    indexTimerDistorsionVerticale = 0;
    intervalleDistorsionHorizontale = rand() % (INTERVALLEDISTORSIONHORIZONTALEMAX - INTERVALLEDISTORSIONHORIZONTALEMIN + 1) + INTERVALLEDISTORSIONHORIZONTALEMIN;
    indexTimerDistorsionHorizontale = 0;
    distorsionSynchronisee = DISTORSIONSYNCHRONISEE;
    pourcentageDistorsionVerticaleCourant = 0;
    pourcentageDistorsionHorizontaleCourant = 0;
    sensDistorsionVerticale = 1;
    sensDistorsionHorizontale = 1;
    pourcentageDistorsionVerticale = rand() % 101;
    pourcentageDistorsionHorizontale = rand() % 101;
    tempsRestantDistorsionVerticale = 0;
    tempsRestantDistorsionHorizontale = 0;
    modeLabyrinthe.distorsionVerticaleFixe = DISTORSIONVERTICALEFIXE;
    modeLabyrinthe.distorsionHorizontaleFixe = DISTORSIONHORIZONTALEFIXE;
    intervalleCisaillementVertical = rand() % (INTERVALLECISAILLEMENTVERTICALMAX - INTERVALLECISAILLEMENTVERTICALMIN + 1) + INTERVALLECISAILLEMENTVERTICALMIN;
    indexTimerCisaillementVertical = 0;
    intervalleCisaillementHorizontal = rand() % (INTERVALLECISAILLEMENTHORIZONTALMAX - INTERVALLECISAILLEMENTHORIZONTALMIN + 1) + INTERVALLECISAILLEMENTHORIZONTALMIN;
    indexTimerCisaillementHorizontal = 0;
    cisaillementSynchronise = CISAILLEMENTSYNCHRONISE;
    pourcentageCisaillementVerticalCourant = 0;
    pourcentageCisaillementHorizontalCourant = 0;
    sensCisaillementVertical = 1;
    sensCisaillementHorizontal = 1;
    pourcentageCisaillementVertical = rand() % 101;
    pourcentageCisaillementHorizontal = rand() % 101;
    tempsRestantCisaillementVertical = 0;
    tempsRestantCisaillementHorizontal = 0;
    modeLabyrinthe.cisaillementVerticalFixe = CISAILLEMENTVERTICALFIXE;
    modeLabyrinthe.cisaillementHorizontalFixe = CISAILLEMENTHORIZONTALFIXE;
    modeEcranDeVeille = false;
    modesAleatoires = true;
    affichageAleatoire = true;
    tailleAleatoire = true;
    algorithmeAleatoire = true;
    typeAleatoire = true;
    formeAleatoire = true;
    rapiditeResolution = 50;
    quitter = false;
    arretResolution = false;

    setFocusPolicy(Qt::StrongFocus);
    setAutoFillBackground(true);
    setMouseTracking(true);

    QPalette p = palette();
    p.setColor(QPalette::Window, textures[0].couleur);

    menu = new QMenu(this);
    menu->hide();

    nouveau(n, longueurLabyrinthe, largeurLabyrinthe, a, type, forme, mode);
}

QLabyrinth::~QLabyrinth()
{
	delete labyrinth;
    delete qPainterRenderer_;
	delete qKeyPress;
}

#include <QTime>
#include <QDebug>
void QLabyrinth::choisirDirection(int &d, int &e, int x, int y)
{
    d = 0;
    e = 0;
/*
    if (rand() % 2)
    {
        e = -2*(rand() % 2)+1;
    }
    else
    {
        d = -2*(rand() % 2)+1;
    }*/

    int marge = 10;
    int angle = double(rand())/double(RAND_MAX) * 2 * M_PI;
    double valeur = double(rand())/double(RAND_MAX) * 4 * marge * fabs(cos(x + y + angle));

    if (valeur < marge)
        d = cos((rand() % (x - y + 1)) * M_PI);
    else if (marge <= valeur && valeur < 2 * marge)
        d = cos((rand() % (x + y + 1)) * M_PI);
    else if (2*marge <= valeur && valeur < 3 * marge)
        e = cos((rand() % (x - y + 1)) * M_PI);
    else if (marge <= 3*valeur && valeur < 4 * marge)
        e = cos((rand() % (x + y + 1)) * M_PI);
}

void QLabyrinth::construireAncienBis()
{/**
    //initialisation du tableau

    labyrinthe.clear();

    for (int y = 0; y < largeur; y++)
    {
        labyrinthe << QList<int>();
        for (int x = 0; x < longueur; x++)
            labyrinthe[y] << 0;
    }

    //tracé des murs faisant les contours du labyrinthe

    for (int y = 0; y < largeur; y += largeur - 1)
        for (int x = 0; x < longueur; x++)
            labyrinthe[y][x] = 1;

    for (int x = 0; x < longueur; x += longueur - 1)
        for (int y = 1; y < largeur - 1; y++)
            labyrinthe[y][x] = 1;

    //tracé du quadrillage des murs

    for (int y = 2; y < largeur-2; y += 2)
        for (int x = 2; x < longueur-2; x += 2)
            labyrinthe[y][x] = 1;

    int nombreConnexionsTotal = (longueur - (longueur + 3) / 2) * (largeur - (largeur + 3) / 2);
    int nombreConnexionsRestantes = (longueur - (longueur + 3) / 2) * (largeur - (largeur + 3) / 2);

    int margeMax = sqrt(largeur * longueur);//(largeur * longueur) / sqrt(largeur * largeur + longueur * longueur);
    int marge, chiffre;

    do
    {
        int x = 0, y = 0, connexions = 0;

        do
        {
            //choix d'une position (paire) d'un mur

            x = (rand() % ((longueur - 1) / 2 + 1)) * 2;
            y = (rand() % ((largeur - 1) / 2 + 1)) * 2;

            connexions = 0;

            if (x && labyrinthe[y][x - 1])// == 1)
                connexions++;
            if (x + 1 < longueur && labyrinthe[y][x + 1])// == 1)
                connexions++;
            if (y && labyrinthe[y - 1][x])// == 1)
                connexions++;
            if (y + 1 < largeur && labyrinthe[y + 1][x])// == 1)
                connexions++;

            if ((!x && (!y || y == largeur - 1)) || (x == longueur - 1 && (!y || y == largeur - 1)))
                connexions = 0;

            if (nombreConnexionsRestantes < nombreConnexionsTotal / 3 && (!connexions || connexions == 4))
            {
                do
                {
                    marge = rand() % (margeMax - 1 + 1) + 1;
                    chiffre = rand()%(2 * marge + 1);
                    if (chiffre < marge)
                    {
                        chiffre = rand() % (2 * marge + 1);
                        if (chiffre < marge)
                            x += 2;
                        else
                            x -= 2;

                        if (x >= longueur)
                        {
                            x = 0;
                            y += 2;
                            if (y >= largeur)
                                y = 0;
                        }
                        else if (x < 0)
                        {
                            x = longueur - 1;
                            y -= 2;
                            if (y < 0)
                                y = largeur - 1;
                        }
                    }
                    else
                    {
                        chiffre = rand() % (2 * marge + 1);
                        if (chiffre < marge)
                            y += 2;
                        else
                            y -= 2;

                        if (y >= largeur)
                        {
                            y = 0;
                            x += 2;
                            if (x >= longueur)
                                x = 0;
                        }
                        else if (y < 0)
                        {
                            y = largeur - 1;
                            x -= 2;
                            if (x < 0)
                                x = longueur - 1;
                        }
                    }

                    connexions = 0;

                    if (x && labyrinthe[y][x - 1])// == 1)
                        connexions++;
                    if (x + 1 < longueur && labyrinthe[y][x + 1])// == 1)
                        connexions++;
                    if (y && labyrinthe[y - 1][x])// == 1)
                        connexions++;
                    if (y + 1 < largeur && labyrinthe[y + 1][x])// == 1)
                        connexions++;

                    if ((!x && (!y || y == largeur - 1)) || (x == longueur - 1 && (!y || y == largeur - 1)))
                        connexions = 0;
                } while (!connexions || connexions == 4);
            }
        } while (!connexions || connexions == 4);

        int max = longueur * largeur / (longueur + largeur) * nombreConnexionsRestantes / sqrt(nombreConnexionsTotal);
        int min = sqrt(longueur * largeur) / (sqrt(longueur) + sqrt(largeur)) * nombreConnexionsRestantes / sqrt(nombreConnexionsTotal);
        int longueurChemin = rand() % (max - min + 1) + min;

        for (int z = 0; z < longueurChemin; z++)
        {
            int compteur = 0;//compteur pour possiblité déplacement si échec déplacement choisi**//*
            int dDepart = 0;//déplacement horizontal
            int eDepart = 0;//déplacement vertical*//**
            bool inversion1 = false;
            bool inversion2 = false;
            bool inversion3 = false;

            marge = rand() % (margeMax - 1 + 1) + 1;
            chiffre = rand() % (2 * marge + 1);
            if (chiffre < marge)
            {
                marge = rand() % (margeMax - 1 + 1) + 1;
                chiffre = rand() % (2 * marge + 1);
                if (chiffre < marge)
                    dDepart = 1;
                else
                    dDepart = -1;
            }
            else
            {
                marge = rand() % (margeMax - 1 + 1) + 1;
                chiffre = rand() % (2 * marge + 1);
                if (chiffre < marge)
                    eDepart = 1;
                else
                    eDepart = -1;
            }

            int d = dDepart;
            int e = eDepart;**//*
            int valeur = rand() % (2 - 1 + 1) + 1;

            for (int i = 0 ; i < valeur; i++)
            {*//**
                do
                {
                    connexions = 0;

                    if (!e)
                    {
                        if (x + 3 * d < longueur && x + 3 * d >= 0)
                        {
                            if (labyrinthe[y][x + 3 * d])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                        if (x + 2 * d < longueur && x + 2 * d >= 0 && y + 1 < largeur)
                        {
                            if (labyrinthe[y + 1][x + 2 * d])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                        if (x + d < longueur && x + d >= 0)
                        {
                            if (labyrinthe[y][x + d])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                        if (x + 2 * d < longueur && x + 2 * d >= 0 && y - 1 >= 0)
                        {
                            if (labyrinthe[y - 1][x + 2 * d])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                    }
                    else
                    {
                        if (x + 1 < longueur && y + 2 * e < largeur && y + 2 * e >= 0)
                        {
                            if (labyrinthe[y + 2 * e][x + 1])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                        if (y + e < largeur && y + e >= 0)
                        {
                            if (labyrinthe[y + e][x])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                        if (x - 1 >= 0 && y + 2 * e < largeur && y + 2 * e >= 0)
                        {
                            if (labyrinthe[y + 2 * e][x - 1])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                        if (y + 3 * e < largeur && y + 3 * e >= 0)
                        {
                            if (labyrinthe[y + 3 * e][x])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                    }

                    if (connexions)
                    {
                        compteur++;
                        if (compteur == 4)
                            break;**//*
                        i = 0;
                        valeur = rand() % (2 - 1 + 1) + 1;*//**
                        marge = rand() % (margeMax - 1 + 1) + 1;//uint((x * x + y * y) / sqrt(x * x + y * y))
                        chiffre = rand() % (3 * marge + 1);
                        if (chiffre < marge)
                        {
                            if (!inversion1)
                            {
                                d = -dDepart;
                                e = -eDepart;
                                inversion1 = true;
                            }
                            else
                            {
                                marge = rand() % (margeMax - 1 + 1) + 1;
                                chiffre = rand() % (2 * marge + 1);
                                if (chiffre < marge)
                                {
                                    if (!inversion2)
                                    {
                                        d = eDepart;
                                        e = dDepart;
                                        inversion2 = true;
                                    }
                                    else
                                    {
                                        d = -eDepart;
                                        e = -dDepart;
                                        inversion3 = true;
                                    }
                                }
                                else
                                {
                                    if (!inversion3)
                                    {
                                        d = -eDepart;
                                        e = -dDepart;
                                        inversion3 = true;
                                    }
                                    else
                                    {
                                        d = eDepart;
                                        e = dDepart;
                                        inversion2 = true;
                                    }
                                }
                            }
                        }
                        else if (marge <= chiffre && chiffre < 2 * marge)
                        {
                            if (!inversion2)
                            {
                                d = eDepart;
                                e = dDepart;
                                inversion2 = true;
                            }
                            else
                            {
                                marge = rand() % (margeMax - 1 + 1) + 1;
                                chiffre = rand() % (2 * marge + 1);
                                if (chiffre < marge)
                                {
                                    if (!inversion1)
                                    {
                                        d = -dDepart;
                                        e = -eDepart;
                                        inversion1 = true;
                                    }
                                    else
                                    {
                                        d = -eDepart;
                                        e = -dDepart;
                                        inversion3 = true;
                                    }
                                }
                                else
                                {
                                    if (!inversion3)
                                    {
                                        d = -eDepart;
                                        e = -dDepart;
                                        inversion3 = true;
                                    }
                                    else
                                    {
                                        d = -dDepart;
                                        e = -eDepart;
                                        inversion1 = true;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (!inversion3)
                            {
                                d = -eDepart;
                                e = -dDepart;
                                inversion3 = true;
                            }
                            else
                            {
                                marge = rand() % (margeMax - 1 + 1) + 1;
                                chiffre = rand() % (2 * marge + 1);
                                if (chiffre < marge)
                                {
                                    if (!inversion1)
                                    {
                                        d = -dDepart;
                                        e = -eDepart;
                                        inversion1 = true;
                                    }
                                    else
                                    {
                                        d = eDepart;
                                        e = dDepart;
                                        inversion2 = true;
                                    }
                                }
                                else
                                {
                                    if (!inversion2)
                                    {
                                        d = eDepart;
                                        e = dDepart;
                                        inversion2 = true;
                                    }
                                    else
                                    {
                                        d = -dDepart;
                                        e = -eDepart;
                                        inversion1 = true;
                                    }
                                }
                            }
                        }
                    }
                } while (connexions);

                if (compteur != 4)
                {
                    labyrinthe[y + e][x + d] = 1;
                    x += 2 * d;
                    y += 2 * e;
                    nombreConnexionsRestantes--;
                }

                if (compteur == 4 || !nombreConnexionsRestantes)
                    break;
            }
**//*
            if (compteur == 4 || !nombreConnexionsRestantes)
                break;
        }*//**
    } while (nombreConnexionsRestantes);

    labyrinthe[yEntree][xEntree] = 2;
    labyrinthe[ySortie][xSortie] = 0;**/
}

void QLabyrinth::construireAncien()
{/**
    //initialisation du tableau

    labyrinthe.clear();

    for (int y = 0; y < largeur; y++)
    {
        labyrinthe << QList<int>();
        for (int x = 0; x < longueur; x++)
            labyrinthe[y] << 0;
    }

    //tracé des murs faisant les contours du labyrinthe

    for (int y = 0; y < largeur; y += largeur - 1)
        for (int x = 0; x < longueur; x++)
            labyrinthe[y][x] = 1;

    for (int x = 0; x < longueur; x += longueur - 1)
        for (int y = 1; y < largeur - 1; y++)
            labyrinthe[y][x] = 1;

    //tracé du quadrillage des murs

    for (int y = 2; y < largeur - 2; y += 2)
        for (int x = 2; x < longueur - 2; x += 2)
            labyrinthe[y][x] = 1;

    int x = 0, y = 0, connexions = 0, nombreConnexionsRestantes = (longueur - (longueur + 3) / 2) * (largeur - (largeur + 3) / 2);
    int compteur = 0;
    QList<int> liste[4];//x, y, longueur, largeur
    int nombreConnexionsTotal = (longueur - (longueur + 3) / 2) * (largeur - (largeur + 3) / 2);
    int temp = nombreConnexionsRestantes;

    do
    {
        //qDebug() << nombreConnexionsRestantes;
        if (nombreConnexionsRestantes > longueur * largeur / 4)
        {
            int v = rand() % (nombreConnexionsTotal - 0 + 1) + 0;
            if (v < int(nombreConnexionsTotal * 100 - 99 * nombreConnexionsRestantes) / 100)
            {
                int s = rand() % liste[0].size() + 1;
                for (int i = 0; i < s; i++)
                {
                    for (int y1 = liste[1][i]; y1 < largeur && y1 < liste[1][i] + liste[3][i]; y1 += liste[3][i] - 1)
                    {
                        for (int x1 = liste[0][i]; x1 < liste[0][i] + liste[2][i]; x1++)
                        {
                            if (x1 < longueur)
                            {
                                if (labyrinthe[y1][x1] == 3)
                                    labyrinthe[y1][x1] = 0;
                            }
                            else
                                break;
                        }
                    }
                    for (int x1 = liste[0][i]; x1 < longueur && x1 < liste[0][i] + liste[2][i]; x1 += liste[2][i] - 1)
                    {
                        for (int y1 = liste[1][i]; y1 < liste[1][i] + liste[3][i]; y1++)
                        {
                            if (y1 < largeur)
                            {
                                if (labyrinthe[y1][x1] == 3)
                                    labyrinthe[y1][x1] = 0;
                            }
                            else
                                break;
                        }
                    }
                }
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < s; j++)
                        liste[i].removeFirst();
                }
            }
            else// if (!(compteur % (rand() % ((longueur + largeur) / 2 - (longueur + largeur) / 4 + 1) + (longueur + largeur) / 4)))
            {
                int valeur = rand() % (nombreConnexionsRestantes * nombreConnexionsRestantes - nombreConnexionsRestantes + 1) + nombreConnexionsRestantes;
                for (int i = 0; i < valeur; i++)
                {
                    liste[0] << (rand() % ((longueur - 1) / 2 + 1)) * 2;
                    liste[1] << (rand() % ((largeur - 1) / 2 + 1)) * 2;
                    liste[2] << (rand() % (longueur/4 - 1 + 1) + 1) *2 + 1;
                    liste[3] << (rand() % (largeur/4 - 1 + 1) + 1) * 2 + 1;
                    if (fabs(double(liste[0].last() * liste[0].last() - liste[1].last() * liste[1].last()) / double(longueur * largeur)) < 1)
                    {
                        liste[2].last() *= fabs(cos(double(liste[0].last() * liste[0].last() - liste[1].last() * liste[1].last()) / double(longueur * largeur))) * 2;
                        liste[3].last() *= fabs(cos(double(liste[0].last() * liste[0].last() - liste[1].last() * liste[1].last()) / double(longueur * largeur))) * 2;
                    }
                    else
                    {
                        liste[2].last() *= fabs(sin(double(liste[0].last() * liste[0].last() - liste[1].last() * liste[1].last()) / double(longueur * largeur))) * 2;
                        liste[3].last() *= fabs(sin(double(liste[0].last() * liste[0].last() - liste[1].last() * liste[1].last()) / double(longueur * largeur))) * 2;
                    }
                }
                for (int i = 0; i < liste[0].size(); i++)
                {
                    for (int y1 = liste[1][i]; y1 < largeur && y1 < liste[1][i] + liste[3][i]; y1 += liste[3][i] - 1)
                    {
                        for (int x1 = liste[0][i]; x1 < liste[0][i] + liste[2][i]; x1++)
                        {
                            if (x1 < longueur)
                            {
                                if (!labyrinthe[y1][x1])
                                    labyrinthe[y1][x1] = 3;**//*
                                update();
                                QApplication::processEvents();*//**
                            }
                            else
                                break;
                        }
                    }
                    for (int x1 = liste[0][i]; x1 < longueur && x1 < liste[0][i] + liste[2][i]; x1 += liste[2][i] - 1)
                    {
                        for (int y1 = liste[1][i]; y1 < liste[1][i] + liste[3][i]; y1++)
                        {
                            if (y1 < largeur)
                            {
                                if (!labyrinthe[y1][x1])
                                    labyrinthe[y1][x1] = 3;**//*
                                update();
                                QApplication::processEvents();*//**
                            }
                            else
                                break;
                        }
                    }
                }
            }
        }
        else if (liste[0].size())
        {

            for (int i = 0; i < liste[0].size(); i++)
            {
                for (int y1 = liste[1][i]; y1 < largeur && y1 < liste[1][i] + liste[3][i]; y1 += liste[3][i] - 1)
                {
                    for (int x1 = liste[0][i]; x1 < liste[0][i] + liste[2][i]; x1++)
                    {
                        if (x1 < longueur)
                        {
                            if (labyrinthe[y1][x1] == 3)
                                labyrinthe[y1][x1] = 0;
                        }
                        else
                            break;
                    }
                }
                for (int x1 = liste[0][i]; x1 < longueur && x1 < liste[0][i] + liste[2][i]; x1 += liste[2][i] - 1)
                {
                    for (int y1 = liste[1][i]; y1 < liste[1][i] + liste[3][i]; y1++)
                    {
                        if (y1 < largeur)
                        {
                            if (labyrinthe[y1][x1] == 3)
                                labyrinthe[y1][x1] = 0;
                        }
                        else
                            break;
                    }
                }
            }
            for (int i = 0; i < 4; i++)
                liste[i].clear();
        }**//*
        if (temp == nombreConnexionsRestantes)
        {
            compteur++;
            if (compteur > 1000)
            {
                qDebug() << nombreConnexionsRestantes;
                qDebug() << liste[0].size();
                qApp->quit();
                return;
            }
        }
        else
        {
            compteur = 0;
            temp = nombreConnexionsRestantes;
        }*//**

        do
        {
            //choix d'une position (paire) d'un mur

            x = (rand() % ((longueur - 1) / 2 + 1)) * 2;
            y = (rand() % ((largeur - 1) / 2 + 1)) * 2;

            connexions = 0;

            if (x)
                if (labyrinthe[y][x - 1])// == 1)
                    connexions++;
            if (x + 1 < longueur)
                if (labyrinthe[y][x + 1])// == 1)
                    connexions++;
            if (y)
                if (labyrinthe[y - 1][x])// == 1)
                    connexions++;
            if (y + 1 < largeur)
                if (labyrinthe[y + 1][x])// == 1)
                    connexions++;

            if ((!x && (!y || y == largeur - 1)) || (x == longueur - 1 && (!y || y == largeur - 1)))
                connexions = 0;

            if (!connexions || connexions == 4)
            {
                if (rand() % 2)
                {
                    x += -4 * (rand() % 2) + 2;

                    if (x >= longueur)
                    {
                        x = 0;
                        y += 2;
                        if (y >= largeur)
                            y = 0;
                    }
                    else if (x < 0)
                    {
                        x = longueur - 1;
                        y -= 2;
                        if (y < 0)
                            y = largeur - 1;
                    }
                }
                else
                {
                    y += -4 * (rand() % 2) + 2;

                    if (y >= largeur)
                    {
                        y = 0;
                        x += 2;
                        if (x >= longueur)
                            x = 0;
                    }
                    else if (y < 0)
                    {
                        y = largeur - 1;
                        x -= 2;
                        if (x < 0)
                            x = longueur - 1;
                    }
                }
            }
        } while (!connexions || connexions == 4);

        uint longueurChemin = rand() % (longueur + largeur) + 1;

        for (uint z = 0; z < longueurChemin; z++)
        {
            int c = 0;//compteur pour possiblité déplacement si échec déplacement choisi**//*
            int dDepart = 0;//déplacement horizontal
            int eDepart = 0;//déplacement vertical*//**
            bool inversion1 = false;
            bool inversion2 = false;
            bool inversion3 = false;

            if (rand() % 2)
                dDepart = -2 * (rand() % 2) + 1;
            else
                eDepart = -2 * (rand() % 2) + 1;

            int d = dDepart;
            int e = eDepart;**//*
            int valeur = rand() % (2 - 1 + 1) + 1;

            for (int i = 0 ; i < valeur; i++)
            {*//**
                do
                {
                    connexions = 0;

                    if (!e)
                    {
                        if (x + 3 * d < longueur && x + 3 * d >= 0)
                        {
                            if (labyrinthe[y][x + 3 * d])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                        if (x + 2 * d < longueur && x + 2 * d >= 0 && y + 1 < largeur)
                        {
                            if (labyrinthe[y + 1][x + 2 * d])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                        if (x + d < longueur && x + d >= 0)
                        {
                            if (labyrinthe[y][x + d])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                        if (x + 2 * d < longueur && x + 2 * d >= 0 && y - 1 >= 0)
                        {
                            if (labyrinthe[y - 1][x + 2 * d])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                    }
                    else
                    {
                        if (x + 1 < longueur && y + 2 * e < largeur && y + 2 * e >= 0)
                        {
                            if (labyrinthe[y + 2 * e][x + 1])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                        if (y + e < largeur && y + e >= 0)
                        {
                            if (labyrinthe[y + e][x])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                        if (x - 1 >= 0 && y + 2 * e < largeur && y + 2 * e >= 0)
                        {
                            if (labyrinthe[y + 2 * e][x - 1])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                        if (y + 3 * e < largeur && y + 3 * e >= 0)
                        {
                            if (labyrinthe[y + 3 * e][x])// == 1)
                                connexions++;
                        }
                        else
                            connexions++;
                    }

                    if (connexions)
                    {
                        c++;
                        if (c == 4)
                            break;**//*
                        i = 0;
                        valeur = rand() % (2 - 1 + 1) + 1;*//**
                        int marge = rand() % (100 - 10 + 1) + 10;//uint((x * x + y * y) / sqrt(x * x + y * y))
                        int chiffre = rand() % 301;
                        if (chiffre < marge)
                        {
                            if (!inversion1)
                            {
                                d = -dDepart;
                                e = -eDepart;
                                inversion1 = true;
                            }
                            else
                            {
                                marge = rand() % (100 - 10 + 1) + 10;
                                chiffre = rand() % 201;
                                if (chiffre < marge)
                                {
                                    if (!inversion2)
                                    {
                                        d = eDepart;
                                        e = dDepart;
                                        inversion2 = true;
                                    }
                                    else
                                    {
                                        d = -eDepart;
                                        e = -dDepart;
                                        inversion3 = true;
                                    }
                                }
                                else
                                {
                                    if (!inversion3)
                                    {
                                        d = -eDepart;
                                        e = -dDepart;
                                        inversion3 = true;
                                    }
                                    else
                                    {
                                        d = eDepart;
                                        e = dDepart;
                                        inversion2 = true;
                                    }
                                }
                            }
                        }
                        else if (marge <= chiffre && chiffre < 2 * marge)
                        {
                            if (!inversion2)
                            {
                                d = eDepart;
                                e = dDepart;
                                inversion2 = true;
                            }
                            else
                            {
                                marge = rand() % (100 - 10 + 1) + 10;
                                chiffre = rand() % 201;
                                if (chiffre < marge)
                                {
                                    if (!inversion1)
                                    {
                                        d = -dDepart;
                                        e = -eDepart;
                                        inversion1 = true;
                                    }
                                    else
                                    {
                                        d = -eDepart;
                                        e = -dDepart;
                                        inversion3 = true;
                                    }
                                }
                                else
                                {
                                    if (!inversion3)
                                    {
                                        d = -eDepart;
                                        e = -dDepart;
                                        inversion3 = true;
                                    }
                                    else
                                    {
                                        d = -dDepart;
                                        e = -eDepart;
                                        inversion1 = true;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (!inversion3)
                            {
                                d = -eDepart;
                                e = -dDepart;
                                inversion3 = true;
                            }
                            else
                            {
                                marge = rand() % (100 - 10 + 1) + 10;
                                chiffre = rand() % 201;
                                if (chiffre < marge)
                                {
                                    if (!inversion1)
                                    {
                                        d = -dDepart;
                                        e = -eDepart;
                                        inversion1 = true;
                                    }
                                    else
                                    {
                                        d = eDepart;
                                        e = dDepart;
                                        inversion2 = true;
                                    }
                                }
                                else
                                {
                                    if (!inversion2)
                                    {
                                        d = eDepart;
                                        e = dDepart;
                                        inversion2 = true;
                                    }
                                    else
                                    {
                                        d = -dDepart;
                                        e = -eDepart;
                                        inversion1 = true;
                                    }
                                }
                            }
                        }
                    }
                } while (connexions);

                if (c != 4)
                {
                    labyrinthe[y + e][x + d] = 1;
                    x += 2 * d;
                    y += 2 * e;
                    nombreConnexionsRestantes--;
                }

                if (c == 4 || !nombreConnexionsRestantes)
                    break;
            }
**//*
            if (c == 4 || !nombreConnexionsRestantes)
                break;
        }*//**
    } while (nombreConnexionsRestantes);

    for (int i = 0; i < liste[0].size(); i++)
    {
        for (int y1 = liste[1][i]; y1 < largeur && y1 < liste[1][i] + liste[3][i]; y1 += liste[3][i] - 1)
        {
            for (int x1 = liste[0][i]; x1 < liste[0][i] + liste[2][i]; x1++)
            {
                if (x1 < longueur)
                {
                    if (labyrinthe[y1][x1] == 3)
                        labyrinthe[y1][x1] = 0;
                }
                else
                    break;
            }
        }
        for (int x1 = liste[0][i]; x1 < longueur && x1 < liste[0][i] + liste[2][i]; x1 += liste[2][i] - 1)
        {
            for (int y1 = liste[1][i]; y1 < liste[1][i] + liste[3][i]; y1++)
            {
                if (y1 < largeur)
                {
                    if (labyrinthe[y1][x1] == 3)
                        labyrinthe[y1][x1] = 0;
                }
                else
                    break;
            }
        }
    }
    for (int i = 0; i < 4; i++)
        liste[i].clear();

    labyrinthe[yEntree][xEntree] = 2;
    labyrinthe[ySortie][xSortie] = 0;**/
}

void QLabyrinth::construireHomeMadeAlgorithm1()
{
    construireAncien();
}

void QLabyrinth::construireHomeMadeAlgorithm2()
{
    construireAncienBis();
}

void QLabyrinth::construireHomeMadeAlgorithm3()
{/**
    int lar = largeur, lon = longueur, xE = xEntree, yE = yEntree, xS = xSortie, yS = ySortie;

    xEntree = 0;
    yEntree = 0;
    xSortie = 0;
    ySortie = 0;

    largeur = rand() % (largeur / 2 - 5 + 1) + 5;
    if (!(largeur % 2))
        largeur++;
    longueur = rand() % (longueur / 2 - 5 + 1) + 5;
    if (!(longueur % 2))
        longueur++;

    if (largeur < 5)
        largeur = 5;
    if (longueur < 5)
        longueur = 5;

    Algorithme a = Algorithme(rand() % 9);

    while (a == FractaleAlgorithm)
    {
        a = Algorithme(rand() % 9);
    }

    switch (a)
    {
        case FirstDepthSearch:
            construireFirstDepthSearch();
            break;
        case CellFusion:
            construireCellFusion();
            break;
        case RecursiveDivision:
            construireRecursiveDivision();
            break;
        case PrimsAlgorithm:
            construireFirstDepthSearch(true);
            break;
        case HuntAndKillAlgorithm:
            construireFirstDepthSearch(false, true);
            break;
        case GrowingTreeAlgorithm:
            construireFirstDepthSearch(false, false, true);
            break;
        case FractaleAlgorithm:
            construireFractaleAlgorithm();
            break;
        case HomeMadeAlgorithm1:
            construireHomeMadeAlgorithm1();
            break;
        case HomeMadeAlgorithm2:
            construireHomeMadeAlgorithm2();
            break;
        case HomeMadeAlgorithm3:
        default:
            construireFirstDepthSearch();
            break;
    }

    labyrinthe[yEntree][xEntree] = 1;
    labyrinthe[ySortie][xSortie] = 1;

    largeur = lar;
    longueur = lon;
    xEntree = xE;
    yEntree = yE;
    xSortie = xS;
    ySortie = yS;

    bool longueurAtteinte = (labyrinthe[0].size() == longueur), largeurAtteinte = (labyrinthe.size() == largeur);

    while (!longueurAtteinte || !largeurAtteinte)
    {
        if ((rand() % 2 || longueurAtteinte) && !largeurAtteinte)//Ajout de deux lignes
        {
            int y = (rand() % ((labyrinthe.size() - 3) / 2 + 1)) * 2 + 1;

            QList<int> lignesAAjouter[3];

            for (int i = 0; i+4 < labyrinthe[0].size(); i += 4)
            {
                QList<QList<int> > motif = construireMotif(false, !labyrinthe[y][i + 4], false, !labyrinthe[y][i], labyrinthe[y][i + 2], true);

                for (int j = 0; j < 4; j++)
                    for (int k = 0; k < 3; k++)
                        lignesAAjouter[k] << motif[k + 1][j];

                if ((labyrinthe[0].size() - 1) % 4 && i + 8 >= labyrinthe[0].size())
                    for (int k = 0; k < 3; k++)
                        lignesAAjouter[k] << motif[k + 1][4];
            }

            if ((labyrinthe[0].size() - 1) % 4)
                for (int k = 0; k < 3; k++)
                    lignesAAjouter[k] << 0;

            for (int k = 0; k < 3; k++)
                lignesAAjouter[k] << 1;

            labyrinthe.removeAt(y);

            for (int k = 0; k < 3; k++)
                labyrinthe.insert(y, lignesAAjouter[2 - k]);
        }
        else if (!longueurAtteinte)//Ajout de deux colonnes
        {
            int x = (rand() % ((labyrinthe[0].size() - 3) / 2 + 1)) * 2 + 1;

            QList<int> colonnesAAjouter[3];

            for (int i = 0; i+4 < labyrinthe.size(); i += 4)
            {
                QList<QList<int> > motif = construireMotif(!labyrinthe[i][x], false, !labyrinthe[i + 4][x], false, labyrinthe[i + 2][x], false);

                for (int j = 0; j < 4; j++)
                    for (int k = 0; k < 3; k++)
                        colonnesAAjouter[k] << motif[j][k + ];

                if ((labyrinthe.size() - 1) % 4 && i + 8 >= labyrinthe.size())
                    for (int k = 0; k < 3; k++)
                        colonnesAAjouter[k] << motif[4][k + 1];
            }

            if ((labyrinthe.size() - 1) % 4)
                for (int k = 0; k < 3; k++)
                    colonnesAAjouter[k] << 0;

            for (int k = 0; k < 3; k++)
                colonnesAAjouter[k] << 1;

            for (int i = 0; i < labyrinthe.size(); i++)
                labyrinthe[i].removeAt(x);

            for (int i = 0; i < labyrinthe.size(); i++)
                for (int k = 0; k < 3; k++)
                    labyrinthe[i].insert(x, colonnesAAjouter[2 - k][i]);
        }

        longueurAtteinte = (labyrinthe[0].size() == longueur);
        largeurAtteinte = (labyrinthe.size() == largeur);
    }

    labyrinthe[yEntree][xEntree] = 2;
    labyrinthe[ySortie][xSortie] = 0;**/
}

bool QLabyrinth::resoudre(int nombreFois)
{
    if (getEmplacementXJoueur() == getXSortie() && getEmplacementYJoueur() == getYSortie())
        return true;

	size_t const seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine g(seed);
	//std::random_device g;

    auto const sleep{
        [this] (std::chrono::milliseconds const& ms) -> void
        {
            QTest::qWait(ms);
            this->update();
        }
    };

    size_t const cycleOperationsSolving(1);
    std::chrono::milliseconds const cyclePauseSolving(1 * 50);

    if (typeResolution_ == 0)
    {
        Labyrinth2d::Solver::AStar ass;
        labyrinth->player(playerId).solve(g, ass, sleep, 0, 0, cycleOperationsSolving,
                                          cyclePauseSolving, nullptr);
    }
    else if (typeResolution_ == 1)
    {
        Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Right};
        labyrinth->player(playerId).solve(g, whs, sleep, 0, 0, cycleOperationsSolving,
                                          cyclePauseSolving, nullptr);
    }
    else if (typeResolution_ == 2)
    {
        Labyrinth2d::Solver::WallHand whs{Labyrinth2d::Solver::WallHand::Left};
        labyrinth->player(playerId).solve(g, whs, sleep, 0, 0, cycleOperationsSolving,
                                          cyclePauseSolving, nullptr);
    }
    else //if (typeResolution_ == 3)
    {
        Labyrinth2d::Solver::Blind bs;
        labyrinth->player(playerId).solve(g, bs, sleep, 0, 0, cycleOperationsSolving,
                                          cyclePauseSolving, nullptr);
    }

    emit deplacementChange();

    return false;
}

int QLabyrinth::getXEntree() const
{
    return labyrinth->player(playerId).startJ();
}

int QLabyrinth::getYEntree() const
{
    return labyrinth->player(playerId).startI();
}

int QLabyrinth::getXSortie() const
{
    return labyrinth->player(playerId).finishJ().front();
}

int QLabyrinth::getYSortie() const
{
    return labyrinth->player(playerId).finishI().front();
}

int QLabyrinth::getLargeur() const
{
    return labyrinth->grid().rows();
}

int QLabyrinth::getLongueur() const
{
    return labyrinth->grid().columns();
}

int QLabyrinth::getEmplacementXJoueur() const
{
    return labyrinth->player(playerId).j();
}

int QLabyrinth::getEmplacementYJoueur() const
{
    return labyrinth->player(playerId).i();
}

bool QLabyrinth::getEnResolution() const
{
    return enResolution;
}

QLabyrinth::TypeLabyrinthe QLabyrinth::getTypeLabyrinthe() const
{
    return typeLabyrinthe;
}

QLabyrinth::FormeLabyrinthe QLabyrinth::getFormeLabyrinthe() const
{
    return formeLabyrinthe;
}

QLabyrinth::Texture QLabyrinth::getTextureParcours() const
{
    return textures[2];
}

void QLabyrinth::setTextureParcours(const Texture &texture)
{
    if (textures[2].couleur == texture.couleur && textures[2].motif == texture.motif && textures[2].image == texture.image && textures[2].typeTexture == texture.typeTexture)
        return;

    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }

    textures[2] = texture;

    if (texture.typeTexture == QLabyrinth::TextureCouleur)
        qPainterRenderer_->changeWaysTexture(Labyrinth2d::Renderer::QPainter::Texture(texture.couleur, 50.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    else if (texture.typeTexture == QLabyrinth::TextureMotif)
        qPainterRenderer_->changeWaysTexture(Labyrinth2d::Renderer::QPainter::Texture(QPixmap(texture.motif), Labyrinth2d::Renderer::QPainter::Texture::Pattern, Qt::IgnoreAspectRatio));
    else// if (texture.typeTexture == QLabyrinth::TextureImage)
        qPainterRenderer_->changeWaysTexture(Labyrinth2d::Renderer::QPainter::Texture(QPixmap(texture.image), Labyrinth2d::Renderer::QPainter::Texture::Background, Qt::KeepAspectRatioByExpanding));

    if (glLabyrinth)
        glLabyrinth->rechargerTextures();

    rafraichir();
}

QLabyrinth::Texture QLabyrinth::getTextureMur() const
{
    return textures[1];
}

void QLabyrinth::setTextureMur(const Texture &texture)
{
    if (textures[1].couleur == texture.couleur && textures[1].motif == texture.motif && textures[1].image == texture.image && textures[1].typeTexture == texture.typeTexture)
        return;

    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }

    textures[1] = texture;

    if (texture.typeTexture == QLabyrinth::TextureCouleur)
        qPainterRenderer_->changeWallsTexture(Labyrinth2d::Renderer::QPainter::Texture(texture.couleur, 50.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    else if (texture.typeTexture == QLabyrinth::TextureMotif)
        qPainterRenderer_->changeWallsTexture(Labyrinth2d::Renderer::QPainter::Texture(QPixmap(texture.motif), Labyrinth2d::Renderer::QPainter::Texture::Pattern, Qt::IgnoreAspectRatio));
    else// if (texture.typeTexture == QLabyrinth::TextureImage)
        qPainterRenderer_->changeWallsTexture(Labyrinth2d::Renderer::QPainter::Texture(QPixmap(texture.image), Labyrinth2d::Renderer::QPainter::Texture::Background, Qt::KeepAspectRatioByExpanding));

    if (glLabyrinth)
        glLabyrinth->rechargerTextures();

    rafraichir();
}

QLabyrinth::Texture QLabyrinth::getTextureFond() const
{
    return textures[0];
}

void QLabyrinth::setTextureFond(const Texture &texture)
{
    if (textures[0].couleur == texture.couleur && textures[0].motif == texture.motif && textures[0].image == texture.image && textures[0].typeTexture == texture.typeTexture)
        return;

    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }

    textures[0] = texture;

    if (texture.typeTexture == QLabyrinth::TextureCouleur)
        qPainterRenderer_->changeBackgroundTexture(Labyrinth2d::Renderer::QPainter::Texture(texture.couleur, 50.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    else if (texture.typeTexture == QLabyrinth::TextureMotif)
        qPainterRenderer_->changeBackgroundTexture(Labyrinth2d::Renderer::QPainter::Texture(QPixmap(texture.motif), Labyrinth2d::Renderer::QPainter::Texture::Pattern, Qt::IgnoreAspectRatio));
    else// if (texture.typeTexture == QLabyrinth::TextureImage)
        qPainterRenderer_->changeBackgroundTexture(Labyrinth2d::Renderer::QPainter::Texture(QPixmap(texture.image), Labyrinth2d::Renderer::QPainter::Texture::Background, Qt::KeepAspectRatioByExpanding));

    QPalette p = scrollArea->palette();

    if (modeLabyrinthe.mode & Obscurite)
        p.setColor(QPalette::Window, modeLabyrinthe.couleurObscurite);
    else
        p.setColor(QPalette::Window, textures[0].couleur);

    scrollArea->setPalette(p);

    if (glLabyrinth)
        glLabyrinth->rechargerTextures();

    rafraichir();
}

bool QLabyrinth::getPartieEnCours() const
{
    return partieEnCours;
}

bool QLabyrinth::getPartieEnPause() const
{
    return partieEnPause;
}

QSize const& QLabyrinth::getTailleCase() const
{
    return tailleCase;
}

void QLabyrinth::setTailleCase(const QSize &taille)
{
    if (tailleCase == taille)
        return;

    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }

    tailleCase = taille;

    qPainterRenderer_->changeWallsSize(taille);
    qPainterRenderer_->changeWaysSize(taille);

    rafraichir();

    if (typeLabyrinthe == Labyrinthe2Den3D)
    {
        glLabyrinth->tailleCaseChangee();
        return;
    }

    setFixedSize(transform.mapRect(QRectF(0, 0, getLongueur() * tailleCase.width(), getLargeur() * tailleCase.height()).toRect()).size());
    if (size().width() < QGuiApplication::screenAt(pos())->size().width() - 50 && size().height() < QGuiApplication::screenAt(pos())->size().height() - 50)
    {
        scrollArea->setMinimumSize(size().width() + scrollArea->frameWidth() * 2, size().height() + scrollArea->frameWidth() * 2);
        QTimer::singleShot(50, this, SLOT(redimensionnerFenetrePrincipale()));
    }
    else
    {
        scrollArea->setMinimumSize(LONGUEURFACILE * TAILLECASE.width() + scrollArea->frameWidth() * 2, LARGEURFACILE * TAILLECASE.height() + scrollArea->frameWidth() * 2);
        QTimer::singleShot(50, this, SLOT(maximiserFenetre()));
    }

    QRect rect = transform.mapRect(QRect(getEmplacementXJoueur() * tailleCase.width() - tailleCase.width() * 2, getEmplacementYJoueur() * tailleCase.height() - tailleCase.height() * 2, tailleCase.width() + tailleCase.width() * 5, tailleCase.height() + tailleCase.height() * 5));
    scrollArea->ensureVisible(rect.x(), rect.y(), rect.width(), rect.height());
}

QSize const& QLabyrinth::waysSize() const
{
    return waysSize_;
}

void QLabyrinth::setWaysSize(const QSize &waysSize)
{
    if (waysSize_== waysSize)
        return;

    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }

    waysSize_ = waysSize;

    qPainterRenderer_->changeWaysSize(waysSize);

    rafraichir();

    if (typeLabyrinthe == Labyrinthe2Den3D)
    {
        glLabyrinth->tailleCaseChangee();
        return;
    }

    setFixedSize(transform.mapRect(QRectF(0, 0,
                                          getLongueur() * waysSize_.width() + (getLongueur() + 1) * wallsSize_.width(),
                                          getLargeur() * waysSize_.height() + (getLargeur() + 1) * wallsSize_.height()).toRect()).size());
    if (size().width() < QGuiApplication::screenAt(pos())->size().width() - 50
        && size().height() < QGuiApplication::screenAt(pos())->size().height() - 50)
    {
        scrollArea->setMinimumSize(size().width() + scrollArea->frameWidth() * 2,
                                   size().height() + scrollArea->frameWidth() * 2);
        QTimer::singleShot(50, this, SLOT(redimensionnerFenetrePrincipale()));
    }
    else
    {
        scrollArea->setMinimumSize(LONGUEURFACILE * WAYSSIZE.width() + (LONGUEURFACILE + 1) * WALLSSIZE.width() + scrollArea->frameWidth() * 2,
                                   LARGEURFACILE * WAYSSIZE.height() + (LARGEURFACILE + 1) * WALLSSIZE.height() + scrollArea->frameWidth() * 2);
        QTimer::singleShot(50, this, SLOT(maximiserFenetre()));
    }

    QRect rect = transform.mapRect(QRect((getEmplacementXJoueur() / 2) * waysSize_.width() + (getEmplacementXJoueur() / 2 + 1) * wallsSize_.width() - waysSize_.width() - wallsSize_.width(),
                                         (getEmplacementYJoueur() / 2) * waysSize_.height() + (getEmplacementYJoueur() / 2 + 1) * wallsSize_.height() - waysSize_.height() - wallsSize_.width(),
                                         waysSize_.width() + (waysSize_.width() + wallsSize_.width()) * 2,
                                         waysSize_.height() + (waysSize_.height() + wallsSize_.height()) * 2));
    scrollArea->ensureVisible(rect.x(), rect.y(), rect.width(), rect.height());
}

QSize const& QLabyrinth::wallsSize() const
{
    return wallsSize_;
}

void QLabyrinth::setWallsSize(const QSize &wallsSize)
{
    if (wallsSize_== wallsSize)
        return;

    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }

    wallsSize_ = wallsSize;

    qPainterRenderer_->changeWallsSize(wallsSize);

    rafraichir();

    if (typeLabyrinthe == Labyrinthe2Den3D && glLabyrinth)
    {
        glLabyrinth->tailleCaseChangee();
        return;
    }

    setFixedSize(transform.mapRect(QRectF(0, 0,
                                          getLongueur() * waysSize_.width() + (getLongueur() + 1) * wallsSize_.width(),
                                          getLargeur() * waysSize_.height() + (getLargeur() + 1) * wallsSize_.height()).toRect()).size());
    if (size().width() < QGuiApplication::screenAt(pos())->size().width() - 50
        && size().height() < QGuiApplication::screenAt(pos())->size().height() - 50)
    {
        scrollArea->setMinimumSize(size().width() + scrollArea->frameWidth() * 2, size().height() + scrollArea->frameWidth() * 2);
        QTimer::singleShot(50, this, SLOT(redimensionnerFenetrePrincipale()));
    }
    else
    {
        scrollArea->setMinimumSize(LONGUEURFACILE * WAYSSIZE.width() + (LONGUEURFACILE + 1) * WALLSSIZE.width() + scrollArea->frameWidth() * 2,
                                   LARGEURFACILE * WAYSSIZE.height() + (LARGEURFACILE + 1) * WALLSSIZE.height() + scrollArea->frameWidth() * 2);
        QTimer::singleShot(50, this, SLOT(maximiserFenetre()));
    }

    QRect rect = transform.mapRect(QRect((getEmplacementXJoueur() / 2) * waysSize_.width() + (getEmplacementXJoueur() / 2 + 1) * wallsSize_.width() - waysSize_.width() - wallsSize_.width(),
                                         (getEmplacementYJoueur() / 2) * waysSize_.height() + (getEmplacementYJoueur() / 2 + 1) * wallsSize_.height() - waysSize_.height() - wallsSize_.width(),
                                         waysSize_.width() + (waysSize_.width() + wallsSize_.width()) * 2,
                                         waysSize_.height() + (waysSize_.height() + wallsSize_.height()) * 2));
    scrollArea->ensureVisible(rect.x(), rect.y(), rect.width(), rect.height());
}

QLabyrinth::Niveau QLabyrinth::getNiveau() const
{
    return niveau;
}

void QLabyrinth::paintEvent(QPaintEvent *event)
{
    if (typeLabyrinthe == Labyrinthe2Den3D)
    {
        if (!(enConstruction || partieEnPause))
        {
            QWidget::paintEvent(event);
            return;
        }
    }
    else
        QWidget::paintEvent(event);

    QPainter painter(this);

    //painter.begin(this);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    QFont f(QString("Comic Sans MS"));
    f.setPointSize(30);
    f.setItalic(true);
    painter.setPen(textures[1].couleur);
    painter.setFont(f);

    QRect visibleRect = visibleRegion().boundingRect();//event->rect();

    if (enConstruction)
    {
        painter.fillRect(visibleRect, textures[0].couleur);

        painter.drawText(visibleRect, tr("Merci de patienter"), QTextOption(Qt::AlignCenter));
    }
    else if (partieEnPause)
    {
        painter.fillRect(visibleRect, textures[0].couleur);

        painter.drawText(visibleRect, tr("Partie en pause"), QTextOption(Qt::AlignCenter));
    }
    else
    {
        painter.setTransform(transform);

        QPainter painter(this);

        QPoint const offset(QRect(QPoint(), size()).center()
                            - QRect(QPoint(), qPainterRenderer_->size()).center());

        (*qPainterRenderer_)(&painter,
                             event->region().translated(-offset).intersected(QRect(QPoint(), qPainterRenderer_->size())),
                             offset);

        if (previousSize_ != qPainterRenderer_->size())
        {
            previousSize_ = qPainterRenderer_->size();

            setFixedSize((QRect(QPoint(), previousSize_)).size());
        }

        if ((modeLabyrinthe.mode & Obscurite) && !(getEmplacementXJoueur() == getXSortie() && getEmplacementYJoueur() == getYSortie()))
        {
            QRadialGradient radialGradient((qreal(getEmplacementXJoueur()) + 0.5) * qreal(waysSize_.height()), (qreal(getEmplacementYJoueur()) + 0.5) * qreal(waysSize_.height()), qMax(waysSize_.width(), waysSize_.height()) * modeLabyrinthe.rayonObscurite);
            radialGradient.setColorAt(0, QColor(0, 0, 0, 0));
            radialGradient.setColorAt(1, modeLabyrinthe.couleurObscurite);

            QRect r = visibleRect;
            r.setX(r.x() - 1);
            r.setY(r.y() - 1);
            r.setWidth(r.width() - 1);
            r.setHeight(r.height() - 1);
            painter.fillRect(r, radialGradient);
        }

        if (getEmplacementXJoueur() != getXSortie() || getEmplacementYJoueur() != getYSortie())
        {
            QRect rect = transform.mapRect(QRect(getEmplacementXJoueur() * waysSize_.width() - waysSize_.width() - wallsSize_.width(),
                                                 getEmplacementYJoueur() * waysSize_.height() - waysSize_.height() - wallsSize_.height(),
                                                 waysSize_.width() + (waysSize_.width() + wallsSize_.width()) * 2,
                                                 waysSize_.height() + (waysSize_.height() + wallsSize_.height()) * 2));
            scrollArea->ensureVisible(rect.x(), rect.y(), rect.width(), rect.height());
        }
    }

    painter.end();
}

void QLabyrinth::apercu(QPaintDevice *device, QRect visibleRect)
{
    QPainter painter(device);

    QPoint const offset(QRect(QPoint(), visibleRect.size()).center()
                        - QRect(QPoint(), qPainterRenderer_->size()).center());

    (*qPainterRenderer_)(&painter,
                         visibleRect.translated(-offset).intersected(QRect(QPoint(), qPainterRenderer_->size())),
                         offset);

    if (previousSize_ != qPainterRenderer_->size())
    {
        previousSize_ = qPainterRenderer_->size();

        setFixedSize((QRect(QPoint(), previousSize_)).size());
    }
}

void QLabyrinth::apercu(QPainter *painter, QRect visibleRect)
{
    if (visibleRect.isNull())
        visibleRect = visibleRegion().boundingRect();

    //Chargement des images
    QPixmap pixmapMotifFond = QPixmap(textures[0].motif).scaled(tailleCase);
    QPixmap pixmapImageFond = QPixmap(textures[0].image);
    QPixmap pixmapMotifMur = QPixmap(textures[1].motif).scaled(tailleCase);
    QPixmap pixmapImageMur = QPixmap(textures[1].image);
    QPixmap pixmapMotifParcours = QPixmap(textures[2].motif).scaled(tailleCase);
    QPixmap pixmapImageParcours = QPixmap(textures[2].image);

    //Dessin du fond
    if (textures[0].typeTexture == TextureMotif && !pixmapMotifFond.isNull())
    {
        for (int i = 0; i < getLargeur(); i++)
        {
            if (i * tailleCase.height() > visibleRect.y() + visibleRect.height())
                break;
            if ((i + 1) * tailleCase.height() > visibleRect.y())
            {
                for (int j = 0; j < getLongueur(); j++)
                {
                    if (j * tailleCase.width() > visibleRect.x() + visibleRect.width())
                        break;
                    if ((j + 1) * tailleCase.width() > visibleRect.x())
                    {
                        if (!labyrinth->grid().at(i, j))
                            painter->drawPixmap(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width(), tailleCase.height(), pixmapMotifFond);
                        else
                        {
                            if ((textures[1].typeTexture == TextureMotif && !pixmapMotifFond.isNull() && pixmapMotifFond.hasAlpha()) || (textures[1].typeTexture == TextureImage && !pixmapImageFond.isNull() && pixmapImageFond.hasAlpha()))
                                painter->drawPixmap(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width(), tailleCase.height(), pixmapMotifFond);
                        }
                    }
                }
            }
        }
    }
    else if (textures[0].typeTexture == TextureImage && !pixmapImageFond.isNull())
    {
        for (int i = 0; i < getLargeur() * tailleCase.height(); i += pixmapImageFond.height())
        {
            if (i > visibleRect.y() + visibleRect.height())
                break;
            int h = pixmapImageFond.height();
            if (i + pixmapImageFond.height() - getLargeur() * tailleCase.height() > 0)
                h = getLargeur() * tailleCase.height() - i;
            if (i + pixmapImageFond.width() > visibleRect.y())
            {
                for (int j = 0; j < getLongueur() * tailleCase.width(); j += pixmapImageFond.width())
                {
                    if (j > visibleRect.x() + visibleRect.width())
                        break;
                    int w = pixmapImageFond.width();
                    if (j + pixmapImageFond.width() > visibleRect.x())
                    {
                        if (j + pixmapImageFond.width() - getLongueur() * tailleCase.width() > 0)
                            w = getLongueur() * tailleCase.width() - j;
                        painter->drawPixmap(QRect(j, i, w, h), pixmapImageFond, QRect(0, 0, w, h));
                    }
                }
            }
        }
    }
    else// if (textures[0].typeTexture == TextureCouleur)
        painter->fillRect(0, 0, getLongueur() * tailleCase.width(), getLargeur() * tailleCase.height(), textures[0].couleur);

    //Dessin du mur
    if (textures[1].typeTexture == TextureMotif && !pixmapMotifMur.isNull())
    {
        for (int i = 0; i < getLargeur(); i++)
        {
            if (i * tailleCase.height() > visibleRect.y() + visibleRect.height())
                break;
            if ((i + 1) * tailleCase.height() > visibleRect.y())
            {
                for (int j = 0; j < getLongueur(); j++)
                {
                    if (j * tailleCase.width() > visibleRect.x() + visibleRect.width())
                        break;
                    if ((j + 1) * tailleCase.width() > visibleRect.x() && labyrinth->grid().at(i, j))
                        painter->drawPixmap(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width(), tailleCase.height(), pixmapMotifMur);
                }
            }
        }
    }
    else if (textures[1].typeTexture == TextureImage && !pixmapImageMur.isNull())
    {
        int decalageY = 0;
        int retrancheY = 0;
        for (int i = 0; i < getLargeur(); i++)
        {
            if (i * tailleCase.height() > visibleRect.y() + visibleRect.height())
                break;
            if (i * tailleCase.height() - decalageY >= pixmapImageMur.height())
                decalageY += pixmapImageMur.height();
            if ((i + 1) * tailleCase.height() - decalageY > pixmapImageMur.height())
                retrancheY = (i + 1) * tailleCase.height() - decalageY- pixmapImageMur.height();
            int decalageX = 0;
            int retrancheX = 0;
            if ((i + 1) * tailleCase.height() > visibleRect.y())
            {
                int valeurY = decalageY;
                for (int j = 0; j < getLongueur(); j++)
                {
                    if (j * tailleCase.width() > visibleRect.x() + visibleRect.width())
                        break;
                    if (j * tailleCase.width() - decalageX >= pixmapImageMur.width())
                        decalageX += pixmapImageMur.width();
                    if ((j + 1) * tailleCase.width() - decalageX > pixmapImageMur.width())
                        retrancheX = (j + 1) * tailleCase.width() - decalageX - pixmapImageMur.width();
                    if ((j + 1) * tailleCase.width() > visibleRect.x() && labyrinth->grid().at(i, j))
                    {
                        if (!retrancheX && !retrancheY)
                            painter->drawPixmap(QRect(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width(), tailleCase.height()), pixmapImageMur, QRect(j * tailleCase.width() - decalageX, i * tailleCase.height() - decalageY, tailleCase.width(), tailleCase.height()));
                        else
                        {
                            if (retrancheY)
                                decalageY = valeurY;
                            int valeurX = decalageX;
                            painter->drawPixmap(QRect(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width() - retrancheX, tailleCase.height() - retrancheY), pixmapImageMur, QRect(j * tailleCase.width() - decalageX, i * tailleCase.height() - decalageY, tailleCase.width() - retrancheX, tailleCase.height() - retrancheY));
                            if (retrancheX)
                                decalageX = (j + 1) * tailleCase.width();
                            painter->drawPixmap(QRect((j + 1) * tailleCase.width() - retrancheX, i * tailleCase.height(), retrancheX, tailleCase.height() - retrancheY), pixmapImageMur, QRect((j + 1) * tailleCase.width() - decalageX, i * tailleCase.height() - decalageY, retrancheX, tailleCase.height() - retrancheY));
                            if (retrancheY)
                            {
                                decalageY = (i + 1) * tailleCase.height();
                                if (retrancheX)
                                    decalageX = valeurX;
                            }
                            painter->drawPixmap(QRect(j * tailleCase.width(), (i + 1) * tailleCase.height() - retrancheY, tailleCase.width() - retrancheX, retrancheY), pixmapImageMur, QRect(j * tailleCase.width() - decalageX, (i + 1) * tailleCase.height() - decalageY, tailleCase.width() - retrancheX, retrancheY));
                            if (retrancheY && retrancheX)
                                decalageX = (j + 1) * tailleCase.width();
                            painter->drawPixmap(QRect((j + 1) * tailleCase.width() - retrancheX, (i + 1) * tailleCase.height() - retrancheY, retrancheX, retrancheY), pixmapImageMur, QRect((j + 1) * tailleCase.width() - decalageX, (i + 1) * tailleCase.height() - decalageY, retrancheX, retrancheY));
                            if (retrancheX)
                            {
                                decalageX -= retrancheX;
                                retrancheX = 0;
                            }
                        }
                    }
                    else if (retrancheX)
                    {
                        decalageX = (j + 1) * tailleCase.width() - retrancheX;
                        retrancheX = 0;
                    }
                }
                if (retrancheY)
                {
                    decalageY -= retrancheY;
                    retrancheY = 0;
                }
            }
            else if (retrancheY)
            {
                decalageY = (i + 1) * tailleCase.height() - retrancheY;
                retrancheY = 0;
            }
        }
    }
    else// if (textures[1].typeTexture == TextureCouleur)
    {
        for (int i = 0; i < getLargeur(); i++)
        {
            if (i * tailleCase.height() > visibleRect.y() + visibleRect.height())
                break;
            if ((i + 1) * tailleCase.height() > visibleRect.y())
            {
                for (int j = 0; j < getLongueur(); j++)
                {
                    if (j * tailleCase.width() > visibleRect.x() + visibleRect.width())
                        break;
                    if ((j + 1) * tailleCase.width() > visibleRect.x() && labyrinth->grid().at(i, j) == 1)
                        painter->fillRect(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width(), tailleCase.height(), textures[1].couleur);
                }
            }
        }
    }

    //Dessin du parcours
    if (textures[2].typeTexture == TextureMotif && !pixmapMotifParcours.isNull())
    {
        if (afficherTrace)
        {
            QImage image = QImage(textures[2].motif).scaled(tailleCase);
            if (!image.hasAlphaChannel())
                image = image.convertToFormat(QImage::Format_ARGB32, Qt::AutoColor | Qt::DiffuseDither | Qt::DiffuseAlphaDither | Qt::PreferDither);
            for (int y = 0; y < image.height(); y++)
            {
                QRgb *scanLine = (QRgb*)image.scanLine(y);
                for (int x = 0; x < image.width(); x++)
                {
                    *scanLine = qRgba(qRed(*scanLine), qGreen(*scanLine), qBlue(*scanLine), qAlpha(*scanLine) / 2);
                    scanLine++;
                }
            }
            pixmapMotifParcours = QPixmap::fromImage(image, Qt::AutoColor | Qt::DiffuseDither | Qt::DiffuseAlphaDither | Qt::PreferDither);
            /**Correct according trace
            for (int i = 0; i < getLargeur(); i++)
            {
                if (i * tailleCase.height() > visibleRect.y() + visibleRect.height())
                    break;
                if ((i + 1) * tailleCase.height() > visibleRect.y())
                {
                    for (int j = 0; j < getLongueur(); j++)
                    {
                        if (j * tailleCase.width() > visibleRect.x() + visibleRect.width())
                            break;
                        if ((j + 1) * tailleCase.width() > visibleRect.x() && (labyrinthe[i][j] == 2 || (labyrinthe[i][j] == 3 && !getEffacerChemin())))
                        {
                            if (i != getEmplacementYJoueur() || j != getEmplacementXJoueur())
                                painter->drawPixmap(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width(), tailleCase.height(), pixmapMotifParcours);
                            else
                            {
                                pixmapMotifParcours = QPixmap(textures[2].motif).scaled(tailleCase);
                                painter->drawPixmap(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width(), tailleCase.height(), pixmapMotifParcours);
                                pixmapMotifParcours = QPixmap::fromImage(image, Qt::AutoColor | Qt::DiffuseDither | Qt::DiffuseAlphaDither | Qt::PreferDither);
                            }
                        }
                    }
                }
            }**/
        }
        else
            painter->drawPixmap(getEmplacementXJoueur() * tailleCase.width(), getEmplacementYJoueur() * tailleCase.height(), tailleCase.width(), tailleCase.height(), pixmapMotifParcours);
    }
    else if (textures[2].typeTexture == TextureImage && !pixmapImageParcours.isNull())
    {
        if (afficherTrace)
        {
            QImage image = QImage(textures[2].image);
            if (!image.hasAlphaChannel())
                image = image.convertToFormat(QImage::Format_ARGB32, Qt::AutoColor | Qt::DiffuseDither | Qt::DiffuseAlphaDither | Qt::PreferDither);
            for (int y = 0; y < image.height(); y++)
            {
                QRgb *scanLine = (QRgb*)image.scanLine(y);
                for (int x = 0; x < image.width(); x++)
                {
                    *scanLine = qRgba(qRed(*scanLine), qGreen(*scanLine), qBlue(*scanLine), qAlpha(*scanLine) / 2);
                    scanLine++;
                }
            }
            pixmapImageParcours = QPixmap::fromImage(image, Qt::AutoColor | Qt::DiffuseDither | Qt::DiffuseAlphaDither | Qt::PreferDither);
            int decalageY = 0;
            int retrancheY = 0;
            /**Correct according trace
            for (int i = 0; i < getLargeur(); i++)
            {
                if (i * tailleCase.height() > visibleRect.y() + visibleRect.height())
                    break;
                if (i * tailleCase.height() - decalageY >= pixmapImageParcours.height())
                    decalageY += pixmapImageParcours.height();
                if ((i + 1) * tailleCase.height() - decalageY > pixmapImageParcours.height())
                    retrancheY = (i + 1) * tailleCase.height() - decalageY - pixmapImageParcours.height();
                int decalageX = 0;
                int retrancheX = 0;
                if ((i + 1) * tailleCase.height() > visibleRect.y())
                {
                    int valeurY = decalageY;
                    for (int j = 0; j < getLongueur(); j++)
                    {
                        if (j * tailleCase.width() > visibleRect.x() + visibleRect.width())
                            break;
                        if (j * tailleCase.width() - decalageX >= pixmapImageParcours.width())
                            decalageX += pixmapImageParcours.width();
                        if ((j + 1) * tailleCase.width() - decalageX > pixmapImageParcours.width())
                            retrancheX = (j + 1) * tailleCase.width() - decalageX - pixmapImageParcours.width();
                        if ((j + 1) * tailleCase.width() > visibleRect.x() && (labyrinthe[i][j] == 2 || (labyrinthe[i][j] == 3 && !getEffacerChemin())))
                        {
                            if (i == getEmplacementYJoueur() && j == getEmplacementXJoueur())
                                pixmapImageParcours = QPixmap(textures[2].image);
                            if (!retrancheX && !retrancheY)
                                painter->drawPixmap(QRect(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width(), tailleCase.height()), pixmapImageParcours, QRect(j * tailleCase.width() - decalageX, i * tailleCase.height() - decalageY, tailleCase.width(), tailleCase.height()));
                            else
                            {
                                if (retrancheY)
                                    decalageY = valeurY;
                                int valeurX = decalageX;
                                painter->drawPixmap(QRect(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width() - retrancheX, tailleCase.height() - retrancheY), pixmapImageParcours, QRect(j * tailleCase.width() - decalageX, i * tailleCase.height() - decalageY, tailleCase.width() - retrancheX, tailleCase.height() - retrancheY));
                                if (retrancheX)
                                    decalageX = (j + 1) * tailleCase.width();
                                painter->drawPixmap(QRect((j + 1) * tailleCase.width() - retrancheX, i * tailleCase.height(), retrancheX, tailleCase.height() - retrancheY), pixmapImageParcours, QRect((j + 1) * tailleCase.width() - decalageX, i * tailleCase.height() - decalageY, retrancheX, tailleCase.height() - retrancheY));
                                if (retrancheY)
                                {
                                    decalageY = (i + 1) * tailleCase.height();
                                    if (retrancheX)
                                        decalageX = valeurX;
                                }
                                painter->drawPixmap(QRect(j * tailleCase.width(), (i + 1) * tailleCase.height() - retrancheY, tailleCase.width() - retrancheX, retrancheY), pixmapImageParcours, QRect(j * tailleCase.width() - decalageX, (i + 1) * tailleCase.height() - decalageY, tailleCase.width() - retrancheX, retrancheY));
                                if (retrancheY && retrancheX)
                                    decalageX = (j + 1) * tailleCase.width();
                                painter->drawPixmap(QRect((j + 1) * tailleCase.width() - retrancheX, (i + 1) * tailleCase.height() - retrancheY, retrancheX, retrancheY), pixmapImageParcours, QRect((j + 1) * tailleCase.width() - decalageX, (i + 1) * tailleCase.height() - decalageY, retrancheX, retrancheY));
                                if (retrancheX)
                                {
                                    decalageX -= retrancheX;
                                    retrancheX = 0;
                                }
                            }
                            if (i == getEmplacementYJoueur() && j == getEmplacementXJoueur())
                                pixmapImageParcours = QPixmap::fromImage(image, Qt::AutoColor | Qt::DiffuseDither | Qt::DiffuseAlphaDither | Qt::PreferDither);
                        }
                        else if (retrancheX)
                        {
                            decalageX = (j + 1) * tailleCase.width() - retrancheX;
                            retrancheX = 0;
                        }
                    }
                    if (retrancheY)
                    {
                        decalageY -= retrancheY;
                        retrancheY = 0;
                    }
                }
                else if (retrancheY)
                {
                    decalageY = (i + 1) * tailleCase.height() - retrancheY;
                    retrancheY = 0;
                }
            }**/
        }
        else
        {
            bool b = false;
            int decalageY = 0;
            int retrancheY = 0;
            for (int i = 0; i < getLargeur(); i++)
            {
                if (i * tailleCase.height() > visibleRect.y() + visibleRect.height())
                    break;
                if (i * tailleCase.height() - decalageY >= pixmapImageParcours.height())
                    decalageY += pixmapImageParcours.height();
                if ((i + 1) * tailleCase.height() - decalageY > pixmapImageParcours.height())
                    retrancheY = (i + 1) * tailleCase.height() - decalageY - pixmapImageParcours.height();
                int decalageX = 0;
                int retrancheX = 0;
                if ((i + 1) * tailleCase.height() > visibleRect.y() && i == getEmplacementYJoueur())
                {
                    int valeurY = decalageY;
                    for (int j = 0; j < getLongueur(); j++)
                    {
                        if (j * tailleCase.width() > visibleRect.x() + visibleRect.width())
                            break;
                        if (j * tailleCase.width() - decalageX >= pixmapImageParcours.width())
                            decalageX += pixmapImageParcours.width();
                        if ((j + 1) * tailleCase.width() - decalageX > pixmapImageParcours.width())
                            retrancheX = (j + 1) * tailleCase.width() - decalageX - pixmapImageParcours.width();
                        if ((j + 1) * tailleCase.width() > visibleRect.x() && j == getEmplacementXJoueur())
                        {
                            if (!retrancheX && !retrancheY)
                                painter->drawPixmap(QRect(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width(), tailleCase.height()), pixmapImageParcours, QRect(j * tailleCase.width() - decalageX, i * tailleCase.height() - decalageY, tailleCase.width(), tailleCase.height()));
                            else
                            {
                                if (retrancheY)
                                    decalageY = valeurY;
                                int valeurX = decalageX;
                                painter->drawPixmap(QRect(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width() - retrancheX, tailleCase.height() - retrancheY), pixmapImageParcours, QRect(j * tailleCase.width() - decalageX, i * tailleCase.height() - decalageY, tailleCase.width() - retrancheX, tailleCase.height() - retrancheY));
                                if (retrancheX)
                                    decalageX = (j + 1) * tailleCase.width();
                                painter->drawPixmap(QRect((j + 1) * tailleCase.width() - retrancheX, i * tailleCase.height(), retrancheX, tailleCase.height() - retrancheY), pixmapImageParcours, QRect((j + 1) * tailleCase.width() - decalageX, i * tailleCase.height() - decalageY, retrancheX, tailleCase.height() - retrancheY));
                                if (retrancheY)
                                {
                                    decalageY = (i + 1) * tailleCase.height();
                                    if (retrancheX)
                                        decalageX = valeurX;
                                }
                                painter->drawPixmap(QRect(j * tailleCase.width(), (i + 1) * tailleCase.height() - retrancheY, tailleCase.width() - retrancheX, retrancheY), pixmapImageParcours, QRect(j * tailleCase.width() - decalageX, (i + 1) * tailleCase.height() - decalageY, tailleCase.width() - retrancheX, retrancheY));
                                if (retrancheY && retrancheX)
                                    decalageX = (j + 1) * tailleCase.width();
                                painter->drawPixmap(QRect((j + 1) * tailleCase.width() - retrancheX, (i + 1) * tailleCase.height() - retrancheY, retrancheX, retrancheY), pixmapImageParcours, QRect((j + 1) * tailleCase.width() - decalageX, (i + 1) * tailleCase.height() - decalageY, retrancheX, retrancheY));
                                if (retrancheX)
                                {
                                    decalageX -= retrancheX;
                                    retrancheX = 0;
                                }
                            }
                            b = true;

                            break;
                        }
                        else if (retrancheX)
                        {
                            decalageX = (j + 1) * tailleCase.width() - retrancheX;
                            retrancheX = 0;
                        }
                    }
                    if (retrancheY)
                    {
                        decalageY -= retrancheY;
                        retrancheY = 0;
                    }
                    if (b)
                        break;
                }
                else if (retrancheY)
                {
                    decalageY = (i + 1) * tailleCase.height() - retrancheY;
                    retrancheY = 0;
                }
            }
        }
    }
    else// if (textures[2].typeTexture == TextureCouleur)
    {
        QColor couleur = textures[2].couleur;
        if (afficherTrace)
        {
            int a = couleur.alpha();
            couleur.setAlpha(a/2);
            /**Correct according trace
            for (int i = 0; i < getLargeur(); i++)
            {
                if (i * tailleCase.height() > visibleRect.y() + visibleRect.height())
                    break;
                if ((i + 1) * tailleCase.height() > visibleRect.y())
                {
                    for (int j = 0; j < getLongueur(); j++)
                    {
                        if (j * tailleCase.width() > visibleRect.x() + visibleRect.width())
                            break;
                        if ((j + 1) * tailleCase.width() > visibleRect.x() && (labyrinthe[i][j] == 2 || (labyrinthe[i][j] == 3 && !getEffacerChemin())))
                        {
                            if (i != getEmplacementYJoueur() || j != getEmplacementXJoueur())
                                painter->fillRect(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width(), tailleCase.height(), couleur);
                            else
                            {
                                couleur.setAlpha(a);
                                painter->fillRect(j * tailleCase.width(), i * tailleCase.height(), tailleCase.width(), tailleCase.height(), couleur);
                                couleur.setAlpha(a/2);
                            }
                        }
                    }
                }
            }**/
        }
        else
            painter->fillRect(getEmplacementXJoueur() * tailleCase.width(), getEmplacementYJoueur() * tailleCase.height(), tailleCase.width(), tailleCase.height(), couleur);
    }

    painter->fillRect(getXEntree() * tailleCase.width(), getYEntree() * tailleCase.height(), tailleCase.width(), tailleCase.height(), QColor(0, 255, 0, 128));
    painter->fillRect(getXSortie() * tailleCase.width(), getYSortie() * tailleCase.height(), tailleCase.width(), tailleCase.height(), QColor(255, 0, 0, 128));
}

void QLabyrinth::routineDeplacement1()
{
    if (!partieEnCours)
    {
        partieEnCours = true;
        emit partieCommencee();
        if ((modeLabyrinthe.mode & Rotation) && !modeLabyrinthe.rotationFixe)
        {
            if (modeLabyrinthe.intervalleRotationAuHasard)
            {
                intervalleRotation = rand() % (INTERVALLEROTATIONMAX - INTERVALLEROTATIONMIN + 1) + INTERVALLEROTATIONMIN;
                indexTimerRotation = startTimer(intervalleRotation);
            }
            else
                indexTimerRotation = startTimer(modeLabyrinthe.intervalleRotation);
        }
        if (modeLabyrinthe.mode & Distorsion)
        {
            if (!modeLabyrinthe.distorsionVerticaleFixe)
            {
                if (modeLabyrinthe.intervalleDistorsionVerticaleAuHasard)
                {
                    intervalleDistorsionVerticale = rand() % (INTERVALLEDISTORSIONVERTICALEMAX - INTERVALLEDISTORSIONVERTICALEMIN + 1) + INTERVALLEDISTORSIONVERTICALEMIN;
                    indexTimerDistorsionVerticale = startTimer(intervalleDistorsionVerticale);
                }
                else
                    indexTimerDistorsionVerticale = startTimer(modeLabyrinthe.intervalleDistorsionVerticale);
            }
            if (!distorsionSynchronisee && !modeLabyrinthe.distorsionHorizontaleFixe)
            {
                if (modeLabyrinthe.intervalleDistorsionHorizontaleAuHasard)
                {
                    intervalleDistorsionHorizontale = rand() % (INTERVALLEDISTORSIONHORIZONTALEMAX - INTERVALLEDISTORSIONHORIZONTALEMIN + 1) + INTERVALLEDISTORSIONHORIZONTALEMIN;
                    indexTimerDistorsionHorizontale = startTimer(intervalleDistorsionHorizontale);
                }
                else
                    indexTimerDistorsionHorizontale = startTimer(modeLabyrinthe.intervalleDistorsionHorizontale);
            }
        }
        if (modeLabyrinthe.mode & Cisaillement)
        {
            if (!modeLabyrinthe.cisaillementVerticalFixe)
            {
                if (modeLabyrinthe.intervalleCisaillementVerticalAuHasard)
                {
                    intervalleCisaillementVertical = rand() % (INTERVALLECISAILLEMENTVERTICALMAX - INTERVALLECISAILLEMENTVERTICALMIN + 1) + INTERVALLECISAILLEMENTVERTICALMIN;
                    indexTimerCisaillementVertical = startTimer(intervalleCisaillementVertical);
                }
                else
                    indexTimerCisaillementVertical = startTimer(modeLabyrinthe.intervalleCisaillementVertical);
            }
            if (!cisaillementSynchronise && !modeLabyrinthe.cisaillementHorizontalFixe)
            {
                if (modeLabyrinthe.intervalleCisaillementHorizontalAuHasard)
                {
                    intervalleCisaillementHorizontal = rand() % (INTERVALLECISAILLEMENTHORIZONTALMAX - INTERVALLECISAILLEMENTHORIZONTALMIN + 1) + INTERVALLECISAILLEMENTHORIZONTALMIN;
                    indexTimerCisaillementHorizontal = startTimer(intervalleCisaillementHorizontal);
                }
                else
                    indexTimerCisaillementHorizontal = startTimer(modeLabyrinthe.intervalleCisaillementHorizontal);
            }
        }

        QPalette p = scrollArea->palette();

        if (modeLabyrinthe.mode & Obscurite)
            p.setColor(QPalette::Window, modeLabyrinthe.couleurObscurite);
        else
            p.setColor(QPalette::Window, textures[0].couleur);

        scrollArea->setPalette(p);
    }

    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }
}

void QLabyrinth::routineDeplacement2()
{
    QRect visibleRect = visibleRegion().boundingRect();

    QRect r = visibleRect.intersected(QRect((getEmplacementXJoueur() / 2 + 1) * waysSize_.width() + (getEmplacementXJoueur() / 2) * wallsSize_.width(),
                                            (getEmplacementYJoueur() / 2 + 1) * waysSize_.height() + (getEmplacementXJoueur() / 2) * wallsSize_.height(),
                                            waysSize_.width(),
                                            waysSize_.height()));
    if (r.width() != waysSize_.width())
        scrollArea->horizontalScrollBar()->setValue(scrollArea->horizontalScrollBar()->value() + scrollArea->horizontalScrollBar()->pageStep() / 2);
    if (r.height() != waysSize_.height())
        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->value() + scrollArea->verticalScrollBar()->pageStep() / 2);
    rafraichir();
    if (getEmplacementXJoueur() == getXSortie() && getEmplacementYJoueur() == getYSortie())
    {
        if (indexTimerRotation)
        {
            killTimer(indexTimerRotation);
            indexTimerRotation = 0;
        }
        if (indexTimerDistorsionVerticale)
        {
            killTimer(indexTimerDistorsionVerticale);
            indexTimerDistorsionVerticale = 0;
        }
        if (indexTimerDistorsionHorizontale)
        {
            killTimer(indexTimerDistorsionHorizontale);
            indexTimerDistorsionHorizontale = 0;
        }
        if (indexTimerCisaillementVertical)
        {
            killTimer(indexTimerCisaillementVertical);
            indexTimerCisaillementVertical = 0;
        }
        if (indexTimerCisaillementHorizontal)
        {
            killTimer(indexTimerCisaillementHorizontal);
            indexTimerCisaillementHorizontal = 0;
        }
        QPalette p = scrollArea->palette();
        p.setColor(QPalette::Window, textures[0].couleur);
        scrollArea->setPalette(p);
        emit partieTerminee();
    }
}

void QLabyrinth::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);

    if (modeEcranDeVeille)
        quitterModeEcranDeVeille();

    if (typeLabyrinthe == Labyrinthe2Den3D || enResolution)
        return;

    if (partieEnPause || (getEmplacementXJoueur() == getXSortie() && getEmplacementYJoueur() == getYSortie()))
        return;

    int key = event->key();

    if (modeLabyrinthe.mode & Rotation)
    {
        switch (numeroQuart)
        {
            case 1:
            default:
                break;
            case 2:
                switch (key)
                {
                    case Qt::Key_Right:
                        key = Qt::Key_Up;
                        break;
                    case Qt::Key_Left:
                        key = Qt::Key_Down;
                        break;
                    case Qt::Key_Up:
                        key = Qt::Key_Left;
                        break;
                    case Qt::Key_Down:
                        key = Qt::Key_Right;
                        break;
                    default:
                        break;
                }
                break;
            case 3:
                switch (key)
                {
                    case Qt::Key_Right:
                        key = Qt::Key_Left;
                        break;
                    case Qt::Key_Left:
                        key = Qt::Key_Right;
                        break;
                    case Qt::Key_Up:
                        key = Qt::Key_Down;
                        break;
                    case Qt::Key_Down:
                        key = Qt::Key_Up;
                        break;
                    default:
                        break;
                }
                break;
            case 4:
                switch (key)
                {
                    case Qt::Key_Right:
                        key = Qt::Key_Down;
                        break;
                    case Qt::Key_Left:
                        key = Qt::Key_Up;
                        break;
                    case Qt::Key_Up:
                        key = Qt::Key_Right;
                        break;
                    case Qt::Key_Down:
                        key = Qt::Key_Left;
                        break;
                    default:
                        break;
                }
                break;
        }
    }
	
	QKeyEvent modifiedEvent(event->type(), key, event->modifiers(), event->text(), event->isAutoRepeat(), event->count());

    routineDeplacement1();
    qKeyPress->filter(&modifiedEvent);
	emit deplacementChange();
	routineDeplacement2();
}

void QLabyrinth::mousePressEvent(QMouseEvent *event)
{
    if (modeEcranDeVeille)
        quitterModeEcranDeVeille();

    QWidget::mousePressEvent(event);

    if (typeLabyrinthe == Labyrinthe2Den3D || enResolution)
        return;

    event->accept();

    if (event->button() & Qt::LeftButton)
    {
        anciennePosSouris = event->pos();

        mouvementSouris(event->pos());
    }
    else if (event->button() & Qt::RightButton)
    {
        menu->clear();
        menu->addActions(actions());
        menu->popup(QCursor::pos());
    }
}

void QLabyrinth::mouseMoveEvent(QMouseEvent *event)
{
    if (modeEcranDeVeille)
        quitterModeEcranDeVeille();

    QWidget::mouseMoveEvent(event);

    if (typeLabyrinthe == Labyrinthe2Den3D || enResolution)
        return;

    if (event->buttons() & Qt::LeftButton)
    {
        QPoint const p = transform.inverted().map(event->pos());
        int const x = p.x() / ((waysSize_.width() + wallsSize_.width()) / 2);
        int const y = p.y() / ((waysSize_.height() + wallsSize_.height()) / 2);

        QPoint posSouris = event->pos();

        if ((x > getEmplacementXJoueur() && getEmplacementXJoueur() + 1 < getLongueur()
             && labyrinth->grid().at(getEmplacementYJoueur(), getEmplacementXJoueur() + 1))
            || (x < getEmplacementXJoueur() && getEmplacementXJoueur() - 1 >= 0
                && labyrinth->grid().at(getEmplacementYJoueur(), getEmplacementXJoueur() - 1)))
            posSouris.rx() = anciennePosSouris.x();
        if ((y > getEmplacementYJoueur() && getEmplacementYJoueur() + 1 < getLargeur()
             && labyrinth->grid().at(getEmplacementYJoueur() + 1, getEmplacementXJoueur()))
            || (y < getEmplacementYJoueur() && getEmplacementYJoueur() - 1 >= 0
                && labyrinth->grid().at(getEmplacementYJoueur() - 1, getEmplacementXJoueur())))
            posSouris.ry() = anciennePosSouris.y();

        if (posSouris != event->pos())
            QCursor::setPos(mapToGlobal(anciennePosSouris));

        mouvementSouris(event->pos());

        if (!(x == getEmplacementXJoueur() && y == getEmplacementYJoueur()))
            QCursor::setPos(mapToGlobal(anciennePosSouris));

        anciennePosSouris = event->pos();
    }
}

void QLabyrinth::mouvementSouris(const QPoint &point)
{
    QPoint const p = transform.inverted().map(point);
    int x = p.x() / (waysSize_.width() + wallsSize_.width()) * 2;
    int y = p.y() / (waysSize_.height() + wallsSize_.height()) * 2;

    if (p.x() - x * (waysSize_.width() + wallsSize_.width()) / 2 > wallsSize_.width())
        ++x;
    if (p.y() - y * (waysSize_.height() + wallsSize_.height()) / 2 > wallsSize_.height())
        ++y;

    if (x == getEmplacementXJoueur() && y == getEmplacementYJoueur())
        return;

    if (x == getEmplacementXJoueur())
    {
        routineDeplacement1();
        int const sens = (y - getEmplacementYJoueur()) / qAbs(y - getEmplacementYJoueur());
        labyrinth->player(playerId).move(sens == 1 ? Labyrinth2d::Down : Labyrinth2d::Up,
            [this] (std::chrono::milliseconds const& ms) -> void { QTest::qWait(ms); this->update(); },
                                         qAbs(y - getEmplacementYJoueur()));
		emit deplacementChange();
		routineDeplacement2();
    }
    else if (y == getEmplacementYJoueur())
    {
        routineDeplacement1();
        int const sens = (x - getEmplacementXJoueur()) / qAbs(x - getEmplacementXJoueur());
        labyrinth->player(playerId).move(sens == 1 ? Labyrinth2d::Right : Labyrinth2d::Left,
                                         [this] (std::chrono::milliseconds const& ms) -> void { QTest::qWait(ms); this->update(); },
                                         qAbs(x - getEmplacementXJoueur()));
		emit deplacementChange();
		routineDeplacement2();
    }
}

void QLabyrinth::nouveau(Niveau n, int longueurLabyrinthe, int largeurLabyrinthe, Algorithme a, TypeLabyrinthe type, FormeLabyrinthe forme, ModeLabyrinthe *mode)
{
    if (indexTimerRotation)
    {
        killTimer(indexTimerRotation);
        indexTimerRotation = 0;
    }
    if (indexTimerDistorsionVerticale)
    {
        killTimer(indexTimerDistorsionVerticale);
        indexTimerDistorsionVerticale = 0;
    }
    if (indexTimerDistorsionHorizontale)
    {
        killTimer(indexTimerDistorsionHorizontale);
        indexTimerDistorsionHorizontale = 0;
    }
    if (indexTimerCisaillementVertical)
    {
        killTimer(indexTimerCisaillementVertical);
        indexTimerCisaillementVertical = 0;
    }
    if (indexTimerCisaillementHorizontal)
    {
        killTimer(indexTimerCisaillementHorizontal);
        indexTimerCisaillementHorizontal = 0;
    }

    arreterResolution();

    switch (n)
    {
        case Facile:
            longueurLabyrinthe = LONGUEURFACILE;
            largeurLabyrinthe = LARGEURFACILE;
            break;
        case Moyen:
            longueurLabyrinthe = LONGUEURMOYEN;
            largeurLabyrinthe = LARGEURMOYEN;
            break;
        case Difficile:
            longueurLabyrinthe = LONGUEURDIFFICILE;
            largeurLabyrinthe = LARGEURDIFFICILE;
            break;
        case Personnalise:
            if (longueurLabyrinthe < 2)
                longueurLabyrinthe = 2;
            if (largeurLabyrinthe < 2)
                largeurLabyrinthe = 2;
            if (longueurLabyrinthe == LONGUEURFACILE && largeurLabyrinthe == LARGEURFACILE)
                n = Facile;
            else if (longueurLabyrinthe == LONGUEURMOYEN && largeurLabyrinthe == LARGEURMOYEN)
                n = Moyen;
            else if (longueurLabyrinthe == LONGUEURDIFFICILE && largeurLabyrinthe == LARGEURDIFFICILE)
                n = Difficile;
            break;
        default:
            break;
    }

    bool b = (getLongueur() != longueurLabyrinthe || getLargeur() != largeurLabyrinthe);

    //longueur = longueurLabyrinthe;
    //largeur = largeurLabyrinthe;
    //xEntree = 0;
    //yEntree = 1;
    //xSortie = longueur - 1;
    //ySortie = largeur - 2;
    //emplacementXJoueur = xEntree;
    //emplacementYJoueur = yEntree;
    enResolution = false;
    partieEnCours = false;
    partieEnPause = false;
    niveau = n;
    algorithme = a;
    angle = 0;
    numeroQuart = 1;
    nombreDeplacement = 0;
    typeLabyrinthe = type;
    formeLabyrinthe = forme;
    unsigned int ancienMode = modeLabyrinthe.mode;
    sensRotation = modeLabyrinthe.sensRotation;
    distorsionSynchronisee = modeLabyrinthe.distorsionSynchronisee;
    pourcentageDistorsionVerticaleCourant = 0;
    pourcentageDistorsionHorizontaleCourant = 0;
    cisaillementSynchronise = modeLabyrinthe.cisaillementSynchronise;
    pourcentageCisaillementVerticalCourant = 0;
    pourcentageCisaillementHorizontalCourant = 0;
    if (mode)
    {
        modeLabyrinthe.mode = mode->mode;
        modeLabyrinthe.angleRotation = mode->angleRotation;
        modeLabyrinthe.angleRotationAuHasard = mode->angleRotationAuHasard;
        modeLabyrinthe.rotationFixe = mode->rotationFixe;
        modeLabyrinthe.intervalleRotation = mode->intervalleRotation;
        modeLabyrinthe.intervalleRotationAuHasard = mode->intervalleRotationAuHasard;
        modeLabyrinthe.sensRotation = mode->sensRotation;
        modeLabyrinthe.sensRotationAuHasard = mode->sensRotationAuHasard;
        modeLabyrinthe.couleurObscurite = mode->couleurObscurite;
        modeLabyrinthe.rayonObscurite = mode->rayonObscurite;
        modeLabyrinthe.pourcentageDistorsionVerticale = mode->pourcentageDistorsionVerticale;
        modeLabyrinthe.pourcentageDistorsionHorizontale = mode->pourcentageDistorsionHorizontale;
        modeLabyrinthe.pourcentageDistorsionVerticaleAuHasard = mode->pourcentageDistorsionVerticaleAuHasard;
        modeLabyrinthe.pourcentageDistorsionHorizontaleAuHasard = mode->pourcentageDistorsionHorizontaleAuHasard;
        modeLabyrinthe.intervalleDistorsionVerticale = mode->intervalleDistorsionVerticale;
        modeLabyrinthe.intervalleDistorsionHorizontale = mode->intervalleDistorsionHorizontale;
        modeLabyrinthe.intervalleDistorsionVerticaleAuHasard = mode->intervalleDistorsionVerticaleAuHasard;
        modeLabyrinthe.intervalleDistorsionHorizontaleAuHasard = mode->intervalleDistorsionHorizontaleAuHasard;
        modeLabyrinthe.distorsionVerticaleFixe = mode->distorsionVerticaleFixe;
        modeLabyrinthe.distorsionHorizontaleFixe = mode->distorsionHorizontaleFixe;
        modeLabyrinthe.distorsionSynchronisee = mode->distorsionSynchronisee;
        modeLabyrinthe.distorsionSynchroniseeAuHasard = mode->distorsionSynchroniseeAuHasard;
        modeLabyrinthe.pourcentageCisaillementVertical = mode->pourcentageCisaillementVertical;
        modeLabyrinthe.pourcentageCisaillementHorizontal = mode->pourcentageCisaillementHorizontal;
        modeLabyrinthe.pourcentageCisaillementVerticalAuHasard = mode->pourcentageCisaillementVerticalAuHasard;
        modeLabyrinthe.pourcentageCisaillementHorizontalAuHasard = mode->pourcentageCisaillementHorizontalAuHasard;
        modeLabyrinthe.intervalleCisaillementVertical = mode->intervalleCisaillementVertical;
        modeLabyrinthe.intervalleCisaillementHorizontal = mode->intervalleCisaillementHorizontal;
        modeLabyrinthe.intervalleCisaillementVerticalAuHasard = mode->intervalleCisaillementVerticalAuHasard;
        modeLabyrinthe.intervalleCisaillementHorizontalAuHasard = mode->intervalleCisaillementHorizontalAuHasard;
        modeLabyrinthe.cisaillementVerticalFixe = mode->cisaillementVerticalFixe;
        modeLabyrinthe.cisaillementHorizontalFixe = mode->cisaillementHorizontalFixe;
        modeLabyrinthe.cisaillementSynchronise = mode->cisaillementSynchronise;
        modeLabyrinthe.cisaillementSynchroniseAuHasard = mode->cisaillementSynchroniseAuHasard;
    }
    if ((modeLabyrinthe.mode & Rotation))
    {
        if (rand() % 2 && modeLabyrinthe.sensRotationAuHasard)
            sensRotation *= -1;
        if (modeLabyrinthe.rotationFixe)
        {
            if (modeLabyrinthe.angleRotationAuHasard)
                angle += sensRotation * rand() % (359 + 1);
            else
                angle += sensRotation * modeLabyrinthe.angleRotation;
            if (angle > 359)
                angle -= 360;
            else if (angle < 0)
                angle += 359;
            if (angle < 45 || angle >= 315)
                numeroQuart = 1;
            else if (angle >= 45 && angle < 135)
                numeroQuart = 2;
            else if (angle >= 135 && angle < 225)
                numeroQuart = 3;
            else
                numeroQuart = 4;
        }
    }
    if (modeLabyrinthe.mode & Distorsion)
    {
        if (modeLabyrinthe.distorsionSynchroniseeAuHasard)
            distorsionSynchronisee = bool(rand() % 2);
        if (modeLabyrinthe.pourcentageDistorsionVerticaleAuHasard)
            pourcentageDistorsionVerticale = rand() % 101;
        if (modeLabyrinthe.pourcentageDistorsionHorizontaleAuHasard)
            pourcentageDistorsionHorizontale = rand() % 101;
    }
    if (modeLabyrinthe.mode & Cisaillement)
    {
        if (modeLabyrinthe.cisaillementSynchroniseAuHasard)
            cisaillementSynchronise = bool(rand() % 2);
        if (modeLabyrinthe.pourcentageCisaillementVerticalAuHasard)
            pourcentageCisaillementVertical = rand() % 101;
        if (modeLabyrinthe.pourcentageCisaillementHorizontalAuHasard)
            pourcentageCisaillementHorizontal = rand() % 101;
    }

    QPalette p = scrollArea->palette();

    if (modeLabyrinthe.mode & Obscurite)
        p.setColor(QPalette::Window, modeLabyrinthe.couleurObscurite);
    else
        p.setColor(QPalette::Window, textures[0].couleur);

    scrollArea->setPalette(p);

    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }

    if (glLabyrinth)
        glLabyrinth->hide();

    enConstruction = true;
    rafraichir();
    QApplication::processEvents();
/**
    QTime time;
    time.start();
**/
	if (labyrinth)
		delete labyrinth;

    labyrinth = new Labyrinth2d::Labyrinth(largeurLabyrinthe, longueurLabyrinthe);
    qPainterRenderer_ = new Labyrinth2d::Renderer::QPainter(*labyrinth);
    qPainterRenderer_->setMargins(QMarginsF());
    qPainterRenderer_->changeWallsSize(wallsSize_);
    qPainterRenderer_->changeWaysSize(waysSize_);
    playerId = labyrinth->addPlayer(0, 0, {labyrinth->grid().rows() - 1}, {labyrinth->grid().columns() - 1}, true);
    qKeyPress = new Labyrinth2d::Mover::QKeyPress(*labyrinth, playerId);

    size_t const cycleOperations(0 * 1);
    std::chrono::milliseconds const cyclePause(0 * 1 * 1);
    size_t const seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine g(seed);
	//std::random_device g;

    auto const sleep{
        [this] (std::chrono::milliseconds const& ms) -> void
        {
            QTest::qWait(ms);
            this->update();
        }
    };

    using namespace Labyrinth2d;

    switch (algorithme)
    {
        case FirstDepthSearch:
        {
			Algorithm::WaySearch wsa(Algorithm::WaySearch::DepthFirstSearch);
            labyrinth->generate(g, wsa, sleep, cycleOperations, cyclePause, nullptr);
            break;
        }
        case CellFusion:
        {
            Algorithm::CellFusion cfa;
            labyrinth->generate(g, cfa, sleep, cycleOperations, cyclePause, nullptr);
            break;
        }
        case RecursiveDivision:
        {
            Algorithm::RecursiveDivision rda;
            labyrinth->generate(g, rda, sleep, cycleOperations, cyclePause, nullptr);
            break;
        }
        case PrimsAlgorithm:
        {
            Algorithm::WaySearch wsa(Algorithm::WaySearch::Prim);
            labyrinth->generate(g, wsa, sleep, cycleOperations, cyclePause, nullptr);
            break;
        }
        case HuntAndKillAlgorithm:
        {
			Algorithm::WaySearch wsa(Algorithm::WaySearch::HuntAndKill);
            labyrinth->generate(g, wsa, sleep, cycleOperations, cyclePause, nullptr);
            break;
        }
        case GrowingTreeAlgorithm:
        {
			Algorithm::WaySearch wsa(Algorithm::WaySearch::DepthFirstSearch);
            labyrinth->generate(g, wsa, sleep, cycleOperations, cyclePause, nullptr);
            break;
        }
        case FractaleAlgorithm:
        {
            Algorithm::Fractal fa;
            labyrinth->generate(g, fa, sleep, cycleOperations, cyclePause, nullptr);
            break;
        }
        case HomeMadeAlgorithm1:
            //construireHomeMadeAlgorithm1();
            break;
        case HomeMadeAlgorithm2:
            //construireHomeMadeAlgorithm2();
            break;
        case HomeMadeAlgorithm3:
            //construireHomeMadeAlgorithm3();
            break;
        default:
        {
			Algorithm::WaySearch wsa(Algorithm::WaySearch::DepthFirstSearch);
            labyrinth->generate(g, wsa, sleep, cycleOperations, cyclePause, nullptr);
            break;
        }
    }

    enConstruction = false;
/**
    qDebug() << QTime().addMSecs(time.elapsed()).toString(tr("hh:mm:ss"));
**/
    if (typeLabyrinthe == Labyrinthe2Den3D)
    {
        if (!glLabyrinth)
        {
            glLabyrinth = new GLLabyrinth(this);
            glLabyrinth->rechargerTextures();
            QHBoxLayout *hBoxLayout = new QHBoxLayout;
            hBoxLayout->setContentsMargins(0, 0, 0, 0);
            hBoxLayout->addWidget(glLabyrinth);
            setLayout(hBoxLayout);

            connect(glLabyrinth, SIGNAL(deplacementJoueur(int, int)), this, SLOT(joueurDeplace(int, int)));

            setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

            scrollArea->setMinimumSize(640, 480);
            scrollArea->setWidgetResizable(true);
            QTimer::singleShot(300, this, SLOT(redimensionnerFenetrePrincipale()));
        }

        glLabyrinth->setLabyrinth(labyrinth);
        glLabyrinth->show();
    }
    else
    {
        if (glLabyrinth)
        {
            glLabyrinth->deleteLater();
            glLabyrinth = 0;
            layout()->deleteLater();
            scrollArea->setWidgetResizable(false);
            setFixedSize(transform.mapRect(QRectF(0, 0,
                                                  getLongueur() * waysSize_.width() + (getLongueur() + 1) * wallsSize_.width(),
                                                  getLargeur() * waysSize_.height() + (getLargeur() + 1) * wallsSize_.height()).toRect()).size());
            if (size().width() < QGuiApplication::screenAt(pos())->size().width() - 50
                && size().height() < QGuiApplication::screenAt(pos())->size().height() - 50)
            {
                scrollArea->setMinimumSize(size().width() + scrollArea->frameWidth() * 2,
                                           size().height() + scrollArea->frameWidth() * 2);
                QTimer::singleShot(50, this, SLOT(redimensionnerFenetrePrincipale()));
            }
            else
            {
                scrollArea->setMinimumSize(LONGUEURFACILE * WAYSSIZE.width() + (LONGUEURFACILE + 1) * WALLSSIZE.width() + scrollArea->frameWidth() * 2,
                                           LARGEURFACILE * WAYSSIZE.height() + (LARGEURFACILE + 1) * WALLSSIZE.height() + scrollArea->frameWidth() * 2);
                QTimer::singleShot(50, this, SLOT(maximiserFenetre()));
            }
        }

        setFixedSize(getLongueur() * waysSize_.width() + (getLongueur() + 1) * wallsSize_.width(),
                     getLargeur() * waysSize_.height() + (getLargeur() + 1) * wallsSize_.height());
        calculerTransformation();

        if (b || (modeLabyrinthe.mode & Rotation) || ((ancienMode & Rotation) && !(modeLabyrinthe.mode & Rotation)))
        {
            setFixedSize(transform.mapRect(QRectF(0, 0,
                                                  getLongueur() * waysSize_.width() + (getLongueur() + 1) * wallsSize_.width(),
                                                  getLargeur() * waysSize_.height() + (getLargeur() + 1) * wallsSize_.height()).toRect()).size());
            if (size().width() < QGuiApplication::screenAt(pos())->size().width() - 50
                && size().height() < QGuiApplication::screenAt(pos())->size().height() - 50)
            {
                scrollArea->setMinimumSize(size().width() + scrollArea->frameWidth() * 2,
                                           size().height() + scrollArea->frameWidth() * 2);
                QTimer::singleShot(50, this, SLOT(redimensionnerFenetrePrincipale()));
            }
            else
            {
                scrollArea->setMinimumSize(LONGUEURFACILE * WAYSSIZE.width() + (LONGUEURFACILE + 1) * WALLSSIZE.width() + scrollArea->frameWidth() * 2,
                                           LARGEURFACILE * WAYSSIZE.height() + (LARGEURFACILE + 1) * WALLSSIZE.height() + scrollArea->frameWidth() * 2);
                QTimer::singleShot(50, this, SLOT(maximiserFenetre()));
            }
        }

        QRect rect = transform.mapRect(QRect((getEmplacementXJoueur() / 2) * waysSize_.width() + (getEmplacementXJoueur() / 2 + 1) * wallsSize_.width() - waysSize_.width() - wallsSize_.width(),
                                             (getEmplacementYJoueur() / 2) * waysSize_.height() + (getEmplacementYJoueur() / 2 + 1) * wallsSize_.height() - waysSize_.height() - wallsSize_.width(),
                                             waysSize_.width() + (waysSize_.width() + wallsSize_.width()) * 2,
                                             waysSize_.height() + (waysSize_.height() + wallsSize_.height()) * 2));
        scrollArea->ensureVisible(rect.x(), rect.y(), rect.width(), rect.height());
    }

    rafraichir();
    activer();
}

void QLabyrinth::redimensionnerFenetrePrincipale()
{
    if (scrollArea->parentWidget()->parentWidget())
    {
        bool fullScreen = scrollArea->parentWidget()->parentWidget()->isFullScreen();
        bool maximized = scrollArea->parentWidget()->parentWidget()->isMaximized();
        scrollArea->parentWidget()->parentWidget()->showNormal();
        scrollArea->parentWidget()->parentWidget()->resize(scrollArea->parentWidget()->parentWidget()->minimumSize());
        scrollArea->parentWidget()->parentWidget()->move(QRect(QPoint(), QGuiApplication::screenAt(pos())->size()).center() - scrollArea->parentWidget()->parentWidget()->rect().center());
        if (maximized)
            scrollArea->parentWidget()->parentWidget()->setWindowState(scrollArea->parentWidget()->parentWidget()->windowState() | Qt::WindowMaximized);
        if (fullScreen)
            scrollArea->parentWidget()->parentWidget()->setWindowState(scrollArea->parentWidget()->parentWidget()->windowState() | Qt::WindowFullScreen);
        scrollArea->parentWidget()->parentWidget()->raise();
    }

    activer();
}

void QLabyrinth::maximiserFenetre()
{
    if (scrollArea->parentWidget()->parentWidget())
    {
        bool fullScreen = scrollArea->parentWidget()->parentWidget()->isFullScreen();
        bool maximized = scrollArea->parentWidget()->parentWidget()->isMaximized();
        scrollArea->parentWidget()->parentWidget()->showNormal();
        scrollArea->parentWidget()->parentWidget()->resize(scrollArea->parentWidget()->parentWidget()->minimumSize());
        scrollArea->parentWidget()->parentWidget()->move(QRect(QPoint(), QGuiApplication::screenAt(pos())->size()).center() - scrollArea->parentWidget()->parentWidget()->rect().center());
        if (maximized)
            scrollArea->parentWidget()->parentWidget()->setWindowState(scrollArea->parentWidget()->parentWidget()->windowState() | Qt::WindowMaximized);
        if (fullScreen)
            scrollArea->parentWidget()->parentWidget()->setWindowState(scrollArea->parentWidget()->parentWidget()->windowState() | Qt::WindowFullScreen);
        if (!maximized && !fullScreen)
            scrollArea->parentWidget()->parentWidget()->setWindowState(scrollArea->parentWidget()->parentWidget()->windowState() | Qt::WindowMaximized);
        scrollArea->parentWidget()->parentWidget()->raise();
    }
}

void QLabyrinth::recommencer()
{
    if (indexTimerRotation)
    {
        killTimer(indexTimerRotation);
        indexTimerRotation = 0;
    }
    if (indexTimerDistorsionVerticale)
    {
        killTimer(indexTimerDistorsionVerticale);
        indexTimerDistorsionVerticale = 0;
    }
    if (indexTimerDistorsionHorizontale)
    {
        killTimer(indexTimerDistorsionHorizontale);
        indexTimerDistorsionHorizontale = 0;
    }
    if (indexTimerCisaillementVertical)
    {
        killTimer(indexTimerCisaillementVertical);
        indexTimerCisaillementVertical = 0;
    }
    if (indexTimerCisaillementHorizontal)
    {
        killTimer(indexTimerCisaillementHorizontal);
        indexTimerCisaillementHorizontal = 0;
    }

    sensRotation = modeLabyrinthe.sensRotation;
    angle = 0;
    numeroQuart = 1;
    distorsionSynchronisee = modeLabyrinthe.distorsionSynchronisee;
    pourcentageDistorsionVerticaleCourant = 0;
    pourcentageDistorsionHorizontaleCourant = 0;
    cisaillementSynchronise = modeLabyrinthe.cisaillementSynchronise;
    pourcentageCisaillementVerticalCourant = 0;
    pourcentageCisaillementHorizontalCourant = 0;
    if ((modeLabyrinthe.mode & Rotation))
    {
        if (rand() % 2 && modeLabyrinthe.sensRotationAuHasard)
            sensRotation *= -1;
        if (modeLabyrinthe.rotationFixe)
        {
            if (modeLabyrinthe.angleRotationAuHasard)
                angle += sensRotation * rand() % (359 + 1);
            else
                angle += sensRotation * modeLabyrinthe.angleRotation;
            if (angle > 359)
                angle -= 360;
            else if (angle < 0)
                angle += 359;
            if (angle < 45 || angle >= 315)
                numeroQuart = 1;
            else if (angle >= 45 && angle < 135)
                numeroQuart = 2;
            else if (angle >= 135 && angle < 225)
                numeroQuart = 3;
            else
                numeroQuart = 4;
        }
    }
    if (modeLabyrinthe.mode & Distorsion)
    {
        if (modeLabyrinthe.distorsionSynchroniseeAuHasard)
            distorsionSynchronisee = bool(rand() % 2);
        if (modeLabyrinthe.pourcentageDistorsionVerticaleAuHasard)
            pourcentageDistorsionVerticale = rand() % 101;
        if (modeLabyrinthe.pourcentageDistorsionHorizontaleAuHasard)
            pourcentageDistorsionHorizontale = rand() % 101;
    }
    if (modeLabyrinthe.mode & Cisaillement)
    {
        if (modeLabyrinthe.cisaillementSynchroniseAuHasard)
            cisaillementSynchronise = bool(rand() % 2);
        if (modeLabyrinthe.pourcentageCisaillementVerticalAuHasard)
            pourcentageCisaillementVertical = rand() % 101;
        if (modeLabyrinthe.pourcentageCisaillementHorizontalAuHasard)
            pourcentageCisaillementHorizontal = rand() % 101;
    }

    setFixedSize(getLongueur() * waysSize_.width() + (getLongueur() + 1) * wallsSize_.width(),
                 getLargeur() * waysSize_.height() + (getLargeur() + 1) * wallsSize_.height());
    calculerTransformation();

    if (size().width() < QGuiApplication::screenAt(pos())->size().width() - 50
        && size().height() < QGuiApplication::screenAt(pos())->size().height() - 50)
    {
        scrollArea->setMinimumSize(size().width() + scrollArea->frameWidth() * 2,
                                   size().height() + scrollArea->frameWidth() * 2);
        QTimer::singleShot(50, this, SLOT(redimensionnerFenetrePrincipale()));
    }
    else
    {
        scrollArea->setMinimumSize(LONGUEURFACILE * WAYSSIZE.width() + (LONGUEURFACILE + 1) * WALLSSIZE.width() + scrollArea->frameWidth() * 2,
                                   LARGEURFACILE * WAYSSIZE.height() + (LARGEURFACILE + 1) * WALLSSIZE.height() + scrollArea->frameWidth() * 2);
        QTimer::singleShot(50, this, SLOT(maximiserFenetre()));
    }

    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }
/*
    for (int i = 0; i < getLargeur(); i++)
        for (int j = 0; j < getLongueur(); j++)
            if (labyrinthe[i][j] > 1)
                labyrinthe[i][j] = 0;
*/
    enResolution = false;
    partieEnCours = false;
    partieEnPause = false;
    nombreDeplacement = 0;

    //labyrinthe[emplacementYJoueur][emplacementXJoueur] = 2;

    QPalette p = scrollArea->palette();

    if (modeLabyrinthe.mode & Obscurite)
        p.setColor(QPalette::Window, modeLabyrinthe.couleurObscurite);
    else
        p.setColor(QPalette::Window, textures[0].couleur);

    scrollArea->setPalette(p);

    QRect rect = transform.mapRect(QRect((getEmplacementXJoueur() / 2) * waysSize_.width() + (getEmplacementXJoueur() / 2 + 1) * wallsSize_.width() - waysSize_.width() - wallsSize_.width(),
                                         (getEmplacementYJoueur() / 2) * waysSize_.height() + (getEmplacementYJoueur() / 2 + 1) * wallsSize_.height() - waysSize_.height() - wallsSize_.width(),
                                         waysSize_.width() + (waysSize_.width() + wallsSize_.width()) * 2,
                                         waysSize_.height() + (waysSize_.height() + wallsSize_.height()) * 2));
    scrollArea->ensureVisible(rect.x(), rect.y(), rect.width(), rect.height());
}

bool QLabyrinth::getPartieTerminee() const
{
    return (enResolution || (getEmplacementXJoueur() == getXSortie() && getEmplacementYJoueur() == getYSortie()));
}

void QLabyrinth::mettreEnPauseRedemarrer()
{
    if (getEmplacementXJoueur() == getXSortie() && getEmplacementYJoueur() == getYSortie())
        return;

    partieEnPause = !partieEnPause;

    if (glLabyrinth)
        glLabyrinth->setVisible(!partieEnPause);

    if (partieEnCours)
    {
        if ((modeLabyrinthe.mode & Rotation) && !modeLabyrinthe.rotationFixe)
        {
            if (partieEnPause)
            {
                int e = tR.elapsed();
                tempsRestantRotation = tempsRestantRotation-e;
                tR = QElapsedTimer();
                killTimer(indexTimerRotation);
                indexTimerRotation = 0;
            }
            else
            {
                if (tempsRestantRotation > 0)
                {
                    tR.start();
                    indexTimerRotation = startTimer(tempsRestantRotation);
                }
                else if (modeLabyrinthe.intervalleRotationAuHasard)
                {
                    intervalleRotation = rand() % (INTERVALLEROTATIONMAX - INTERVALLEROTATIONMIN + 1) + INTERVALLEROTATIONMIN;
                    tempsRestantRotation = intervalleRotation;
                    indexTimerRotation = startTimer(intervalleRotation);
                }
                else
                    indexTimerRotation = startTimer(modeLabyrinthe.intervalleRotation);
            }
        }
        if (modeLabyrinthe.mode & Distorsion)
        {
            if (partieEnPause)
            {
                if (!modeLabyrinthe.distorsionVerticaleFixe)
                {
                    int eDV = tDV.elapsed();
                    tempsRestantDistorsionVerticale = tempsRestantDistorsionVerticale - eDV;
                    tDV = QElapsedTimer();
                    killTimer(indexTimerDistorsionVerticale);
                    indexTimerDistorsionVerticale = 0;
                }
                if (!distorsionSynchronisee && !modeLabyrinthe.distorsionHorizontaleFixe)
                {
                    int eDH = tDH.elapsed();
                    tempsRestantDistorsionHorizontale = tempsRestantDistorsionHorizontale - eDH;
                    tDH = QElapsedTimer();
                    killTimer(indexTimerDistorsionHorizontale);
                    indexTimerDistorsionHorizontale = 0;
                }
            }
            else
            {
                if (!modeLabyrinthe.distorsionVerticaleFixe)
                {
                    if (tempsRestantDistorsionVerticale > 0)
                    {
                        tDV.start();
                        indexTimerDistorsionVerticale = startTimer(tempsRestantDistorsionVerticale);
                    }
                    else if (modeLabyrinthe.intervalleDistorsionVerticaleAuHasard)
                    {
                        intervalleDistorsionVerticale = rand() % (INTERVALLEDISTORSIONVERTICALEMAX - INTERVALLEDISTORSIONVERTICALEMIN + 1) + INTERVALLEDISTORSIONVERTICALEMIN;
                        tempsRestantDistorsionVerticale = intervalleDistorsionVerticale;
                        indexTimerDistorsionVerticale = startTimer(intervalleDistorsionVerticale);
                    }
                    else
                        indexTimerDistorsionVerticale = startTimer(modeLabyrinthe.intervalleDistorsionVerticale);
                }
                if (!distorsionSynchronisee && !modeLabyrinthe.distorsionHorizontaleFixe)
                {
                    if (tempsRestantDistorsionHorizontale > 0)
                    {
                        tDH.start();
                        indexTimerDistorsionHorizontale = startTimer(tempsRestantDistorsionHorizontale);
                    }
                    else if (modeLabyrinthe.intervalleDistorsionHorizontaleAuHasard)
                    {
                        intervalleDistorsionHorizontale = rand() % (INTERVALLEDISTORSIONHORIZONTALEMAX - INTERVALLEDISTORSIONHORIZONTALEMIN + 1) + INTERVALLEDISTORSIONHORIZONTALEMIN;
                        tempsRestantDistorsionHorizontale = intervalleDistorsionHorizontale;
                        indexTimerDistorsionHorizontale = startTimer(intervalleDistorsionHorizontale);
                    }
                    else
                        indexTimerDistorsionHorizontale = startTimer(modeLabyrinthe.intervalleDistorsionHorizontale);
                }
            }
        }
        if (modeLabyrinthe.mode & Cisaillement)
        {
            if (partieEnPause)
            {
                if (!modeLabyrinthe.cisaillementVerticalFixe)
                {
                    int eDV = tDV.elapsed();
                    tempsRestantCisaillementVertical = tempsRestantCisaillementVertical - eDV;
                    tDV = QElapsedTimer();
                    killTimer(indexTimerCisaillementVertical);
                    indexTimerCisaillementVertical = 0;
                }
                if (!cisaillementSynchronise && !modeLabyrinthe.cisaillementHorizontalFixe)
                {
                    int eDH = tDH.elapsed();
                    tempsRestantCisaillementHorizontal = tempsRestantCisaillementHorizontal - eDH;
                    tDH = QElapsedTimer();
                    killTimer(indexTimerCisaillementHorizontal);
                    indexTimerCisaillementHorizontal = 0;
                }
            }
            else
            {
                if (modeLabyrinthe.cisaillementVerticalFixe)
                {
                    if (tempsRestantCisaillementVertical > 0)
                    {
                        tDV.start();
                        indexTimerCisaillementVertical = startTimer(tempsRestantCisaillementVertical);
                    }
                    else if (modeLabyrinthe.intervalleCisaillementVerticalAuHasard)
                    {
                        intervalleCisaillementVertical = rand() % (INTERVALLECISAILLEMENTVERTICALMAX - INTERVALLECISAILLEMENTVERTICALMIN + 1) + INTERVALLECISAILLEMENTVERTICALMIN;
                        tempsRestantCisaillementVertical = intervalleCisaillementVertical;
                        indexTimerCisaillementVertical = startTimer(intervalleCisaillementVertical);
                    }
                    else
                        indexTimerCisaillementVertical = startTimer(modeLabyrinthe.intervalleCisaillementVertical);
                }
                if (!cisaillementSynchronise && !modeLabyrinthe.cisaillementHorizontalFixe)
                {
                    if (tempsRestantCisaillementHorizontal > 0)
                    {
                        tDH.start();
                        indexTimerCisaillementHorizontal = startTimer(tempsRestantCisaillementHorizontal);
                    }
                    else if (modeLabyrinthe.intervalleCisaillementHorizontalAuHasard)
                    {
                        intervalleCisaillementHorizontal = rand() % (INTERVALLECISAILLEMENTHORIZONTALMAX - INTERVALLECISAILLEMENTHORIZONTALMIN + 1) + INTERVALLECISAILLEMENTHORIZONTALMIN;
                        tempsRestantCisaillementHorizontal = intervalleCisaillementHorizontal;
                        indexTimerCisaillementHorizontal = startTimer(intervalleCisaillementHorizontal);
                    }
                    else
                        indexTimerCisaillementHorizontal = startTimer(modeLabyrinthe.intervalleCisaillementHorizontal);
                }
            }
        }
    }

    if (partieEnPause)
    {
        QPalette p = scrollArea->palette();
        p.setColor(QPalette::Window, textures[0].couleur);
        scrollArea->setPalette(p);
    }
    else if (modeLabyrinthe.mode & Obscurite)
    {
        QPalette p = scrollArea->palette();
        p.setColor(QPalette::Window, modeLabyrinthe.couleurObscurite);
        scrollArea->setPalette(p);
    }

    rafraichir();
    activer();
}

void QLabyrinth::resoudre()
{
    enResolution = true;
    arretResolution = false;

    if (resolutionProgressive)
    {
        if (!partieEnCours)
        {
            partieEnCours = true;
            if ((modeLabyrinthe.mode & Rotation) && !modeLabyrinthe.rotationFixe)
            {
                if (modeLabyrinthe.intervalleRotationAuHasard)
                {
                    intervalleRotation = rand() % (INTERVALLEROTATIONMAX - INTERVALLEROTATIONMIN + 1) + INTERVALLEROTATIONMIN;
                    indexTimerRotation = startTimer(intervalleRotation);
                }
                else
                    indexTimerRotation = startTimer(modeLabyrinthe.intervalleRotation);
            }
            if (modeLabyrinthe.mode & Distorsion)
            {
                if (!modeLabyrinthe.distorsionVerticaleFixe)
                {
                    if (modeLabyrinthe.intervalleDistorsionVerticaleAuHasard)
                    {
                        intervalleDistorsionVerticale = rand() % (INTERVALLEDISTORSIONVERTICALEMAX - INTERVALLEDISTORSIONVERTICALEMIN + 1) + INTERVALLEDISTORSIONVERTICALEMIN;
                        indexTimerDistorsionVerticale = startTimer(intervalleDistorsionVerticale);
                    }
                    else
                        indexTimerDistorsionVerticale = startTimer(modeLabyrinthe.intervalleDistorsionVerticale);
                }
                if (!distorsionSynchronisee && !modeLabyrinthe.distorsionHorizontaleFixe)
                {
                    if (modeLabyrinthe.intervalleDistorsionHorizontaleAuHasard)
                    {
                        intervalleDistorsionHorizontale = rand() % (INTERVALLEDISTORSIONHORIZONTALEMAX - INTERVALLEDISTORSIONHORIZONTALEMIN + 1) + INTERVALLEDISTORSIONHORIZONTALEMIN;
                        indexTimerDistorsionHorizontale = startTimer(intervalleDistorsionHorizontale);
                    }
                    else
                        indexTimerDistorsionHorizontale = startTimer(modeLabyrinthe.intervalleDistorsionHorizontale);
                }
            }
            if (modeLabyrinthe.mode & Cisaillement)
            {
                if (!modeLabyrinthe.cisaillementVerticalFixe)
                {
                    if (modeLabyrinthe.intervalleCisaillementVerticalAuHasard)
                    {
                        intervalleCisaillementVertical = rand() % (INTERVALLECISAILLEMENTVERTICALMAX - INTERVALLECISAILLEMENTVERTICALMIN + 1) + INTERVALLECISAILLEMENTVERTICALMIN;
                        indexTimerCisaillementVertical = startTimer(intervalleCisaillementVertical);
                    }
                    else
                        indexTimerCisaillementVertical = startTimer(modeLabyrinthe.intervalleCisaillementVertical);
                }
                if (!cisaillementSynchronise && !modeLabyrinthe.cisaillementHorizontalFixe)
                {
                    if (modeLabyrinthe.intervalleCisaillementHorizontalAuHasard)
                    {
                        intervalleCisaillementHorizontal = rand() % (INTERVALLECISAILLEMENTHORIZONTALMAX - INTERVALLECISAILLEMENTHORIZONTALMIN + 1) + INTERVALLECISAILLEMENTHORIZONTALMIN;
                        indexTimerCisaillementHorizontal = startTimer(intervalleCisaillementHorizontal);
                    }
                    else
                        indexTimerCisaillementHorizontal = startTimer(modeLabyrinthe.intervalleCisaillementHorizontal);
                }
            }
        }
        solveLabyrinth();
        return;
    }

    if (indexTimerRotation)
    {
        killTimer(indexTimerRotation);
        indexTimerRotation = 0;
    }
    if (indexTimerDistorsionVerticale)
    {
        killTimer(indexTimerDistorsionVerticale);
        indexTimerDistorsionVerticale = 0;
    }
    if (indexTimerDistorsionHorizontale)
    {
        killTimer(indexTimerDistorsionHorizontale);
        indexTimerDistorsionHorizontale = 0;
    }
    if (indexTimerCisaillementVertical)
    {
        killTimer(indexTimerCisaillementVertical);
        indexTimerCisaillementVertical = 0;
    }
    if (indexTimerCisaillementHorizontal)
    {
        killTimer(indexTimerCisaillementHorizontal);
        indexTimerCisaillementHorizontal = 0;
    }

    while (!(labyrinth->player(playerId).state() & Labyrinth2d::Player::Finished) && enResolution)
        resoudre(-1);

    QPalette p = scrollArea->palette();
    p.setColor(QPalette::Window, textures[0].couleur);
    scrollArea->setPalette(p);

    QRect rect = transform.mapRect(QRect((getEmplacementXJoueur() / 2) * waysSize_.width() + (getEmplacementXJoueur() / 2 + 1) * wallsSize_.width() - waysSize_.width() - wallsSize_.width(),
                                         (getEmplacementYJoueur() / 2) * waysSize_.height() + (getEmplacementYJoueur() / 2 + 1) * wallsSize_.height() - waysSize_.height() - wallsSize_.width(),
                                         waysSize_.width() + (waysSize_.width() + wallsSize_.width()) * 2,
                                         waysSize_.height() + (waysSize_.height() + wallsSize_.height()) * 2));
    scrollArea->ensureVisible(rect.x(), rect.y(), rect.width(), rect.height());

    rafraichir();
}

void QLabyrinth::solveLabyrinth()
{
    if (!arretResolution && !(labyrinth->player(playerId).state() & Labyrinth2d::Player::Finished) && enResolution)
    {
        if (!resolutionProgressive)
        {
            resoudre(typeResolution_);
            return;
        }

        if (glLabyrinth)
        {
            glLabyrinth->solveLabyrinth();
            return;
        }

        resoudre(1);

        if (!(labyrinth->player(playerId).state() & Labyrinth2d::Player::Finished) && enResolution)
            QTimer::singleShot(50, this, SLOT(solveLabyrinth()));
        else
        {
            QPalette p = scrollArea->palette();
            p.setColor(QPalette::Window, textures[0].couleur);
            scrollArea->setPalette(p);
        }

        QRect rect = transform.mapRect(QRect((getEmplacementXJoueur() / 2) * waysSize_.width() + (getEmplacementXJoueur() / 2 + 1) * wallsSize_.width() - waysSize_.width() - wallsSize_.width(),
                                             (getEmplacementYJoueur() / 2) * waysSize_.height() + (getEmplacementYJoueur() / 2 + 1) * wallsSize_.height() - waysSize_.height() - wallsSize_.width(),
                                             waysSize_.width() + (waysSize_.width() + wallsSize_.width()) * 2,
                                             waysSize_.height() + (waysSize_.height() + wallsSize_.height()) * 2));
        scrollArea->ensureVisible(rect.x(), rect.y(), rect.width(), rect.height());
        rafraichir();
    }
}

bool QLabyrinth::getResolutionProgressive() const
{
    return resolutionProgressive;
}

void QLabyrinth::setResolutionProgressive(bool b)
{
    if (resolutionProgressive == b)
        return;

    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }

    resolutionProgressive = b;
}

bool QLabyrinth::getEffacerChemin() const
{
    return !labyrinth->player(playerId).keptFullTrace();
}

void QLabyrinth::setEffacerChemin(bool b)
{
    if (partieEnCours || getEffacerChemin() == b)
        return;

    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }

    labyrinth->player(playerId).keepFullTrace(!b);

    rafraichir();
}

bool QLabyrinth::getAfficherTrace() const
{
    return afficherTrace;
}

void QLabyrinth::setAfficherTrace(bool b)
{
    if (afficherTrace == b)
        return;

    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }

    afficherTrace = b;

    rafraichir();
}

bool QLabyrinth::getEnregistre() const
{
    return enregistre;
}

void QLabyrinth::enregistrer(QDataStream &data, bool chrono, int ms, bool pauseImposee, const QString &musique, bool muet, bool labySeulement, bool adaptationTailleEcran, bool adaptationTaillePapier, bool enregistrerLabyrinthe)
{
    data << enregistrerLabyrinthe;
    if (enregistrerLabyrinthe)
        data << labyrinthe;
    data << getXEntree();
    data << getYEntree();
    data << getXSortie();
    data << getYSortie();
    data << getLargeur();
    data << getLongueur();
    data << getEmplacementXJoueur();
    data << getEmplacementYJoueur();
    data << enResolution;
    data << resolutionProgressive;
    data << getEffacerChemin();
    data << afficherTrace;
    data << niveau;
    data << typeLabyrinthe;
    for (int i = 0; i < 3; i++)
    {
        data << textures[i].typeTexture;
        data << textures[i].couleur;
        data << textures[i].motif;
        data << textures[i].image;
    }
    data << partieEnCours;
    if (pauseImposee)
        data << partieEnPause;
    else
        data << false;
    data << enConstruction;
    //data << tailleCase;
    data << waysSize_;
    data << wallsSize_;
    data << chrono;
    data << ms;
    data << musique;
    data << muet;
    data << labySeulement;
    data << qobject_cast<QMainWindow *>(scrollArea->parentWidget()->parentWidget())->saveState();
    data << scrollArea->parentWidget()->parentWidget()->size();
    data << scrollArea->parentWidget()->parentWidget()->isFullScreen();
    data << scrollArea->parentWidget()->parentWidget()->isMaximized();
    data << scrollArea->horizontalScrollBar()->value();
    data << scrollArea->verticalScrollBar()->value();
    data << adaptationTailleEcran;
    data << adaptationTaillePapier;
    data << algorithme;
    data << angle;
    data << nombreDeplacement;
    data << modeLabyrinthe.mode;
    data << modeLabyrinthe.angleRotation;
    data << modeLabyrinthe.angleRotationAuHasard;
    data << modeLabyrinthe.rotationFixe;
    data << modeLabyrinthe.intervalleRotation;
    data << modeLabyrinthe.intervalleRotationAuHasard;
    data << modeLabyrinthe.sensRotation;
    data << modeLabyrinthe.sensRotationAuHasard;
    data << modeLabyrinthe.couleurObscurite;
    data << modeLabyrinthe.rayonObscurite;
    data << modeLabyrinthe.pourcentageDistorsionVerticale;
    data << modeLabyrinthe.pourcentageDistorsionHorizontale;
    data << modeLabyrinthe.pourcentageDistorsionVerticaleAuHasard;
    data << modeLabyrinthe.pourcentageDistorsionHorizontaleAuHasard;
    data << modeLabyrinthe.intervalleDistorsionVerticale;
    data << modeLabyrinthe.intervalleDistorsionHorizontale;
    data << modeLabyrinthe.intervalleDistorsionVerticaleAuHasard;
    data << modeLabyrinthe.intervalleDistorsionHorizontaleAuHasard;
    data << modeLabyrinthe.distorsionVerticaleFixe;
    data << modeLabyrinthe.distorsionHorizontaleFixe;
    data << modeLabyrinthe.distorsionSynchronisee;
    data << modeLabyrinthe.distorsionSynchroniseeAuHasard;
    data << modeLabyrinthe.pourcentageCisaillementVertical;
    data << modeLabyrinthe.pourcentageCisaillementHorizontal;
    data << modeLabyrinthe.pourcentageCisaillementVerticalAuHasard;
    data << modeLabyrinthe.pourcentageCisaillementHorizontalAuHasard;
    data << modeLabyrinthe.intervalleCisaillementVertical;
    data << modeLabyrinthe.intervalleCisaillementHorizontal;
    data << modeLabyrinthe.intervalleCisaillementVerticalAuHasard;
    data << modeLabyrinthe.intervalleCisaillementHorizontalAuHasard;
    data << modeLabyrinthe.cisaillementVerticalFixe;
    data << modeLabyrinthe.cisaillementHorizontalFixe;
    data << modeLabyrinthe.cisaillementSynchronise;
    data << modeLabyrinthe.cisaillementSynchroniseAuHasard;
    data << typeResolution_;

    enregistre = true;
    emit enregistrementChange();
}

void QLabyrinth::charger(QDataStream &data, bool &chrono, int &ms, QString &musique, bool &muet, bool &labySeulement, bool &adaptationTailleEcran, bool &adaptationTaillePapier)
{
    enregistre = true;
    emit enregistrementChange();

    int xEntree, yEntree, xSortie, ySortie, lon, lar, emplacementXJoueur, emplacementYJoueur;
    bool b, effacerChemin;
    unsigned int ancienMode = modeLabyrinthe.mode;

    data >> b;
    if (b)
        data >> labyrinthe;
    data >> xEntree;
    data >> yEntree;
    data >> xSortie;
    data >> ySortie;
    data >> lar;
    data >> lon;
    data >> emplacementXJoueur;
    data >> emplacementYJoueur;
    data >> enResolution;
    data >> resolutionProgressive;
    data >> effacerChemin;
    data >> afficherTrace;
    int entier;
    data >> entier;
    niveau = Niveau(entier);
    data >> entier;
    typeLabyrinthe = TypeLabyrinthe(entier);
    for (int i = 0; i < 3; i++)
    {
        data >> entier;
        textures[i].typeTexture = TypeTexture(entier);
        data >> textures[i].couleur;
        data >> textures[i].motif;
        data >> textures[i].image;
    }
    data >> partieEnCours;
    data >> partieEnPause;
    data >> enConstruction;
    QSize s;
    data >> s;
    //setTailleCase(s);
    setWaysSize(s);
    data >> s;
    setWallsSize(s);
    data >> chrono;
    data >> ms;
    data >> musique;
    data >> muet;
    data >> labySeulement;
    data >> state;
    data >> s;
    data >> full;
    data >> max;
    data >> scrollBarHValue;
    data >> scrollBarVValue;
    data >> adaptationTailleEcran;
    data >> adaptationTaillePapier;
    data >> entier;
    algorithme = Algorithme(entier);
    data >> angle;
    data >> nombreDeplacement;
    data >> modeLabyrinthe.mode;
    data >> modeLabyrinthe.angleRotation;
    data >> modeLabyrinthe.angleRotationAuHasard;
    data >> modeLabyrinthe.rotationFixe;
    data >> modeLabyrinthe.intervalleRotation;
    data >> modeLabyrinthe.intervalleRotationAuHasard;
    data >> modeLabyrinthe.sensRotation;
    data >> modeLabyrinthe.sensRotationAuHasard;
    data >> modeLabyrinthe.couleurObscurite;
    data >> modeLabyrinthe.rayonObscurite;
    data >> modeLabyrinthe.pourcentageDistorsionVerticale;
    data >> modeLabyrinthe.pourcentageDistorsionHorizontale;
    data >> modeLabyrinthe.pourcentageDistorsionVerticaleAuHasard;
    data >> modeLabyrinthe.pourcentageDistorsionHorizontaleAuHasard;
    data >> modeLabyrinthe.intervalleDistorsionVerticale;
    data >> modeLabyrinthe.intervalleDistorsionHorizontale;
    data >> modeLabyrinthe.intervalleDistorsionVerticaleAuHasard;
    data >> modeLabyrinthe.intervalleDistorsionHorizontaleAuHasard;
    data >> modeLabyrinthe.distorsionVerticaleFixe;
    data >> modeLabyrinthe.distorsionHorizontaleFixe;
    data >> modeLabyrinthe.distorsionSynchronisee;
    data >> modeLabyrinthe.distorsionSynchroniseeAuHasard;
    data >> modeLabyrinthe.pourcentageCisaillementVertical;
    data >> modeLabyrinthe.pourcentageCisaillementHorizontal;
    data >> modeLabyrinthe.pourcentageCisaillementVerticalAuHasard;
    data >> modeLabyrinthe.pourcentageCisaillementHorizontalAuHasard;
    data >> modeLabyrinthe.intervalleCisaillementVertical;
    data >> modeLabyrinthe.intervalleCisaillementHorizontal;
    data >> modeLabyrinthe.intervalleCisaillementVerticalAuHasard;
    data >> modeLabyrinthe.intervalleCisaillementHorizontalAuHasard;
    data >> modeLabyrinthe.cisaillementVerticalFixe;
    data >> modeLabyrinthe.cisaillementHorizontalFixe;
    data >> modeLabyrinthe.cisaillementSynchronise;
    data >> modeLabyrinthe.cisaillementSynchroniseAuHasard;
    data >> typeResolution_;

    b = (getLongueur() != lon || getLargeur() != lar);

    if (glLabyrinth)
        glLabyrinth->hide();

    rafraichir();

    if (typeLabyrinthe == Labyrinthe2Den3D)
    {
        if (!glLabyrinth)
        {
            glLabyrinth = new GLLabyrinth(this);
            glLabyrinth->rechargerTextures();
            QHBoxLayout *hBoxLayout = new QHBoxLayout;
            hBoxLayout->setContentsMargins(0, 0, 0, 0);
            hBoxLayout->addWidget(glLabyrinth);
            setLayout(hBoxLayout);

            connect(glLabyrinth, SIGNAL(deplacementJoueur(int, int)), this, SLOT(joueurDeplace(int, int)));

            setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

            scrollArea->setMinimumSize(640, 480);
            scrollArea->setWidgetResizable(true);
            QTimer::singleShot(100, this, SLOT(redimensionnerFenetrePrincipale()));
        }

        glLabyrinth->setLabyrinth(labyrinth);
        glLabyrinth->show();
    }
    else
    {
        if (glLabyrinth)
        {
            glLabyrinth->deleteLater();
            glLabyrinth = 0;
            layout()->deleteLater();
            scrollArea->setWidgetResizable(false);
            setFixedSize(transform.mapRect(QRectF(0, 0,
                                                  getLongueur() * waysSize_.width() + (getLongueur() + 1) * wallsSize_.width(),
                                                  getLargeur() * waysSize_.height() + (getLargeur() + 1) * wallsSize_.height()).toRect()).size());
            if (size().width() < QGuiApplication::screenAt(pos())->size().width() - 50
                && size().height() < QGuiApplication::screenAt(pos())->size().height() - 50)
            {
                scrollArea->setMinimumSize(size().width() + scrollArea->frameWidth() * 2,
                                           size().height() + scrollArea->frameWidth() * 2);
                QTimer::singleShot(50, this, SLOT(redimensionnerFenetrePrincipale()));
            }
            else
            {
                scrollArea->setMinimumSize(LONGUEURFACILE * WAYSSIZE.width() + (LONGUEURFACILE + 1) * WALLSSIZE.width() + scrollArea->frameWidth() * 2,
                                           LARGEURFACILE * WAYSSIZE.height() + (LARGEURFACILE + 1) * WALLSSIZE.height() + scrollArea->frameWidth() * 2);
                QTimer::singleShot(50, this, SLOT(maximiserFenetre()));
            }
        }

        setFixedSize(getLongueur() * waysSize_.width() + (getLongueur() + 1) * wallsSize_.width(),
                     getLargeur() * waysSize_.height() + (getLargeur() + 1) * wallsSize_.height());
        calculerTransformation();

        if (b || (modeLabyrinthe.mode & Rotation) || ((ancienMode & Rotation) && !(modeLabyrinthe.mode & Rotation)))
        {
            setFixedSize(transform.mapRect(QRectF(0, 0,
                                                  getLongueur() * waysSize_.width() + (getLongueur() + 1) * wallsSize_.width(),
                                                  getLargeur() * waysSize_.height() + (getLargeur() + 1) * wallsSize_.height()).toRect()).size());
            if (size().width() < QGuiApplication::screenAt(pos())->size().width() - 50
                && size().height() < QGuiApplication::screenAt(pos())->size().height() - 50)
            {
                scrollArea->setMinimumSize(size().width() + scrollArea->frameWidth() * 2,
                                           size().height() + scrollArea->frameWidth() * 2);
                QTimer::singleShot(50, this, SLOT(redimensionnerFenetrePrincipale()));
            }
            else
            {
                scrollArea->setMinimumSize(LONGUEURFACILE * WAYSSIZE.width() + (LONGUEURFACILE + 1) * WALLSSIZE.width() + scrollArea->frameWidth() * 2,
                                           LARGEURFACILE * WAYSSIZE.height() + (LARGEURFACILE + 1) * WALLSSIZE.height() + scrollArea->frameWidth() * 2);
                QTimer::singleShot(50, this, SLOT(maximiserFenetre()));
            }
        }

        QRect rect = transform.mapRect(QRect((getEmplacementXJoueur() / 2) * waysSize_.width() + (getEmplacementXJoueur() / 2 + 1) * wallsSize_.width() - waysSize_.width() - wallsSize_.width(),
                                             (getEmplacementYJoueur() / 2) * waysSize_.height() + (getEmplacementYJoueur() / 2 + 1) * wallsSize_.height() - waysSize_.height() - wallsSize_.width(),
                                             waysSize_.width() + (waysSize_.width() + wallsSize_.width()) * 2,
                                             waysSize_.height() + (waysSize_.height() + wallsSize_.height()) * 2));
        scrollArea->ensureVisible(rect.x(), rect.y(), rect.width(), rect.height());
    }

    rafraichir();
    activer();

    QTimer::singleShot(100, this, SLOT(actualiserChargement()));
}

void QLabyrinth::actualiserChargement()
{
    if (full)
        scrollArea->parentWidget()->parentWidget()->setWindowState(scrollArea->parentWidget()->parentWidget()->windowState() | Qt::WindowFullScreen);
    else if (isFullScreen())
        scrollArea->parentWidget()->parentWidget()->setWindowState(scrollArea->parentWidget()->parentWidget()->windowState() ^ Qt::WindowFullScreen);
    if (max)
        scrollArea->parentWidget()->parentWidget()->setWindowState(scrollArea->parentWidget()->parentWidget()->windowState() | Qt::WindowMaximized);
    else if (isMaximized())
        scrollArea->parentWidget()->parentWidget()->setWindowState(scrollArea->parentWidget()->parentWidget()->windowState() ^ Qt::WindowMaximized);

    rafraichir();

    QRect rect = transform.mapRect(QRect((getEmplacementXJoueur() / 2) * waysSize_.width() + (getEmplacementXJoueur() / 2 + 1) * wallsSize_.width() - waysSize_.width() - wallsSize_.width(),
                                         (getEmplacementYJoueur() / 2) * waysSize_.height() + (getEmplacementYJoueur() / 2 + 1) * wallsSize_.height() - waysSize_.height() - wallsSize_.width(),
                                         waysSize_.width() + (waysSize_.width() + wallsSize_.width()) * 2,
                                         waysSize_.height() + (waysSize_.height() + wallsSize_.height()) * 2));
    scrollArea->ensureVisible(rect.x(), rect.y(), rect.width(), rect.height());

    scrollArea->parentWidget()->parentWidget()->raise();
}

QLabyrinth::ModeLabyrinthe QLabyrinth::getModeLabyrinthe() const
{
    return modeLabyrinthe;
}

QLabyrinth::Algorithme QLabyrinth::getAlgorithme() const
{
    return algorithme;
}

void QLabyrinth::timerEvent(QTimerEvent *event)
{
    QWidget::timerEvent(event);

    if (typeLabyrinthe == Labyrinthe2Den3D)
        return;

    if (event->timerId() == indexTimerRotation)
    {
        if (modeLabyrinthe.intervalleRotationAuHasard)
        {
            killTimer(indexTimerRotation);
            indexTimerRotation = 0;
            tR = QElapsedTimer();
            tR.start();
            intervalleRotation = rand() % (INTERVALLEROTATIONMAX - INTERVALLEROTATIONMIN + 1) + INTERVALLEROTATIONMIN;
            tempsRestantRotation = intervalleRotation;
            indexTimerRotation = startTimer(intervalleRotation);
        }
        else
            tempsRestantRotation = modeLabyrinthe.intervalleRotation;
        if (modeLabyrinthe.sensRotationAuHasard)
        {
            if ((rand() % 101) > 95)
                sensRotation *= -1;
        }
        if (modeLabyrinthe.angleRotationAuHasard)
            angle += sensRotation * rand() % (359 + 1);
        else
            angle += sensRotation * modeLabyrinthe.angleRotation;
        if (angle > 359)
            angle -= 360;
        else if (angle < 0)
            angle += 359;
        qreal a = angle;
        if (a < 45 || a >= 315)
            numeroQuart = 1;
        else if (a >= 45 && a < 135)
            numeroQuart = 2;
        else if (a >= 135 && a < 225)
            numeroQuart = 3;
        else
            numeroQuart = 4;
    }
    if (event->timerId() == indexTimerDistorsionVerticale)
    {
        if (modeLabyrinthe.pourcentageDistorsionVerticaleAuHasard && rand() % 101 > 95)
            pourcentageDistorsionVerticale = rand() % 100 + 1;
        if (modeLabyrinthe.intervalleDistorsionVerticaleAuHasard)
        {
            killTimer(indexTimerDistorsionVerticale);
            indexTimerDistorsionVerticale = 0;
            tDV = QElapsedTimer();
            tDV.start();
            intervalleDistorsionVerticale = rand() % (INTERVALLEDISTORSIONVERTICALEMAX - INTERVALLEDISTORSIONVERTICALEMIN + 1) + INTERVALLEDISTORSIONVERTICALEMIN;
            tempsRestantDistorsionVerticale = intervalleDistorsionVerticale;
            indexTimerDistorsionVerticale = startTimer(intervalleDistorsionVerticale);
        }
        else
            tempsRestantDistorsionVerticale = modeLabyrinthe.intervalleDistorsionVerticale;
        if (pourcentageDistorsionVerticaleCourant >= pourcentageDistorsionVerticale)
            sensDistorsionVerticale = -1;
        else if (pourcentageDistorsionVerticaleCourant <= 0)
            sensDistorsionVerticale = 1;
        pourcentageDistorsionVerticaleCourant += sensDistorsionVerticale;
        if (distorsionSynchronisee)
        {
            if (modeLabyrinthe.pourcentageDistorsionHorizontaleAuHasard && rand() % 101 > 95)
                pourcentageDistorsionHorizontale = rand() % 100 + 1;
            if (pourcentageDistorsionHorizontaleCourant >= pourcentageDistorsionHorizontale)
                sensDistorsionHorizontale = -1;
            else if (pourcentageDistorsionHorizontaleCourant <= 0)
                sensDistorsionHorizontale = 1;
            pourcentageDistorsionHorizontaleCourant += sensDistorsionHorizontale;
        }
        if (modeLabyrinthe.distorsionSynchroniseeAuHasard)
        {
            if (rand() % 101 > 95)
                distorsionSynchronisee = !distorsionSynchronisee;
            if (!distorsionSynchronisee)
            {
                if (modeLabyrinthe.intervalleDistorsionHorizontaleAuHasard)
                {
                    killTimer(indexTimerDistorsionHorizontale);
                    indexTimerDistorsionHorizontale = 0;
                    tDV = QElapsedTimer();
                    tDV.start();
                    intervalleDistorsionHorizontale = rand() % (INTERVALLEDISTORSIONHORIZONTALEMAX - INTERVALLEDISTORSIONHORIZONTALEMIN + 1) + INTERVALLEDISTORSIONHORIZONTALEMIN;
                    tempsRestantDistorsionHorizontale = intervalleDistorsionHorizontale;
                    indexTimerDistorsionHorizontale = startTimer(intervalleDistorsionHorizontale);
                }
                else
                    tempsRestantDistorsionHorizontale = modeLabyrinthe.intervalleDistorsionHorizontale;
            }
            else if (indexTimerDistorsionHorizontale)
            {
                killTimer(indexTimerDistorsionHorizontale);
                indexTimerDistorsionHorizontale = 0;
            }
        }
    }
    else if (event->timerId() == indexTimerDistorsionHorizontale)
    {
        if (modeLabyrinthe.pourcentageDistorsionHorizontaleAuHasard && rand() % 101 > 95)
            pourcentageDistorsionHorizontale = rand() % 100 + 1;
        if (modeLabyrinthe.intervalleDistorsionHorizontaleAuHasard)
        {
            killTimer(indexTimerDistorsionHorizontale);
            indexTimerDistorsionHorizontale = 0;
            tDV = QElapsedTimer();
            tDV.start();
            intervalleDistorsionHorizontale = rand() % (INTERVALLEDISTORSIONHORIZONTALEMAX - INTERVALLEDISTORSIONHORIZONTALEMIN + 1) + INTERVALLEDISTORSIONHORIZONTALEMIN;
            tempsRestantDistorsionHorizontale = intervalleDistorsionHorizontale;
            indexTimerDistorsionHorizontale = startTimer(intervalleDistorsionHorizontale);
        }
        else
            tempsRestantDistorsionHorizontale = modeLabyrinthe.intervalleDistorsionHorizontale;
        if (pourcentageDistorsionHorizontaleCourant >= pourcentageDistorsionHorizontale)
            sensDistorsionHorizontale = -1;
        else if (pourcentageDistorsionHorizontaleCourant <= 0)
            sensDistorsionHorizontale = 1;
        pourcentageDistorsionHorizontaleCourant += sensDistorsionHorizontale;
        if (modeLabyrinthe.distorsionSynchroniseeAuHasard)
        {
            if (rand() % 101 > 95)
                distorsionSynchronisee = !distorsionSynchronisee;
            if (distorsionSynchronisee)
            {
                killTimer(indexTimerDistorsionHorizontale);
                indexTimerDistorsionHorizontale = 0;
            }
        }
    }
    if (event->timerId() == indexTimerCisaillementVertical)
    {
        if (modeLabyrinthe.pourcentageCisaillementVerticalAuHasard && rand() % 101 > 95)
            pourcentageCisaillementVertical = rand() % 100 + 1;
        if (modeLabyrinthe.intervalleCisaillementVerticalAuHasard)
        {
            killTimer(indexTimerCisaillementVertical);
            indexTimerCisaillementVertical = 0;
            tDV = QElapsedTimer();
            tDV.start();
            intervalleCisaillementVertical = rand() % (INTERVALLECISAILLEMENTVERTICALMAX - INTERVALLECISAILLEMENTVERTICALMIN + 1) + INTERVALLECISAILLEMENTVERTICALMIN;
            tempsRestantCisaillementVertical = intervalleCisaillementVertical;
            indexTimerCisaillementVertical = startTimer(intervalleCisaillementVertical);
        }
        else
            tempsRestantCisaillementVertical = modeLabyrinthe.intervalleCisaillementVertical;
        if (pourcentageCisaillementVerticalCourant >= pourcentageCisaillementVertical)
            sensCisaillementVertical = -1;
        else if (pourcentageCisaillementVerticalCourant <= -pourcentageCisaillementVertical)
            sensCisaillementVertical = 1;
        pourcentageCisaillementVerticalCourant += sensCisaillementVertical;
        if (cisaillementSynchronise)
        {
            if (modeLabyrinthe.pourcentageCisaillementHorizontalAuHasard && rand() % 101 > 95)
                pourcentageCisaillementHorizontal = rand() % 100 + 1;
            if (pourcentageCisaillementHorizontalCourant >= pourcentageCisaillementHorizontal)
                sensCisaillementHorizontal = -1;
            else if (pourcentageCisaillementHorizontalCourant <= -pourcentageCisaillementHorizontal)
                sensCisaillementHorizontal = 1;
            pourcentageCisaillementHorizontalCourant += sensCisaillementHorizontal;
        }
        if (modeLabyrinthe.cisaillementSynchroniseAuHasard)
        {
            if (rand() % 101 > 95)
                cisaillementSynchronise = !cisaillementSynchronise;
            if (!cisaillementSynchronise)
            {
                if (modeLabyrinthe.intervalleCisaillementHorizontalAuHasard)
                {
                    killTimer(indexTimerCisaillementHorizontal);
                    indexTimerCisaillementHorizontal = 0;
                    tDV = QElapsedTimer();
                    tDV.start();
                    intervalleCisaillementHorizontal = rand() % (INTERVALLECISAILLEMENTHORIZONTALMAX - INTERVALLECISAILLEMENTHORIZONTALMIN + 1) + INTERVALLECISAILLEMENTHORIZONTALMIN;
                    tempsRestantCisaillementHorizontal = intervalleCisaillementHorizontal;
                    indexTimerCisaillementHorizontal = startTimer(intervalleCisaillementHorizontal);
                }
                else
                    tempsRestantCisaillementHorizontal = modeLabyrinthe.intervalleCisaillementHorizontal;
            }
            else if (indexTimerCisaillementHorizontal)
            {
                killTimer(indexTimerCisaillementHorizontal);
                indexTimerCisaillementHorizontal = 0;
            }
        }
    }
    else if (event->timerId() == indexTimerCisaillementHorizontal)
    {
        if (modeLabyrinthe.pourcentageCisaillementHorizontalAuHasard && rand() % 101 > 95)
            pourcentageCisaillementHorizontal = rand() % 100 + 1;
        if (modeLabyrinthe.intervalleCisaillementHorizontalAuHasard)
        {
            killTimer(indexTimerCisaillementHorizontal);
            indexTimerCisaillementHorizontal = 0;
            tDV = QElapsedTimer();
            tDV.start();
            intervalleCisaillementHorizontal = rand() % (INTERVALLECISAILLEMENTHORIZONTALMAX - INTERVALLECISAILLEMENTHORIZONTALMIN + 1) + INTERVALLECISAILLEMENTHORIZONTALMIN;
            tempsRestantCisaillementHorizontal = intervalleCisaillementHorizontal;
            indexTimerCisaillementHorizontal = startTimer(intervalleCisaillementHorizontal);
        }
        else
            tempsRestantCisaillementHorizontal = modeLabyrinthe.intervalleCisaillementHorizontal;
        if (pourcentageCisaillementHorizontalCourant >= pourcentageCisaillementHorizontal)
            sensCisaillementHorizontal = -1;
        else if (pourcentageCisaillementHorizontalCourant <= -pourcentageCisaillementHorizontal)
            sensCisaillementHorizontal = 1;
        pourcentageCisaillementHorizontalCourant += sensCisaillementHorizontal;
        if (modeLabyrinthe.cisaillementSynchroniseAuHasard)
        {
            if (rand() % 101 > 95)
                cisaillementSynchronise = !cisaillementSynchronise;
            if (cisaillementSynchronise)
            {
                killTimer(indexTimerCisaillementHorizontal);
                indexTimerCisaillementHorizontal = 0;
            }
        }
    }

    calculerTransformation();
}

size_t QLabyrinth::getNombreDeplacement() const
{
    return nombreDeplacement;
}

void QLabyrinth::calculerTransformation()
{
    transform.reset();

    transform.translate(width() / 2, height() / 2);

    if ((modeLabyrinthe.mode & Rotation) && angle)
        transform.rotate(angle);

    if (modeLabyrinthe.mode & Distorsion)
        transform.scale(1 + double(pourcentageDistorsionHorizontaleCourant) / 100, 1 + double(pourcentageDistorsionVerticaleCourant) / 100);

    if (modeLabyrinthe.mode & Cisaillement)
        transform.shear(double(pourcentageCisaillementHorizontalCourant) / 100 * 0.9, double(pourcentageCisaillementVerticalCourant) / 100 * 0.9);//entre 0 et 1

    transform.translate(-(getLongueur() * waysSize_.width() + (getLongueur() + 1) * wallsSize_.width()) / 2,
                        -(getLargeur() * waysSize_.height() + (getLargeur() + 1) * wallsSize_.height()) / 2);

    setFixedSize(transform.mapRect(QRectF(0, 0,
                                          getLongueur() * waysSize_.width() + (getLongueur() + 1) * wallsSize_.width(),
                                          getLargeur() * waysSize_.height() + (getLargeur() + 1) * wallsSize_.height())).toRect().size());

    rafraichir();
}

void QLabyrinth::passerEnModeEcranDeVeille()
{
    modeEcranDeVeille = true;

    QFile f("Labyrinthe_scr.ini");

    if (f.open(QFile::ReadOnly))// | QFile::Truncate
    {
        QDataStream data(&f);
        data.setVersion(QDataStream::Qt_4_7);

        bool chrono, muet, labySeulement, adaptationTailleEcran, adaptationTaillePapier;

        int ms;
        QString mus;

        charger(data, chrono, ms, mus, muet, labySeulement, adaptationTailleEcran, adaptationTaillePapier);

        data >> modesAleatoires;
        data >> affichageAleatoire;
        data >> tailleAleatoire;
        data >> algorithmeAleatoire;
        data >> rapiditeResolution;
        data >> typeAleatoire;
        data >> formeAleatoire;

        f.close();

        qobject_cast<MainWindow *>(scrollArea->parentWidget()->parentWidget())->setAdaptationTailleEcran(adaptationTailleEcran);
        qobject_cast<MainWindow *>(scrollArea->parentWidget()->parentWidget())->setAdaptationTaillePapier(adaptationTaillePapier);
    }

    QPixmap p = QPixmap(32, 32);
    p.fill(QColor(0, 0, 0, 0));
    QCursor c = QCursor(p);
    c.setPos(QGuiApplication::screenAt(pos())->size().width() / 2, QGuiApplication::screenAt(pos())->size().height() / 2);

    setCursor(c);
    raise();
    activer();

    demarrerNouvellePartie();
}

bool QLabyrinth::enModeEcranDeVeille() const
{
    return modeEcranDeVeille;
}

void QLabyrinth::quitterModeEcranDeVeille()
{
    if (!modeEcranDeVeille)
        return;

    quitter = true;

    if (getEmplacementXJoueur() == getXSortie() && getEmplacementYJoueur() == getYSortie())
    {
        QFile f("Labyrinthe_scr.ini");

        if (f.open(QFile::WriteOnly))// | QFile::Truncate
        {
            QDataStream data(&f);
            data.setVersion(QDataStream::Qt_4_7);

            enregistrer(data, false, 0, false, QString(), true, false, qobject_cast<MainWindow *>(scrollArea->parentWidget()->parentWidget())->getAdaptationTailleEcran(), qobject_cast<MainWindow *>(scrollArea->parentWidget()->parentWidget())->getAdaptationTaillePapier(), false);

            data << modesAleatoires;
            data << affichageAleatoire;
            data << tailleAleatoire;
            data << algorithmeAleatoire;
            data << rapiditeResolution;
            data << typeAleatoire;
            data << formeAleatoire;

            f.close();
        }

        qApp->quit();
    }
}

void QLabyrinth::demarrerNouvellePartie()
{
    ModeLabyrinthe mode = modeLabyrinthe;
    int lon = getLongueur(), lar = getLargeur();
    Algorithme a = algorithme;
    TypeLabyrinthe type = typeLabyrinthe;
    FormeLabyrinthe forme = formeLabyrinthe;

    if (modesAleatoires)
    {
        mode.mode = Aucun;
        if (rand() % 2)
            mode.mode |= Rotation;
        if (rand() % 2)
            mode.mode |= Obscurite;
        if (rand() % 2)
            mode.mode |= Distorsion;
        if (rand() % 2)
            mode.mode |= Cisaillement;
    }
    if (affichageAleatoire)
        for (int i = 0; i < 3; i++)
            textures[0].typeTexture = TypeTexture(rand() % 3);
    if (tailleAleatoire)
    {
        int longueurLabyrinthe = QGuiApplication::screenAt(pos())->size().width() / tailleCase.width();
        if (!(longueurLabyrinthe % 2))
        {
            longueurLabyrinthe++;
            if (getLongueur() * tailleCase.width() > QGuiApplication::screenAt(pos())->size().width())
                longueurLabyrinthe -= 2;
        }
        int largeurLabyrinthe = QGuiApplication::screenAt(pos())->size().height() / tailleCase.height();
        if (!(largeurLabyrinthe%2))
        {
            largeurLabyrinthe++;
            if (largeurLabyrinthe * tailleCase.height() > QGuiApplication::screenAt(pos())->size().height())
                largeurLabyrinthe -= 2;
        }

        lon = rand() % (longueurLabyrinthe - LONGUEURFACILE + 1) + LONGUEURFACILE;
        lar = rand() % (largeurLabyrinthe - LARGEURFACILE + 1) + LARGEURFACILE;
    }
    if (algorithmeAleatoire)
        a = Algorithme(rand() % 9);
    if (typeAleatoire)
        type = TypeLabyrinthe(rand() % 2);
    if (formeAleatoire)
        forme = FormeLabyrinthe(rand() % 2);

    nouveau(Personnalise, lon, lar, a, type, forme, &mode);

    QTimer::singleShot(10, this, SLOT(resoudreModeEcran()));
}

void QLabyrinth::resoudreModeEcran()
{
    enResolution = true;

    if (!partieEnCours)
    {
        partieEnCours = true;
        if ((modeLabyrinthe.mode & Rotation) && !modeLabyrinthe.rotationFixe)
        {
            if (modeLabyrinthe.intervalleRotationAuHasard)
            {
                intervalleRotation = rand() % (INTERVALLEROTATIONMAX - INTERVALLEROTATIONMIN + 1) + INTERVALLEROTATIONMIN;
                indexTimerRotation = startTimer(intervalleRotation);
            }
            else
                indexTimerRotation = startTimer(modeLabyrinthe.intervalleRotation);
        }
        if (modeLabyrinthe.mode & Distorsion)
        {
            if (!modeLabyrinthe.distorsionVerticaleFixe)
            {
                if (modeLabyrinthe.intervalleDistorsionVerticaleAuHasard)
                {
                    intervalleDistorsionVerticale = rand() % (INTERVALLEDISTORSIONVERTICALEMAX - INTERVALLEDISTORSIONVERTICALEMIN + 1) + INTERVALLEDISTORSIONVERTICALEMIN;
                    indexTimerDistorsionVerticale = startTimer(intervalleDistorsionVerticale);
                }
                else
                    indexTimerDistorsionVerticale = startTimer(modeLabyrinthe.intervalleDistorsionVerticale);
            }
            if (!distorsionSynchronisee && !modeLabyrinthe.distorsionHorizontaleFixe)
            {
                if (modeLabyrinthe.intervalleDistorsionHorizontaleAuHasard)
                {
                    intervalleDistorsionHorizontale = rand() % (INTERVALLEDISTORSIONHORIZONTALEMAX - INTERVALLEDISTORSIONHORIZONTALEMIN + 1) + INTERVALLEDISTORSIONHORIZONTALEMIN;
                    indexTimerDistorsionHorizontale = startTimer(intervalleDistorsionHorizontale);
                }
                else
                    indexTimerDistorsionHorizontale = startTimer(modeLabyrinthe.intervalleDistorsionHorizontale);
            }
        }
        if (modeLabyrinthe.mode & Cisaillement)
        {
            if (!modeLabyrinthe.cisaillementVerticalFixe)
            {
                if (modeLabyrinthe.intervalleCisaillementVerticalAuHasard)
                {
                    intervalleCisaillementVertical = rand() % (INTERVALLECISAILLEMENTVERTICALMAX - INTERVALLECISAILLEMENTVERTICALMIN + 1) + INTERVALLECISAILLEMENTVERTICALMIN;
                    indexTimerCisaillementVertical = startTimer(intervalleCisaillementVertical);
                }
                else
                    indexTimerCisaillementVertical = startTimer(modeLabyrinthe.intervalleCisaillementVertical);
            }
            if (!cisaillementSynchronise && !modeLabyrinthe.cisaillementHorizontalFixe)
            {
                if (modeLabyrinthe.intervalleCisaillementHorizontalAuHasard)
                {
                    intervalleCisaillementHorizontal = rand() % (INTERVALLECISAILLEMENTHORIZONTALMAX - INTERVALLECISAILLEMENTHORIZONTALMIN + 1) + INTERVALLECISAILLEMENTHORIZONTALMIN;
                    indexTimerCisaillementHorizontal = startTimer(intervalleCisaillementHorizontal);
                }
                else
                    indexTimerCisaillementHorizontal = startTimer(modeLabyrinthe.intervalleCisaillementHorizontal);
            }
        }
    }

    while (!(labyrinth->player(playerId).state() & Labyrinth2d::Player::Finished) && !quitter)
    {
        resoudre(1);

        QElapsedTimer t;
        t.start();

        do
        {
            QApplication::processEvents(QEventLoop::AllEvents | QEventLoop::ExcludeSocketNotifiers);
        } while (t.elapsed() < double(rapiditeResolution) / 100 * 100);
    }

    if (indexTimerRotation)
    {
        killTimer(indexTimerRotation);
        indexTimerRotation = 0;
    }
    if (indexTimerDistorsionVerticale)
    {
        killTimer(indexTimerDistorsionVerticale);
        indexTimerDistorsionVerticale = 0;
    }
    if (indexTimerDistorsionHorizontale)
    {
        killTimer(indexTimerDistorsionHorizontale);
        indexTimerDistorsionHorizontale = 0;
    }
    if (indexTimerCisaillementVertical)
    {
        killTimer(indexTimerCisaillementVertical);
        indexTimerCisaillementVertical = 0;
    }
    if (indexTimerCisaillementHorizontal)
    {
        killTimer(indexTimerCisaillementHorizontal);
        indexTimerCisaillementHorizontal = 0;
    }

    if (!quitter)
        QTimer::singleShot(5000, this, SLOT(demarrerNouvellePartie()));
    else
    {
        QFile f("Labyrinthe_scr.ini");

        if (f.open(QFile::WriteOnly))// | QFile::Truncate
        {
            QDataStream data(&f);
            data.setVersion(QDataStream::Qt_4_7);

            enregistrer(data, false, 0, false, QString(), true, false, qobject_cast<MainWindow *>(scrollArea->parentWidget()->parentWidget())->getAdaptationTailleEcran(), qobject_cast<MainWindow *>(scrollArea->parentWidget()->parentWidget())->getAdaptationTaillePapier(), false);

            data << modesAleatoires;
            data << affichageAleatoire;
            data << tailleAleatoire;
            data << algorithmeAleatoire;
            data << rapiditeResolution;

            f.close();
        }

        QTimer::singleShot(0, qApp, SLOT(quit()));
    }
}

void QLabyrinth::wheelEvent(QWheelEvent *event)
{
    if (modeEcranDeVeille)
    {
        quitterModeEcranDeVeille();
        return;
    }

    QWidget::wheelEvent(event);
}

void QLabyrinth::joueurDeplace(int dx, int dy)
{
    if (!partieEnCours)
    {
        partieEnCours = true;
        emit partieCommencee();
    }
    if (enregistre)
    {
        enregistre = false;
        emit enregistrementChange();
    }
    if (!dx && !dy)
        return;
    if (!enResolution && getEffacerChemin() && labyrinth->grid().at(getEmplacementYJoueur() + dy,
                                                                    getEmplacementXJoueur() + dx))
        labyrinth->grid().reset(getEmplacementYJoueur(), getEmplacementXJoueur());/*
    emplacementXJoueur += dx;
    emplacementYJoueur += dy;
    labyrinthe[emplacementYJoueur][emplacementXJoueur] = 2;*/
    nombreDeplacement++;
    emit deplacementChange();
    if (!enResolution && getEmplacementXJoueur() == getXSortie() && getEmplacementYJoueur() == getYSortie())
    {
        QPalette p = scrollArea->palette();
        p.setColor(QPalette::Window, textures[0].couleur);
        scrollArea->setPalette(p);
        emit partieTerminee();
    }
    glLabyrinth->update();
}

void QLabyrinth::activer()
{
    setFocus();
    if (glLabyrinth)
        glLabyrinth->setFocus();
}

void QLabyrinth::rafraichir()
{
    update();
    if (glLabyrinth)
    {
        glLabyrinth->update();
        if (glLabyrinth->isVisible())
            glLabyrinth->update();
    }
}

void QLabyrinth::arreterResolution()
{
    arretResolution = true;

    labyrinth->player(playerId).stopSolving();

    if (glLabyrinth)
        glLabyrinth->arreterResolution();
}

bool QLabyrinth::getArretResolution() const
{
    return arretResolution;
}

void QLabyrinth::setTypeResolution(unsigned int type)
{
    assert(type < 4);

    typeResolution_ = type;

    if (glLabyrinth)
        glLabyrinth->setTypeResolution(typeResolution_);
}

unsigned int QLabyrinth::getTypeResolution() const
{
    return typeResolution_;
}


Labyrinth2d::Labyrinth const& QLabyrinth::getLabyrinth() const
{
    return *labyrinth;
}
