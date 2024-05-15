#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QCheckBox>
#include <QTime>
#include <QTimer>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QGroupBox>
#include <QComboBox>
#include <QPrinter>
#include <QRadioButton>
#include <QTranslator>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <fmod.h>

class Labyrinth;

#define NOMBRESCORES 10
#define MUSIQUE QString("./Animation.mid")
#define MUET false
#define VERSION QString("3.0")

class TabWidget : public QTabWidget
{
    public:
        TabWidget(QWidget *parent = 0) : QTabWidget(parent)
        {
        }
        QTabBar *tabBar() const
        {
            return QTabWidget::tabBar();
        }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        struct Score
        {
            QString nom;
            int deplacement;
            QTime temps;
        };
        enum Langue
        {
            Systeme = 0,
            Francais = 1,
            Anglais = 2,
            Espagnol = 3,
            Allemand = 4,
            Italien = 5
        };
        MainWindow();
        ~MainWindow();
        void chargerPartie(const QString &fichier);
        QString getNomPartie() const;
        bool getAdaptationTailleEcran() const;
        void setAdaptationTailleEcran(bool oui);
        bool getAdaptationTaillePapier() const;
        void setAdaptationTaillePapier(bool oui);

    private:
        QLabyrinth *labyrinth;
        QMenu *menuPartie;
        QMenu *menuOptions;
        QMenu *menuAide;
        QAction *actionNouvellePartie;
        QAction *actionRecommencer;
        QAction *actionMettreEnPause;
        QAction *actionResoudre;
        QAction *actionResolutionProgressive;
        QAction *actionOuvrir;
        QAction *actionEnregistrer;
        QAction *actionEnregistrerSous;
        QAction *actionFacile;
        QAction *actionMoyen;
        QAction *actionDifficile;
        QAction *actionPersonnalise;
        QAction *actionApercu;
        QAction *actionImprimer;
        QAction *actionMiseEnPage;
        QAction *actionMeilleursTemps;
        QAction *actionQuitter;
        QAction *actionPleinEcran;
        QAction *actionLabyrintheSeulement;
        QAction *actionAfficherTrace;
        QAction *actionEffacerChemin;
        QAction *actionAffichage;
        QAction *actionMusique;
        QAction *actionModes;
        QAction *actionButDuJeu;
        QAction *actionCommandes;
        QAction *actionAPropos;
        QAction *actionAProposQt;
        QAction *actionLangue;
        QMenu *menuTypeLabyrinthe;
        QAction *actionLabyrinthe2D;
        QAction *actionLabyrinthe2Den3D;
        QMenu *menuAlgorithmes;
        QList<QAction *> actionsAlgorithmes;
        QLabel *niveau;
        QLabel *deplacement;
        QCheckBox *chronometre;
        QTime temps;
        QTimer *timer;
        QScrollArea *scrollArea;
        QList<QList<Score> > scores2D;
        QList<QList<Score> > scores2Den3D;
        QPushButton *boutonCouleurFond;
        QLineEdit *lineEditMotifFond;
        QLineEdit *lineEditImageFond;
        QPushButton *boutonCouleurMur;
        QLineEdit *lineEditMotifMur;
        QLineEdit *lineEditImageMur;
        QPushButton *boutonCouleurParcours;
        QLineEdit *lineEditMotifParcours;
        QLineEdit *lineEditImageParcours;
        QSpinBox *spinBoxLongueur;
        QSpinBox *spinBoxLargeur;
        QGroupBox *groupBoxChronometre;
        QGroupBox *groupBoxDeplacement;
        QGroupBox *groupBoxNiveau;
        QGroupBox *groupBoxTailleLabyrinthe;
        bool adaptationEcran;
        bool adaptationFormats;
        QList<QLabel *> labelsNomsFacile;
        QList<QLabel *> labelsTempsFacile;
        QList<QLabel *> labelsDeplacementsFacile;
        QList<QLabel *> labelsNomsMoyen;
        QList<QLabel *> labelsTempsMoyen;
        QList<QLabel *> labelsDeplacementsMoyen;
        QList<QLabel *> labelsNomsDifficile;
        QList<QLabel *> labelsTempsDifficile;
        QList<QLabel *> labelsDeplacementsDifficile;
        QRadioButton *radioButtonScores2D;
        QCheckBox *checkBoxAdaptation;
        QCheckBox *checkBoxFormats;
        QPushButton *boutonConfigurerFormats;
        int ms;
        QPrinter *printer;
        QString emplacementMusique;
        Phonon::AudioOutput *audioOutput;
        Phonon::MediaObject *mediaObject;
        Phonon::Path path;
        FMOD_SYSTEM *system;
        FMOD_SOUND *m;
        QLineEdit *lineEditMusique;
        QCheckBox *checkBoxMuet;
        QString fichierEnregistrement;
        bool pauseImposee;
        bool nouveau;
        QRadioButton *couleurFond;
        QRadioButton *motifFond;
        QRadioButton *imageFond;
        QRadioButton *couleurMur;
        QRadioButton *motifMur;
        QRadioButton *imageMur;
        QRadioButton *couleurParcours;
        QRadioButton *motifParcours;
        QRadioButton *imageParcours;
        QTabWidget *tabWidgetScores;
        TabWidget *tabWidgetModes;
        QPushButton *boutonCouleurObscurite;
        QSpinBox *spinBoxRayonObscurite;
        QSpinBox *spinBoxAngleRotation;
        QCheckBox *checkBoxAngleRotationAuHasard;
        QSpinBox *spinBoxIntervalleRotation;
        QCheckBox *checkBoxIntervalleRotationAuHasard;
        QCheckBox *checkBoxSensRotation;
        QCheckBox *checkBoxSensRotationAuHasard;
        QCheckBox *checkBoxRotationFixe;
        QSpinBox *spinBoxPourcentageDistorsionVerticale;
        QSpinBox *spinBoxPourcentageDistorsionHorizontale;
        QCheckBox *checkBoxPourcentageDistorsionVerticaleAuHasard;
        QCheckBox *checkBoxPourcentageDistorsionHorizontaleAuHasard;
        QSpinBox *spinBoxIntervalleDistorsionVerticale;
        QSpinBox *spinBoxIntervalleDistorsionHorizontale;
        QCheckBox *checkBoxIntervalleDistorsionVerticaleAuHasard;
        QCheckBox *checkBoxIntervalleDistorsionHorizontaleAuHasard;
        QCheckBox *checkBoxDistorsionVerticaleFixe;
        QCheckBox *checkBoxDistorsionHorizontaleFixe;
        QCheckBox *checkBoxDistorsionSynchronisee;
        QCheckBox *checkBoxDistorsionSynchroniseeAuHasard;
        QSpinBox *spinBoxPourcentageCisaillementVertical;
        QSpinBox *spinBoxPourcentageCisaillementHorizontal;
        QCheckBox *checkBoxPourcentageCisaillementVerticalAuHasard;
        QCheckBox *checkBoxPourcentageCisaillementHorizontalAuHasard;
        QSpinBox *spinBoxIntervalleCisaillementVertical;
        QSpinBox *spinBoxIntervalleCisaillementHorizontal;
        QCheckBox *checkBoxIntervalleCisaillementVerticalAuHasard;
        QCheckBox *checkBoxIntervalleCisaillementHorizontalAuHasard;
        QCheckBox *checkBoxCisaillementVerticalFixe;
        QCheckBox *checkBoxCisaillementHorizontalFixe;
        QCheckBox *checkBoxCisaillementSynchronise;
        QCheckBox *checkBoxCisaillementSynchroniseAuHasard;
        Langue langueChoisie;
        QComboBox *comboBoxLangue;
        QTranslator *translatorLabyrinthe;
        QTranslator *translatorQt;
        QString nomPartie;
        QString dernierNomEnregistre;
        void actualiserLangue();

    private slots:
        void commencer();
        void arreter();
        void actualiserChronometre();
        void nouvellePartie();
        void recommencer();
        void mettreEnPause();
        void resoudre();
        void resolutionProgressive();
        void ouvrir();
        void enregistrer();
        void enregistrerSous();
        void facile();
        void moyen();
        void difficile();
        void personnalise();
        void imprimer();
        void apercu();
        void miseEnPage();
        void meilleursTemps();
        void quitter();
        void affichage();
        void musique();
        void butDuJeu();
        void commandes();
        void aPropos();
        void stateChanged(int);
        void choisirCouleurFond();
        void reinitialiserCouleurFond();
        void choisirMotifFond();
        void reinitialiserMotifFond();
        void choisirImageFond();
        void reinitialiserImageFond();
        void choisirCouleurMur();
        void reinitialiserCouleurMur();
        void choisirMotifMur();
        void reinitialiserMotifMur();
        void choisirImageMur();
        void reinitialiserImageMur();
        void choisirCouleurParcours();
        void reinitialiserCouleurParcours();
        void choisirMotifParcours();
        void reinitialiserMotifParcours();
        void choisirImageParcours();
        void reinitialiserImageParcours();
        void reinitialiserLongueur();
        void reinitialiserLargeur();
        void reinitialiserAffichage();
        void pleinEcran();
        void labyrintheSeulement();
        void stateChangedAdapation(int);
        void stateChangedFormats(int);
        void configurerFormats();
        void modes();
        void changementCategorieScores();
        void reinitialiserScores();
        void apercuLabyrinthe(QPrinter *p);
        void reinitialiserMusique();
        void parcourirMusique();
        void afficherTrace();
        void effacerChemin();
        void changerEnregistrement();
        void pause();
        void type();
        void algorithme();
        void reinitialiserModes();
        void choisirCouleurObscurite();
        void reinitialiserCouleurObscurite();
        void reinitialiserRayonObscurite();
        void reinitialiserAngleRotation();
        void reinitialiserIntervalleRotation();
        void reinitialiserSensRotation();
        void reinitialiserPourcentageDistorsionVerticale();
        void reinitialiserPourcentageDistorsionHorizontale();
        void reinitialiserIntervalleDistorsionVerticale();
        void reinitialiserIntervalleDistorsionHorizontale();
        void reinitialiserDistorsionSynchronisee();
        void synchroniserDistorsion(int state);
        void reinitialiserPourcentageCisaillementVertical();
        void reinitialiserPourcentageCisaillementHorizontal();
        void reinitialiserIntervalleCisaillementVertical();
        void reinitialiserIntervalleCisaillementHorizontal();
        void reinitialiserCisaillementSynchronise();
        void synchroniserCisaillement(int state);
        void langue();
        void reinitialiserLangue();
        void actualiserDeplacement();

    protected:
        void changeEvent(QEvent *event);
        void closeEvent(QCloseEvent *event);
        bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MAINWINDOW_H
