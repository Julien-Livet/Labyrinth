#ifndef QLABYRINTH_H
#define QLABYRINTH_H

#include <QWidget>
#include <QColor>
#include <QPixmap>
#include <QTimer>
#include <QScrollArea>
#include <QMenu>
#include <QPaintDevice>
#include <QDataStream>
#include <QTime>

#include "Labyrinth2D/Labyrinth.h"

class GLLabyrinth;

class QLabyrinth : public QWidget
{
    Q_OBJECT

    public:
        enum TypeLabyrinthe
        {
            Labyrinthe2D = 0,
            Labyrinthe2Den3D = 1
        };
        enum TypeTexture
        {
            TextureCouleur = 0,
            TextureMotif = 1,
            TextureImage = 2
        };
        enum Niveau
        {
            Facile = 0,
            Moyen = 1,
            Difficile = 2,
            Personnalise = 3
        };
        struct Texture
        {
            TypeTexture typeTexture;
            QColor couleur;
            QString motif;
            QString image;
        };
        enum Mode
        {
            Aucun = 0,
            Rotation = 1,
            Obscurite = 2,
            Distorsion = 4,
            Cisaillement = 8/*,
            InversionTouches = 16*/
        };
        enum Algorithme
        {
            FirstDepthSearch = 0,
            CellFusion = 1,
            RecursiveDivision = 2,
            PrimsAlgorithm = 3,
            HuntAndKillAlgorithm = 4,
            GrowingTreeAlgorithm = 5,
            FractaleAlgorithm = 6,
            HomeMadeAlgorithm1 = 7,
            HomeMadeAlgorithm2 = 8,
            HomeMadeAlgorithm3 = 9
        };
        enum FormeLabyrinthe
        {
            Rectangle = 0/*,
            Ellipse = 1*/
        };
        struct ModeLabyrinthe
        {
            unsigned int mode;
            qreal angleRotation;
            bool angleRotationAuHasard;
            bool rotationFixe;
            int intervalleRotation;
            bool intervalleRotationAuHasard;
            int sensRotation;
            bool sensRotationAuHasard;
            QColor couleurObscurite;
            int rayonObscurite;
            int pourcentageDistorsionVerticale;
            int pourcentageDistorsionHorizontale;
            bool pourcentageDistorsionVerticaleAuHasard;
            bool pourcentageDistorsionHorizontaleAuHasard;
            int intervalleDistorsionVerticale;
            int intervalleDistorsionHorizontale;
            bool intervalleDistorsionVerticaleAuHasard;
            bool intervalleDistorsionHorizontaleAuHasard;
            bool distorsionVerticaleFixe;
            bool distorsionHorizontaleFixe;
            bool distorsionSynchronisee;
            bool distorsionSynchroniseeAuHasard;
            int pourcentageCisaillementVertical;
            int pourcentageCisaillementHorizontal;
            bool pourcentageCisaillementVerticalAuHasard;
            bool pourcentageCisaillementHorizontalAuHasard;
            int intervalleCisaillementVertical;
            int intervalleCisaillementHorizontal;
            bool intervalleCisaillementVerticalAuHasard;
            bool intervalleCisaillementHorizontalAuHasard;
            bool cisaillementVerticalFixe;
            bool cisaillementHorizontalFixe;
            bool cisaillementSynchronise;
            bool cisaillementSynchroniseAuHasard;
        };
        QLabyrinth(QScrollArea *parent, Niveau n = Facile, int longueurLabyrinthe = 5, int largeurLabyrinthe = 5, Algorithme a = FirstDepthSearch, TypeLabyrinthe type = Labyrinthe2D, FormeLabyrinthe forme = Rectangle, ModeLabyrinthe *mode = 0);
		~QLabyrinth();
        int getXEntree() const;
        int getYEntree() const;
        int getXSortie() const;
        int getYSortie() const;
        int getLargeur() const;
        int getLongueur() const;
        int getEmplacementXJoueur() const;
        int getEmplacementYJoueur() const;
        bool getEnResolution() const;
        TypeLabyrinthe getTypeLabyrinthe() const;
        FormeLabyrinthe getFormeLabyrinthe() const;
        Texture getTextureParcours() const;
        void setTextureParcours(const Texture &texture);
        Texture getTextureMur() const;
        void setTextureMur(const Texture &texture);
        Texture getTextureFond() const;
        void setTextureFond(const Texture &texture);
        bool getPartieEnCours() const;
        bool getPartieEnPause() const;
        QSize getTailleCase() const;
        void setTailleCase(const QSize &taille);
        Niveau getNiveau() const;
        void nouveau(Niveau n, int longueurLabyrinthe = 5, int largeurLabyrinthe = 5, Algorithme a = FirstDepthSearch, TypeLabyrinthe type = Labyrinthe2D, FormeLabyrinthe forme = Rectangle, ModeLabyrinthe *mode = 0);
        void recommencer();
        bool getPartieTerminee() const;
        void mettreEnPauseRedemarrer();
        void resoudre();
        bool getResolutionProgressive() const;
        void setResolutionProgressive(bool b);
        void apercu(QPaintDevice *painterDevice, QRect visibleRect = QRect());
        void apercu(QPainter *painter, QRect visibleRect = QRect());
        bool getEffacerChemin() const;
        void setEffacerChemin(bool b);
        bool getAfficherTrace() const;
        void setAfficherTrace(bool b);
        bool getEnregistre() const;
        void enregistrer(QDataStream &data, bool chrono, int ms, bool pauseImposee, const QString &musique, bool muet, bool labySeulement, bool adaptationTailleEcran, bool adaptationTaillePapier, bool enregistrerLabyrinthe = true);
        void charger(QDataStream &data, bool &chrono, int &ms, QString &musique, bool &muet, bool &labySeulement, bool &adaptationTailleEcran, bool &adaptationTaillePapier);
        ModeLabyrinthe getModeLabyrinthe() const;
        Algorithme getAlgorithme() const;
        qreal getNombreDeplacement() const;
        void passerEnModeEcranDeVeille();
        bool enModeEcranDeVeille() const;
        void quitterModeEcranDeVeille();
        void activer();
        void rafraichir();
        void arreterResolution();
        bool getArretResolution() const;
		bool getEffacerChemin() const;
		void setEffacerChemin(bool effacerChemin);

    signals:
        void partieCommencee();
        void partieRedemarree();
        void partieTerminee();
        void enregistrementChange();
        void deplacementChange();

    private:
		QList<QList<int> > labyrinthe; // to be unused
        Labyrinth2d::Labyrinth* labyrinth;
		Labyrinth2d::QKeyPress* qKeyPress;
		size_t playerId;
        //int xEntree;
        //int yEntree;
        //int xSortie;
        //int ySortie;
        //int largeur;
        //int longueur;
        //int emplacementXJoueur;
        //int emplacementYJoueur;
        bool enResolution;
        bool resolutionProgressive;
        //bool effacerChemin;
        bool afficherTrace;
        bool enregistre;
        Niveau niveau;
        TypeLabyrinthe typeLabyrinthe;
        FormeLabyrinthe formeLabyrinthe;
        Texture textures[3];//0 pour le fond, 1 pour le mur, 2 pour le parcours
        bool partieEnCours;
        bool partieEnPause;
        bool enConstruction;
        QSize tailleCase;
        QScrollArea *scrollArea;
        QMenu *menu;
        bool full;
        bool max;
        int scrollBarHValue;
        int scrollBarVValue;
        QSize s;
        QByteArray state;
        ModeLabyrinthe modeLabyrinthe;
        qreal nombreDeplacement;
        Algorithme algorithme;
        qreal angle;
        int intervalleRotation;
        int indexTimerRotation;
        QTime tR;
        QTime tDV;
        QTime tDH;
        int tempsRestantRotation;
        int tempsRestantDistorsionVerticale;
        int tempsRestantDistorsionHorizontale;
        int tempsRestantCisaillementVertical;
        int tempsRestantCisaillementHorizontal;
        int numeroQuart;
        QTransform transform;
        QPoint anciennePosSouris;
        int sensRotation;
        int sensDistorsionVerticale;
        int sensDistorsionHorizontale;
        int intervalleDistorsionVerticale;
        int indexTimerDistorsionVerticale;
        int intervalleDistorsionHorizontale;
        int indexTimerDistorsionHorizontale;
        bool distorsionSynchronisee;
        int pourcentageDistorsionVerticaleCourant;
        int pourcentageDistorsionHorizontaleCourant;
        int pourcentageDistorsionVerticale;
        int pourcentageDistorsionHorizontale;
        int sensCisaillementVertical;
        int sensCisaillementHorizontal;
        int intervalleCisaillementVertical;
        int indexTimerCisaillementVertical;
        int intervalleCisaillementHorizontal;
        int indexTimerCisaillementHorizontal;
        bool cisaillementSynchronise;
        int pourcentageCisaillementVerticalCourant;
        int pourcentageCisaillementHorizontalCourant;
        int pourcentageCisaillementVertical;
        int pourcentageCisaillementHorizontal;
        bool modeEcranDeVeille;
        bool modesAleatoires;
        bool affichageAleatoire;
        bool tailleAleatoire;
        bool algorithmeAleatoire;
        bool typeAleatoire;
        bool formeAleatoire;
        int rapiditeResolution;
        bool quitter;
        bool arretResolution;
        GLLabyrinth *glLabyrinth;
        bool resoudre(int nombreFois);
        void construireAncienBis();
        void construireAncien();
        void construireHomeMadeAlgorithm1();
        void construireHomeMadeAlgorithm2();
        void construireHomeMadeAlgorithm3();
        void choisirDirection(int &d, int &e, int x, int y);
        void mouvementSouris(const QPoint &point);
        void routineDeplacement1();
        void routineDeplacement2();
        void calculerTransformation();

    private slots:
        void redimensionnerFenetrePrincipale();
        void maximiserFenetre();
        void actualiserChargement();
        void demarrerNouvellePartie();
        void resoudreModeEcran();
        void joueurDeplace(int dx, int dy);
        void solveLabyrinth();

    protected:
        void paintEvent(QPaintEvent *event);
        void keyPressEvent(QKeyEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void timerEvent(QTimerEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);
};

#define TYPETEXTUREFOND QLabyrinth::TextureCouleur
#define TYPETEXTUREMUR QLabyrinth::TextureCouleur
#define TYPETEXTUREPARCOURS QLabyrinth::TextureCouleur

#endif // QLABYRINTH_H
