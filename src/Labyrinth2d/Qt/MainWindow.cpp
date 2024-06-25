#include <QActionGroup>
#include <QApplication>
#include <QButtonGroup>
#include <QColorDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QMenuBar>
#include <QScrollBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QTabBar>
#include <QVBoxLayout>

#include "Labyrinth2d/Qt/MainWindow.h"
#include "Labyrinth2d/Qt/QLabyrinth.h"
#include "Labyrinth2d/Qt/constants.h"

class ScrollArea : public QScrollArea
{
    public:
        ScrollArea(QWidget *parent = 0) : QScrollArea(parent)
        {
            menu = new QMenu(this);
            menu->hide();
        }

    private:
        QMenu *menu;

    protected:
        void scrollContentsBy(int dx, int dy)
        {
            QScrollArea::scrollContentsBy(dx, dy);

            qobject_cast<QLabyrinth *>(widget())->rafraichir();
        }

        void mousePressEvent(QMouseEvent *event)
        {
            QWidget::mousePressEvent(event);

            if (event->button() & Qt::RightButton)
            {
                    menu->clear();
                    menu->addActions(actions());
                    menu->popup(QCursor::pos());
            }
        }
};

MainWindow::MainWindow()
{
    adaptationEcran = false;
    adaptationFormats = false;
    ms = 0;
    emplacementMusique = MUSIQUE;
    pauseImposee = false;
    nouveau = true;
    bool muet = MUET;

    printer = new QPrinter;
    printer->setPageOrientation(QPageLayout::Landscape);
    timer = new QTimer(this);
    timer->setInterval(10);
    timer->setSingleShot(false);
    temps = QTime(0, 0, 0);
    langueChoisie = Systeme;
    translatorLabyrinthe = new QTranslator(this);
    translatorQt = new QTranslator(this);
    translatorQtBase = new QTranslator(this);
    nomPartie = tr("Partie");

    connect(timer, SIGNAL(timeout()), this, SLOT(actualiserChronometre()));

    audioOutput = new QAudioOutput();
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setAudioOutput(audioOutput);

    for (int i = 0; i < 3; i++)
    {
        scores2D << QList<Score>();
        scores2Den3D << QList<Score>();
    }

    scrollArea = new ScrollArea(this);
    labyrinth = new QLabyrinth(scrollArea, QLabyrinth::Personnalise, 5, 5);

    bool chrono, labySeulement, ouvertureReussie = false;
    QString mus = MUSIQUE;

    QFile f("Labyrinth.ini");

    if (f.open(QFile::ReadOnly))
    {
        temps = QTime(0, 0, 0);
        ms = 0;
        timer->stop();

        QDataStream data(&f);
        data.setVersion(QDataStream::Qt_4_7);

        labyrinth->charger(data, chrono, ms, mus, muet, labySeulement, adaptationEcran, adaptationFormats);

        for (int i = 0; i < 3; i++)
        {
            scores2D[i].clear();
            qsizetype nbscores2D{0};
            data >> nbscores2D;
            for (int j = 0 ; j < nbscores2D; j++)
            {
                Score s;
                data >> s.nom;
                data >> s.temps;
                data >> s.deplacement;
                scores2D[i] << s;
            }
        }
        for (int i = 0; i < 3; i++)
        {
            scores2Den3D[i].clear();
            qsizetype nbscores2Den3D{0};
            data >> nbscores2Den3D;
            for (int j = 0 ; j < nbscores2Den3D; j++)
            {
                Score s;
                data >> s.nom;
                data >> s.temps;
                data >> s.deplacement;
                scores2Den3D[i] << s;
            }
        }

        int entier;
        data >> entier;
        langueChoisie = Langue(entier);
        data >> dernierNomEnregistre;

        f.close();

        actualiserLangue();

        ouvertureReussie = true;
    }
    else
        actualiserLangue();

    menuPartie = new QMenu(tr("&Partie"), this);
    menuOptions = new QMenu(tr("&Options"), this);
    menuAide = new QMenu(tr("&Aide"), this);

    actionNouvellePartie = new QAction(tr("&Nouvelle partie"), this);
    actionNouvellePartie->setShortcut(tr("Ctrl+N"));
    actionRecommencer = new QAction(tr("Re&commencer"), this);
    actionRecommencer->setShortcut(tr("Ctrl+C"));
    actionMettreEnPause = new QAction(tr("&Mettre en pause"), this);
    actionMettreEnPause->setShortcut(tr("P"));
    actionMettreEnPause->setAutoRepeat(false);
    actionMettreEnPause->setCheckable(true);

    menuAlgorithmesResolution = new QMenu(tr("Algorithme de résol&ution"), this);

    actionsAlgorithmesResolution << new QAction(tr("&A-Star"), this);
    actionsAlgorithmesResolution << new QAction(tr("Main &droite au mur"), this);
    actionsAlgorithmesResolution << new QAction(tr("Main &gauche au mur"), this);
    actionsAlgorithmesResolution << new QAction(tr("A&veugle"), this);

    QActionGroup *actionGroupAlgorithmesResolution = new QActionGroup(this);

    for (int i = 0; i < actionsAlgorithmesResolution.size(); i++)
    {
        actionsAlgorithmesResolution[i]->setCheckable(true);
        actionGroupAlgorithmesResolution->addAction(actionsAlgorithmesResolution[i]);
        menuAlgorithmesResolution->addAction(actionsAlgorithmesResolution[i]);
        connect(actionsAlgorithmesResolution[i], SIGNAL(triggered()), this, SLOT(algorithmeResolution()));
    }

    actionsAlgorithmesResolution[0]->setChecked(true);

    actionResoudre = new QAction(tr("&Résoudre"), this);
    actionResoudre->setShortcut(tr("Ctrl+R"));
    actionResolutionProgressive = new QAction(tr("Réso&lution progressive"), this);
    actionResolutionProgressive->setCheckable(true);
    actionResolutionProgressive->setToolTip(tr("Déconseillée pour les grands labyrinthes"));
    actionOuvrir = new QAction(tr("&Ouvrir"), this);
    actionOuvrir->setShortcut(tr("Ctrl+O"));
    actionEnregistrer = new QAction(tr("&Enregistrer"), this);
    actionEnregistrer->setShortcut(tr("Ctrl+S"));
    actionEnregistrerSous = new QAction(tr("Enregistrer &sous ..."), this);
    actionFacile = new QAction(tr("&Facile"), this);
    actionFacile->setCheckable(true);
    actionFacile->setChecked(true);
    actionMoyen = new QAction(tr("&Moyen"), this);
    actionMoyen->setCheckable(true);
    actionDifficile = new QAction(tr("&Difficile"), this);
    actionDifficile->setCheckable(true);
    actionPersonnalise = new QAction(tr("&Personnalisé"), this);
    actionPersonnalise->setCheckable(true);
    actionMiseEnPage = new QAction(tr("Mise en pa&ge"), this);
    actionApercu = new QAction(tr("&Aperçu avant impression"), this);
    actionImprimer = new QAction(tr("&Imprimer"), this);
    actionImprimer->setShortcut(tr("Ctrl+P"));
    actionMeilleursTemps = new QAction(tr("Meilleurs &temps"), this);
    actionQuitter = new QAction(tr("&Quitter"), this);
    actionQuitter->setShortcut(tr("Ctrl+Q"));
    actionPleinEcran = new QAction(tr("&Plein écran"), this);
    actionPleinEcran->setCheckable(true);
    actionPleinEcran->setShortcut(tr("F11"));
    actionLabyrintheSeulement = new QAction(tr("&Labyrinthe seulement"), this);
    actionLabyrintheSeulement->setCheckable(true);
    actionLabyrintheSeulement->setShortcut(tr("Esc"));
    actionEffacerChemin = new QAction(tr("&Effacer le chemin"), this);
    actionEffacerChemin->setCheckable(true);
    actionEffacerChemin->setVisible(false);
    actionAfficherTrace = new QAction(tr("A&fficher la trace"), this);
    actionAfficherTrace->setCheckable(true);
    actionAffichage = new QAction(tr("&Affichage"), this);
    actionMusique = new QAction(tr("Mu&sique"), this);
    actionModes = new QAction(tr("&Modes"), this);
    actionLangue = new QAction(tr("&Langue"), this);
    actionButDuJeu = new QAction(tr("&But du jeu"), this);
    actionCommandes = new QAction(tr("&Commandes"), this);
    actionAPropos = new QAction(tr("À propos du &Labyrinthe ..."), this);
    actionAProposQt = new QAction(tr("À propos de &Qt ..."), this);

    menuTypeLabyrinthe = new QMenu(tr("&Type"));
    actionLabyrinthe2D = new QAction(tr("Labyrinthe 2D"), this);
    actionLabyrinthe2D->setCheckable(true);
    actionLabyrinthe2Den3D = new QAction(tr("Labyrinthe 2D en 3D"), this);
    actionLabyrinthe2Den3D->setCheckable(true);

    QActionGroup *actionGroupType = new QActionGroup(this);
    actionGroupType->addAction(actionLabyrinthe2D);
    actionGroupType->addAction(actionLabyrinthe2Den3D);

    connect(actionLabyrinthe2D, SIGNAL(triggered()), this, SLOT(type()));
    connect(actionLabyrinthe2Den3D, SIGNAL(triggered()), this, SLOT(type()));

    menuTypeLabyrinthe->addAction(actionLabyrinthe2D);
    menuTypeLabyrinthe->addAction(actionLabyrinthe2Den3D);

    menuAlgorithmesGeneration = new QMenu(tr("&Algorithme de génération"), this);

    actionsAlgorithmesGeneration << new QAction(tr("&Recherche en première profondeur"), this);
    actionsAlgorithmesGeneration << new QAction(tr("&Fusion de cellules"), this);
    actionsAlgorithmesGeneration << new QAction(tr("&Division récursive"), this);
    actionsAlgorithmesGeneration << new QAction(tr("Algorithme de &Prim"), this);
    actionsAlgorithmesGeneration << new QAction(tr("Algorithme de &chasse et de tuerie"), this);
    actionsAlgorithmesGeneration << new QAction(tr("Algorithme de l'&arbre croissant"), this);
    actionsAlgorithmesGeneration << new QAction(tr("Algorithme des &fractales"), this);/*
    actionsAlgorithmesGeneration << new QAction(tr("Algorithme fait-maison n°&1"), this);
    actionsAlgorithmesGeneration << new QAction(tr("Algorithme fait-maison n°&2"), this);
    actionsAlgorithmesGeneration << new QAction(tr("Algorithme fait-maison n°&3"), this);*/

    QActionGroup *actionGroupAlgorithmesGeneration = new QActionGroup(this);

    for (int i = 0; i < actionsAlgorithmesGeneration.size(); i++)
    {
        actionsAlgorithmesGeneration[i]->setCheckable(true);
        actionGroupAlgorithmesGeneration->addAction(actionsAlgorithmesGeneration[i]);
        menuAlgorithmesGeneration->addAction(actionsAlgorithmesGeneration[i]);
        connect(actionsAlgorithmesGeneration[i], SIGNAL(triggered()), this, SLOT(algorithmeGeneration()));
    }

    menuPartie->addAction(actionNouvellePartie);
    menuPartie->addAction(actionRecommencer);
    menuPartie->addAction(actionMettreEnPause);
    menuPartie->addMenu(menuAlgorithmesResolution);
    menuPartie->addAction(actionResoudre);
    menuPartie->addAction(actionResolutionProgressive);
    menuPartie->addSeparator();
    menuPartie->addAction(actionOuvrir);
    menuPartie->addAction(actionEnregistrer);
    menuPartie->addAction(actionEnregistrerSous);
    menuPartie->addSeparator();
    menuPartie->addAction(actionFacile);
    menuPartie->addAction(actionMoyen);
    menuPartie->addAction(actionDifficile);
    menuPartie->addAction(actionPersonnalise);
    menuPartie->addSeparator();
    menuPartie->addAction(actionMeilleursTemps);
    menuPartie->addSeparator();
    menuPartie->addAction(actionMiseEnPage);
    menuPartie->addAction(actionApercu);
    menuPartie->addAction(actionImprimer);
    menuPartie->addSeparator();
    menuPartie->addAction(actionQuitter);

    menuOptions->addMenu(menuTypeLabyrinthe);
    menuOptions->addMenu(menuAlgorithmesGeneration);
    menuOptions->addAction(actionModes);
    menuOptions->addSeparator();
    menuOptions->addAction(actionEffacerChemin);
    menuOptions->addAction(actionAfficherTrace);
    menuOptions->addSeparator();
    menuOptions->addAction(actionPleinEcran);
    menuOptions->addAction(actionLabyrintheSeulement);
    menuOptions->addSeparator();
    menuOptions->addAction(actionAffichage);
    menuOptions->addAction(actionMusique);
    menuOptions->addAction(actionLangue);

    menuAide->addAction(actionButDuJeu);
    menuAide->addAction(actionCommandes);
    menuAide->addSeparator();
    menuAide->addAction(actionAPropos);
    menuAide->addAction(actionAProposQt);

    menuBar()->addMenu(menuPartie);
    menuBar()->addMenu(menuOptions);
    menuBar()->addMenu(menuAide);

    QActionGroup *actionGroup = new QActionGroup(this);
    actionGroup->addAction(actionFacile);
    actionGroup->addAction(actionMoyen);
    actionGroup->addAction(actionDifficile);
    actionGroup->addAction(actionPersonnalise);

    QWidget *widget = new QWidget(this);

    scrollArea->setAlignment(Qt::AlignCenter);
    //scrollArea->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    niveau = new QLabel(this);
    deplacement = new QLabel(this);
    chronometre = new QCheckBox(tr("&Chronomètre : %1").arg(temps.toString(tr("hh:mm:ss"))), this);

    groupBoxNiveau = new QGroupBox;
    QHBoxLayout *hBoxLayoutNiveau = new QHBoxLayout;
    hBoxLayoutNiveau->addWidget(niveau);
    groupBoxNiveau->setLayout(hBoxLayoutNiveau);

    groupBoxDeplacement = new QGroupBox;
    QHBoxLayout *hBoxLayoutDeplacement = new QHBoxLayout;
    hBoxLayoutDeplacement->addWidget(deplacement);
    groupBoxDeplacement->setLayout(hBoxLayoutDeplacement);

    groupBoxChronometre = new QGroupBox;
    QHBoxLayout *hBoxLayoutChronometre = new QHBoxLayout;
    hBoxLayoutChronometre->addWidget(chronometre);
    groupBoxChronometre->setLayout(hBoxLayoutChronometre);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(scrollArea, 0, 0, 1, 3);//, Qt::AlignCenter);
    gridLayout->addWidget(groupBoxNiveau, 1, 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);
    gridLayout->addWidget(groupBoxDeplacement, 1, 1, 1, 1, Qt::AlignCenter);
    gridLayout->addWidget(groupBoxChronometre, 1, 2, 1, 1, Qt::AlignVCenter | Qt::AlignRight);
    widget->setLayout(gridLayout);

    //niveau->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //chronometre->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //chronometre->setMinimumWidth(chronometre->width()*11/8);

    labyrinth->addAction(menuPartie->menuAction());
    labyrinth->addAction(menuOptions->menuAction());
    labyrinth->addAction(menuAide->menuAction());
    labyrinth->activer();
    scrollArea->addAction(menuPartie->menuAction());
    scrollArea->addAction(menuOptions->menuAction());
    scrollArea->addAction(menuAide->menuAction());
    scrollArea->setWidget(labyrinth);
    scrollArea->setFocusProxy(labyrinth);

    setCentralWidget(widget);

    setWindowIcon(QIcon(":/Images/resources/Labyrinth.ico"));
    setWindowTitle(((labyrinth->getEnregistre()) ? QString() : QString("*")) + tr("%1 - Labyrinthe").arg(nomPartie));

    connect(actionNouvellePartie, SIGNAL(triggered()), this, SLOT(nouvellePartie()));
    connect(actionRecommencer, SIGNAL(triggered()), this, SLOT(recommencer()));
    connect(actionMettreEnPause, SIGNAL(triggered()), this, SLOT(pause()));
    connect(actionResoudre, SIGNAL(triggered()), this, SLOT(resoudre()));
    connect(actionResolutionProgressive, SIGNAL(triggered()), this, SLOT(resolutionProgressive()));
    connect(actionOuvrir, SIGNAL(triggered()), this, SLOT(ouvrir()));
    connect(actionEnregistrer, SIGNAL(triggered()), this, SLOT(enregistrer()));
    connect(actionEnregistrerSous, SIGNAL(triggered()), this, SLOT(enregistrerSous()));
    connect(actionFacile, SIGNAL(triggered()), this, SLOT(facile()));
    connect(actionMoyen, SIGNAL(triggered()), this, SLOT(moyen()));
    connect(actionDifficile, SIGNAL(triggered()), this, SLOT(difficile()));
    connect(actionPersonnalise, SIGNAL(triggered()), this, SLOT(personnalise()));
    connect(actionMiseEnPage, SIGNAL(triggered()), this, SLOT(miseEnPage()));
    connect(actionApercu, SIGNAL(triggered()), this, SLOT(apercu()));
    connect(actionImprimer, SIGNAL(triggered()), this, SLOT(imprimer()));
    connect(actionMeilleursTemps, SIGNAL(triggered()), this, SLOT(meilleursTemps()));
    connect(actionQuitter, SIGNAL(triggered()), this, SLOT(quitter()));
    connect(actionPleinEcran, SIGNAL(triggered()), this, SLOT(pleinEcran()));
    connect(actionLabyrintheSeulement, SIGNAL(triggered()), this, SLOT(labyrintheSeulement()));
    connect(actionAfficherTrace, SIGNAL(triggered()), this, SLOT(afficherTrace()));
    connect(actionEffacerChemin, SIGNAL(triggered()), this, SLOT(effacerChemin()));
    connect(actionAffichage, SIGNAL(triggered()), this, SLOT(affichage()));
    connect(actionMusique, SIGNAL(triggered()), this, SLOT(musique()));
    connect(actionLangue, SIGNAL(triggered()), this, SLOT(langue()));
    connect(actionModes, SIGNAL(triggered()), this, SLOT(modes()));
    connect(actionButDuJeu, SIGNAL(triggered()), this, SLOT(butDuJeu()));
    connect(actionCommandes, SIGNAL(triggered()), this, SLOT(commandes()));
    connect(actionAPropos, SIGNAL(triggered()), this, SLOT(aPropos()));
    connect(actionAProposQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(chronometre, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    connect(labyrinth, SIGNAL(partieCommencee()), this, SLOT(commencer()));
    connect(labyrinth, SIGNAL(partieTerminee()), this, SLOT(arreter()));
    connect(labyrinth, SIGNAL(enregistrementChange()), this, SLOT(changerEnregistrement()));
    connect(labyrinth, SIGNAL(deplacementChange()), this, SLOT(actualiserDeplacement()));

    emplacementMusique = mus;
    mediaPlayer->setSource(emplacementMusique);

    audioOutput->setMuted(muet);

    if (QFileInfo(emplacementMusique).exists())
        mediaPlayer->play();

    if (ouvertureReussie)
    {
        chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(tr("hh:mm:ss"))));
        nouveau = false;

        chronometre->setDisabled(labyrinth->getPartieEnCours());
        chronometre->setChecked(chrono);
        actionResoudre->setDisabled(labyrinth->getEnResolution());
        actionMettreEnPause->setDisabled(labyrinth->getPartieTerminee());
        actionMettreEnPause->setChecked(labyrinth->getPartieEnPause());
        pauseImposee = labyrinth->getPartieEnPause();
        actionEffacerChemin->setDisabled(labyrinth->getPartieEnCours());
        actionEffacerChemin->setChecked(labyrinth->getEffacerChemin());
        actionResolutionProgressive->setChecked(labyrinth->getResolutionProgressive());
        actionAfficherTrace->setChecked(labyrinth->getAfficherTrace());
        actionLabyrintheSeulement->setChecked(labySeulement);
        if (labyrinth->getTypeLabyrinthe() == QLabyrinth::Labyrinthe2D)
            actionLabyrinthe2D->setChecked(true);
        else// if (labyrinth->getTypeLabyrinthe() == QLabyrinth::Labyrinthe2Den3D)
            actionLabyrinthe2Den3D->setChecked(true);
        if (int(labyrinth->getAlgorithme()) < actionsAlgorithmesGeneration.size())
            actionsAlgorithmesGeneration[int(labyrinth->getAlgorithme())]->setChecked(true);
        if (labySeulement)
            labyrintheSeulement();

        if (chronometre->isChecked() && labyrinth->getPartieEnCours() && !pauseImposee)
        {
            timer->start();
            elapsedTimer.start();
        }

        nouvellePartie();

        QString n;

        if (labyrinth->getNiveau() == QLabyrinth::Facile)
        {
            n = tr("Facile");
            actionFacile->setChecked(true);
        }
        else if (labyrinth->getNiveau() == QLabyrinth::Moyen)
        {
            n = tr("Moyen");
            actionMoyen->setChecked(true);
        }
        else if (labyrinth->getNiveau() == QLabyrinth::Difficile)
        {
            n = tr("Difficile");
            actionDifficile->setChecked(true);
        }
        else
        {
            n = tr("Personnalisé");
            actionPersonnalise->setChecked(true);
        }

        niveau->setText(tr("Niveau : %1 (%2x%3)").arg(n).arg(labyrinth->getLongueur()).arg(labyrinth->getLargeur()));

        labyrinth->activer();
    }
    else
    {
        labyrinth->nouveau(QLabyrinth::Facile, 10001, 10001, labyrinth->getAlgorithme(), QLabyrinth::Labyrinthe2D, QLabyrinth::Rectangle, 0);

        if (labyrinth->getLongueur() * labyrinth->getLargeur() > LONGUEURDIFFICILE * LARGEURDIFFICILE)
        {
            actionResolutionProgressive->setChecked(false);
            labyrinth->setResolutionProgressive(false);
        }
        else
        {
            actionResolutionProgressive->setChecked(true);
            labyrinth->setResolutionProgressive(true);
        }

        QString n = tr("Personnalisé");

        if (labyrinth->getNiveau() == QLabyrinth::Facile)
        {
            n = tr("Facile");
            actionFacile->setChecked(true);
        }
        else if (labyrinth->getNiveau() == QLabyrinth::Moyen)
        {
            n = tr("Moyen");
            actionMoyen->setChecked(true);
        }
        else if (labyrinth->getNiveau() == QLabyrinth::Difficile)
        {
            n = tr("Difficile");
            actionDifficile->setChecked(true);
        }
        else
            actionPersonnalise->setChecked(true);

        niveau->setText(tr("Niveau : %1 (%2x%3)").arg(n).arg(labyrinth->getLongueur()).arg(labyrinth->getLargeur()));
        actionAfficherTrace->setChecked(labyrinth->getAfficherTrace());
        actionEffacerChemin->setChecked(labyrinth->getEffacerChemin());

        if (QFileInfo(emplacementMusique).exists())
            mediaPlayer->play();

        actionLabyrinthe2D->setChecked(true);
        actionsAlgorithmesGeneration.first()->setChecked(true);
    }

    deplacement->setText(tr("Déplacement : ") + QString::number(labyrinth->getNombreDeplacement()));

    QPalette p = scrollArea->palette();

    if (labyrinth->getModeLabyrinthe().mode & QLabyrinth::Obscurite)
        p.setColor(QPalette::Window, labyrinth->getModeLabyrinthe().couleurObscurite);
    else
        p.setColor(QPalette::Window, labyrinth->getTextureFond().couleur);

    scrollArea->setPalette(p);

    labyrinth->activer();
}

MainWindow::~MainWindow()
{
}

void MainWindow::commencer()
{
    chronometre->setDisabled(true);
    actionEffacerChemin->setDisabled(true);
    actionRecommencer->setDisabled(false);

    if (chronometre->isChecked())
    {
        timer->start();
        elapsedTimer.start();
    }
}

void MainWindow::actualiserChronometre()
{
    chronometre->setText(tr("&Chronomètre : %1").arg(QTime(0, 0).addMSecs(elapsedTimer.elapsed() + ms)
                                                                .toString(tr("hh:mm:ss"))));
}

void MainWindow::nouvellePartie()
{
    if (!labyrinth->getEnregistre() && labyrinth->getPartieEnCours() && !labyrinth->getPartieTerminee())
    {
        if (!pauseImposee)
            mettreEnPause();

        if (QMessageBox::warning(this, tr("Partie non enregistrée"),
                                 tr("La partie en cours n'est pas enregistrée.\n"
                                    "Voulez-vous vraiment faire une nouvelle partie ?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            if (!pauseImposee)
                mettreEnPause();
            return;
        }
    }

    temps = QTime(0, 0, 0);
    ms = 0;
    timer->stop();
    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(QString("hh:mm:ss"))));
    deplacement->setText(tr("Déplacement : %1").arg(0));
    nouveau = true;
    pauseImposee = false;
    nomPartie = tr("Partie");
    setWindowTitle(((labyrinth->getEnregistre()) ? QString() : QString("*")) + tr("%1 - Labyrinthe").arg(nomPartie));

    chronometre->setDisabled(false);

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(true);
    actionResoudre->setDisabled(true);
    actionMettreEnPause->setDisabled(true);
    actionEffacerChemin->setDisabled(false);

    labyrinth->nouveau(labyrinth->getNiveau(), labyrinth->getLongueur(), labyrinth->getLargeur(), labyrinth->getAlgorithme(), labyrinth->getTypeLabyrinthe(), labyrinth->getFormeLabyrinthe(), 0);

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(false);
    actionResoudre->setDisabled(false);
    actionMettreEnPause->setDisabled(false);
}

void MainWindow::recommencer()
{
    if (!labyrinth->getEnregistre() && labyrinth->getPartieEnCours() && !labyrinth->getPartieTerminee())
    {
        if (!pauseImposee)
            mettreEnPause();

        if (QMessageBox::warning(this, tr("Partie non enregistrée"),
                                 tr("La partie en cours n'est pas enregistrée.\n"
                                    "Voulez-vous vraiment recommencer cette partie ?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            if (!pauseImposee)
                mettreEnPause();
            return;
        }
    }

    temps = QTime(0, 0, 0);
    ms = 0;
    timer->stop();
    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(QString("hh:mm:ss"))));
    deplacement->setText(tr("Déplacement : %1").arg(0));
    pauseImposee = false;

    chronometre->setDisabled(false);

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(false);
    actionResoudre->setDisabled(false);
    actionMettreEnPause->setDisabled(false);
    actionEffacerChemin->setDisabled(false);

    labyrinth->recommencer();
}

void MainWindow::arreter()
{
    if (chronometre->isChecked())
    {
        temps = QTime(0, 0).addMSecs(elapsedTimer.elapsed()+ms);
        timer->stop();
    }

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(true);
    actionResoudre->setDisabled(true);
    actionMettreEnPause->setDisabled(true);
    pauseImposee = false;

    int indexScore = -1;

    QList<QList<Score> > *scores = &scores2D;

    if (labyrinth->getTypeLabyrinthe() == QLabyrinth::Labyrinthe2Den3D)
        scores = &scores2Den3D;

    if (chronometre->isChecked() && labyrinth->getNiveau() != QLabyrinth::Personnalise
        && labyrinth->getModeLabyrinthe().mode == QLabyrinth::Aucun)
    {
        for (int i = 0; i < (*scores)[int(labyrinth->getNiveau())].size(); i++)
        {
            if (temps < (*scores)[int(labyrinth->getNiveau())][i].temps)
            {
                indexScore = i;
                break;
            }
        }
        if (indexScore == -1)
            indexScore = (*scores)[int(labyrinth->getNiveau())].size();
    }

    if (indexScore != -1)
    {
        QString nom = QInputDialog::getText(this, tr("Nouveau score"),
                                            tr("Félicitations ! Vous avez terminé le labyrinthe en un temps record !\n"
                                               "Veuillez entrer votre nom :"),
                                            QLineEdit::Normal, dernierNomEnregistre);

        if (!nom.isEmpty())
        {
            Score score;
            score.nom = nom;
            score.temps = temps;
            score.deplacement = labyrinth->getNombreDeplacement();
            (*scores)[int(labyrinth->getNiveau())].insert(indexScore, score);
            if ((*scores)[int(labyrinth->getNiveau())].size() > NOMBRESCORES)
                (*scores)[int(labyrinth->getNiveau())].removeLast();
            dernierNomEnregistre = nom;
            meilleursTemps();
        }
    }
    else
        QMessageBox::information(this, tr("Partie terminée"), tr("Félicitations ! Vous avez terminé le labyrinthe !"),
                                 QMessageBox::Ok);
}

void MainWindow::mettreEnPause()
{
    labyrinth->mettreEnPauseRedemarrer();

    if (chronometre->isChecked() && labyrinth->getPartieEnCours() && !labyrinth->getPartieTerminee())
    {
        if (labyrinth->getPartieEnPause())
        {
            ms += elapsedTimer.elapsed();
            temps = QTime(0, 0).addMSecs(elapsedTimer.elapsed());
            timer->stop();
        }
        else
        {
            timer->start();
            elapsedTimer.start();
        }
    }
}

void MainWindow::resoudre()
{
    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(true);
    actionResoudre->setDisabled(true);
    actionMettreEnPause->setDisabled(true);
    chronometre->setDisabled(true);
    actionEffacerChemin->setDisabled(true);

    temps = QTime(0, 0).addMSecs(elapsedTimer.elapsed());
    timer->stop();

    labyrinth->resoudre();
}

void MainWindow::resolutionProgressive()
{
    labyrinth->setResolutionProgressive(actionResolutionProgressive->isChecked());
}

void MainWindow::ouvrir()
{
    if (!labyrinth->getEnregistre() && labyrinth->getPartieEnCours() && !labyrinth->getPartieTerminee())
    {
        if (!pauseImposee)
            mettreEnPause();

        if (QMessageBox::warning(this, tr("Partie non enregistrée"),
                                 tr("La partie en cours n'est pas enregistrée.\n"
                                    "Voulez-vous vraiment charger une autre partie ?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            if (!pauseImposee)
                mettreEnPause();
            return;
        }
    }

    QString s = fichierEnregistrement;
    if (!QFileInfo(s).exists())
        s = QApplication::applicationDirPath();
    QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir une partie"), s, tr("Labyrinthes (*.laby)"));

    if (QFileInfo(fichier).exists())
        chargerPartie(fichier);
}

void MainWindow::enregistrer()
{
    if (!pauseImposee)
        mettreEnPause();

    if (fichierEnregistrement.isEmpty() || nouveau)
    {
        fichierEnregistrement = nomPartie+tr(".laby");
        fichierEnregistrement = QFileDialog::getSaveFileName(this, tr("Enregistrer la partie"),
                                                             fichierEnregistrement, tr("Labyrinthes (*.laby)"));
        if (!fichierEnregistrement.isEmpty())
        {
            if (fichierEnregistrement.right(5) != tr(".laby"))
                fichierEnregistrement += tr(".laby");
            nouveau = false;
        }
    }

    if (!fichierEnregistrement.isEmpty())
    {
        QFile f(fichierEnregistrement);

        if (f.open(QFile::WriteOnly | QFile::Truncate))
        {
            nomPartie = QFileInfo(fichierEnregistrement).baseName();
            QDataStream data(&f);
            data.setVersion(QDataStream::Qt_4_7);

            //data << labyrinthe 2D (ou 2D en 3D) ou 3D;

            labyrinth->enregistrer(data, chronometre->isChecked(), ms, pauseImposee,
                                   emplacementMusique, audioOutput->isMuted(),
                                   actionLabyrintheSeulement->isChecked(), adaptationEcran, adaptationFormats);

            f.close();
        }
        else
            QMessageBox::warning(this, tr("Impossible d'enregistrer la partie"),
                                 tr("Il est impossible d'ouvrir le fichier pour enregistrer la partie."),
                                 QMessageBox::Ok);
    }

    if (!pauseImposee)
        mettreEnPause();
}

void MainWindow::enregistrerSous()
{
    if (!pauseImposee)
        mettreEnPause();

    QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer la partie sous ..."),
                                                   fichierEnregistrement, tr("Labyrinthes (*.laby)"));

    if (!fichier.isEmpty())
    {
        fichierEnregistrement = fichier;
        if (fichierEnregistrement.right(5) != tr(".laby"))
            fichierEnregistrement += tr(".laby");

        QFile f(fichierEnregistrement);

        if (f.open(QFile::WriteOnly | QFile::Truncate))
        {
            nomPartie = QFileInfo(fichierEnregistrement).baseName();
            QDataStream data(&f);
            data.setVersion(QDataStream::Qt_4_7);

            labyrinth->enregistrer(data, chronometre->isChecked(), ms, pauseImposee,
                                   emplacementMusique, audioOutput->isMuted(),
                                   actionLabyrintheSeulement->isChecked(), adaptationEcran, adaptationFormats);

            f.close();
        }
        else
            QMessageBox::warning(this, tr("Impossible d'enregistrer la partie"),
                                 tr("Il est impossible d'ouvrir le fichier pour enregistrer la partie."),
                                 QMessageBox::Ok);
    }

    if (!pauseImposee)
        mettreEnPause();
}

void MainWindow::facile()
{
    if (labyrinth->getNiveau() == QLabyrinth::Facile)
        return;

    if (!labyrinth->getEnregistre() && labyrinth->getPartieEnCours() && !labyrinth->getPartieTerminee())
    {
        if (!pauseImposee)
            mettreEnPause();

        if (QMessageBox::warning(this, tr("Partie non enregistrée"),
                                 tr("La partie en cours n'est pas enregistrée.\n"
                                    "Voulez-vous vraiment faire une nouvelle partie ?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            if (!pauseImposee)
                mettreEnPause();
            return;
        }
    }

    temps = QTime(0, 0, 0);
    ms = 0;
    timer->stop();
    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(QString("hh:mm:ss"))));
    deplacement->setText(tr("Déplacement : %1").arg(0));
    nouveau = true;
    pauseImposee = false;
    nomPartie = tr("Partie");
    setWindowTitle(((labyrinth->getEnregistre()) ? QString() : QString("*")) + tr("%1 - Labyrinthe").arg(nomPartie));

    chronometre->setDisabled(false);
    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(true);
    actionResoudre->setDisabled(true);
    actionMettreEnPause->setDisabled(true);
    actionEffacerChemin->setDisabled(false);

    labyrinth->nouveau(QLabyrinth::Facile, 0, 0, labyrinth->getAlgorithme(),
                       labyrinth->getTypeLabyrinthe(), labyrinth->getFormeLabyrinthe(), 0);

    niveau->setText(tr("Niveau : Facile (%1x%2)").arg(labyrinth->getLongueur()).arg(labyrinth->getLargeur()));

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(false);
    actionResoudre->setDisabled(false);
    actionMettreEnPause->setDisabled(false);
    adaptationEcran = false;
    adaptationFormats = false;
}

void MainWindow::moyen()
{
    if (labyrinth->getNiveau() == QLabyrinth::Moyen)
        return;

    if (!labyrinth->getEnregistre() && labyrinth->getPartieEnCours() && !labyrinth->getPartieTerminee())
    {
        if (!pauseImposee)
            mettreEnPause();

        if (QMessageBox::warning(this, tr("Partie non enregistrée"),
                                 tr("La partie en cours n'est pas enregistrée.\n"
                                    "Voulez-vous vraiment faire une nouvelle partie ?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            if (!pauseImposee)
                mettreEnPause();
            return;
        }
    }

    temps = QTime(0, 0, 0);
    ms = 0;
    timer->stop();
    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(QString("hh:mm:ss"))));
    deplacement->setText(tr("Déplacement : %1").arg(0));
    nouveau = true;
    pauseImposee = false;
    nomPartie = tr("Partie");
    setWindowTitle(((labyrinth->getEnregistre()) ? QString() : QString("*")) + tr("%1 - Labyrinthe").arg(nomPartie));

    chronometre->setDisabled(false);
    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(true);
    actionResoudre->setDisabled(true);
    actionMettreEnPause->setDisabled(true);
    actionEffacerChemin->setDisabled(false);

    labyrinth->nouveau(QLabyrinth::Moyen, 0, 0, labyrinth->getAlgorithme(),
                       labyrinth->getTypeLabyrinthe(), labyrinth->getFormeLabyrinthe(), 0);

    niveau->setText(tr("Niveau : Moyen (%1x%2)").arg(labyrinth->getLongueur()).arg(labyrinth->getLargeur()));

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(false);
    actionResoudre->setDisabled(false);
    actionMettreEnPause->setDisabled(false);
    adaptationEcran = false;
    adaptationFormats = false;
}

void MainWindow::difficile()
{
    if (labyrinth->getNiveau() == QLabyrinth::Difficile)
        return;

    if (!labyrinth->getEnregistre() && labyrinth->getPartieEnCours() && !labyrinth->getPartieTerminee())
    {
        if (!pauseImposee)
            mettreEnPause();

        if (QMessageBox::warning(this, tr("Partie non enregistrée"),
                                 tr("La partie en cours n'est pas enregistrée.\n"
                                    "Voulez-vous vraiment faire une nouvelle partie ?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            if (!pauseImposee)
                mettreEnPause();
            return;
        }
    }

    temps = QTime(0, 0, 0);
    ms = 0;
    timer->stop();
    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(QString("hh:mm:ss"))));
    deplacement->setText(tr("Déplacement : %1").arg(0));
    nouveau = true;
    pauseImposee = false;
    nomPartie = tr("Partie");
    setWindowTitle(((labyrinth->getEnregistre()) ? QString() : QString("*")) + tr("%1 - Labyrinthe").arg(nomPartie));

    chronometre->setDisabled(false);
    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(true);
    actionResoudre->setDisabled(true);
    actionMettreEnPause->setDisabled(true);
    actionEffacerChemin->setDisabled(false);

    labyrinth->nouveau(QLabyrinth::Difficile, 0, 0, labyrinth->getAlgorithme(),
                       labyrinth->getTypeLabyrinthe(), labyrinth->getFormeLabyrinthe(), 0);

    niveau->setText(tr("Niveau : Difficile (%1x%2)").arg(labyrinth->getLongueur()).arg(labyrinth->getLargeur()));

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(false);
    actionResoudre->setDisabled(false);
    actionMettreEnPause->setDisabled(false);
    adaptationEcran = false;
    adaptationFormats = false;
}

void MainWindow::personnalise()
{
    if (!pauseImposee)
        mettreEnPause();

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Personnalisé"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), dialog, SLOT(reject()));
    spinBoxLargeur = new QSpinBox(dialog);
    spinBoxLargeur->setSingleStep(2);
    spinBoxLargeur->setRange(2, 5000);
    spinBoxLargeur->setValue(labyrinth->getLargeur());
    spinBoxLongueur = new QSpinBox(dialog);
    spinBoxLongueur->setSingleStep(2);
    spinBoxLongueur->setRange(2, 5000);
    spinBoxLongueur->setValue(labyrinth->getLongueur());

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(tr("Longueur :"), spinBoxLongueur);
    formLayout->addRow(tr("Largeur :"), spinBoxLargeur);
    groupBoxTailleLabyrinthe = new QGroupBox(tr("Taille du labyrinthe"), dialog);
    groupBoxTailleLabyrinthe->setLayout(formLayout);
    checkBoxAdaptation = new QCheckBox(tr("Taille adaptée à l'écran"), dialog);
    checkBoxFormats = new QCheckBox(tr("Taille du format du papier"), dialog);
    boutonConfigurerFormats = new QPushButton(tr("Configurer"), dialog);
    QHBoxLayout *layoutFormats = new QHBoxLayout;
    layoutFormats->addWidget(checkBoxFormats);
    layoutFormats->addWidget(boutonConfigurerFormats);
    QFrame *frame = new QFrame(dialog);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(groupBoxTailleLabyrinthe);
    vBoxLayout->addWidget(checkBoxAdaptation);
    vBoxLayout->addLayout(layoutFormats);
    vBoxLayout->addWidget(frame);
    vBoxLayout->addWidget(buttonBox);

    if (adaptationEcran)
    {
        groupBoxTailleLabyrinthe->setDisabled(true);
        checkBoxAdaptation->setChecked(true);
        checkBoxFormats->setChecked(false);
        boutonConfigurerFormats->setDisabled(true);
        stateChangedAdapation(Qt::Checked);
    }
    else if (adaptationFormats)
    {
        groupBoxTailleLabyrinthe->setDisabled(true);
        checkBoxAdaptation->setChecked(false);
        checkBoxFormats->setChecked(true);
        boutonConfigurerFormats->setDisabled(false);
        stateChangedFormats(Qt::Checked);
    }
    else
    {
        groupBoxTailleLabyrinthe->setDisabled(false);
        checkBoxAdaptation->setChecked(false);
        boutonConfigurerFormats->setDisabled(false);
    }

    connect(checkBoxAdaptation, SIGNAL(stateChanged(int)), this, SLOT(stateChangedAdapation(int)));
    connect(checkBoxFormats, SIGNAL(stateChanged(int)), this, SLOT(stateChangedFormats(int)));
    connect(boutonConfigurerFormats, SIGNAL(clicked()), this, SLOT(configurerFormats()));

    dialog->setLayout(vBoxLayout);

    if (dialog->exec() == QDialog::Rejected)
    {
        if (labyrinth->getNiveau() == QLabyrinth::Facile)
            actionFacile->setChecked(true);
        else if (labyrinth->getNiveau() == QLabyrinth::Moyen)
            actionMoyen->setChecked(true);
        else if (labyrinth->getNiveau() == QLabyrinth::Difficile)
            actionDifficile->setChecked(true);

        dialog->deleteLater();

        if (!pauseImposee)
            mettreEnPause();
        return;
    }

    if (!labyrinth->getEnregistre() && labyrinth->getPartieEnCours() && !labyrinth->getPartieTerminee())
    {
        if (QMessageBox::warning(this, tr("Partie non enregistrée"),
                                 tr("La partie en cours n'est pas enregistrée.\n"
                                    "Voulez-vous vraiment faire une nouvelle partie ?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            if (!pauseImposee)
                mettreEnPause();
            return;
        }
    }

    temps = QTime(0, 0, 0);
    ms = 0;
    timer->stop();
    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(QString("hh:mm:ss"))));
    deplacement->setText(tr("Déplacement : %1").arg(0));
    nouveau = true;
    pauseImposee = false;
    nomPartie = tr("Partie");
    setWindowTitle(((labyrinth->getEnregistre()) ? QString() : QString("*")) + tr("%1 - Labyrinthe").arg(nomPartie));

    chronometre->setDisabled(false);
    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(true);
    actionResoudre->setDisabled(true);
    actionMettreEnPause->setDisabled(true);
    actionEffacerChemin->setDisabled(false);

    labyrinth->nouveau(QLabyrinth::Personnalise, spinBoxLongueur->value(),
                       spinBoxLargeur->value(), labyrinth->getAlgorithme(),
                       labyrinth->getTypeLabyrinthe(), labyrinth->getFormeLabyrinthe(), 0);

    QString n = tr("Personnalisé");

    if (labyrinth->getNiveau() == QLabyrinth::Facile)
    {
        n = tr("Facile");
        actionFacile->setChecked(true);
    }
    else if (labyrinth->getNiveau() == QLabyrinth::Moyen)
    {
        n = tr("Moyen");
        actionMoyen->setChecked(true);
    }
    else if (labyrinth->getNiveau() == QLabyrinth::Difficile)
    {
        n = tr("Difficile");
        actionDifficile->setChecked(true);
    }

    dialog->deleteLater();

    niveau->setText(tr("Niveau : %1 (%2x%3)").arg(n).arg(labyrinth->getLongueur()).arg(labyrinth->getLargeur()));

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(false);
    actionResoudre->setDisabled(false);
    actionMettreEnPause->setDisabled(false);

    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(tr("hh:mm:ss"))));

    if (labyrinth->getLongueur() * labyrinth->getLargeur() > LONGUEURDIFFICILE * LARGEURDIFFICILE)
    {
        labyrinth->setResolutionProgressive(false);
        actionResolutionProgressive->setChecked(false);
    }

    labyrinth->activer();
}

void MainWindow::apercu()
{
    if (!pauseImposee)
        mettreEnPause();

    QPrintPreviewDialog printPreview(printer, this);

    connect(&printPreview, SIGNAL(paintRequested(QPrinter *)), this, SLOT(apercuLabyrinthe(QPrinter *)));

    printPreview.exec();

    if (!pauseImposee)
        mettreEnPause();
}

void MainWindow::imprimer()
{
    if (!pauseImposee)
        mettreEnPause();

    QPrintDialog printDialog(printer, this);

    connect(&printDialog, SIGNAL(accepted(QPrinter *)), this, SLOT(apercuLabyrinthe(QPrinter *)));

    printDialog.exec();

    if (!pauseImposee)
        mettreEnPause();
}

void MainWindow::meilleursTemps()
{
    if (!pauseImposee)
        mettreEnPause();

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Meilleurs temps"));

    tabWidgetScores = new QTabWidget(dialog);

    radioButtonScores2D = new QRadioButton(tr("Scores 2D"), dialog);
    QRadioButton *radioButtonScores2Den3D = new QRadioButton(tr("Scores 2D en 3D"), dialog);

    QButtonGroup *buttonGroup = new QButtonGroup(dialog);
    buttonGroup->addButton(radioButtonScores2D);
    buttonGroup->addButton(radioButtonScores2Den3D);

    if (labyrinth->getTypeLabyrinthe() == QLabyrinth::Labyrinthe2D)
        radioButtonScores2D->setChecked(true);
    else
        radioButtonScores2Den3D->setChecked(true);

    connect(radioButtonScores2D, SIGNAL(clicked()), this, SLOT(changementCategorieScores()));
    connect(radioButtonScores2Den3D, SIGNAL(clicked()), this, SLOT(changementCategorieScores()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Reset, Qt::Horizontal, dialog);
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reinitialiserScores()));

    QWidget *tabFacile = new QWidget(dialog);
    QGridLayout *gridLayoutFacile = new QGridLayout;
    tabFacile->setLayout(gridLayoutFacile);
    QLabel *labelNomFacile = new QLabel(tr("<b>Nom</b>"), tabFacile);
    QLabel *labelTempsFacile = new QLabel(tr("<b>Temps</b>"), tabFacile);
    QLabel *labelDeplacementFacile = new QLabel(tr("<b>Déplacement</b>"), tabFacile);
    gridLayoutFacile->addWidget(labelNomFacile, 0, 0);
    gridLayoutFacile->addWidget(labelTempsFacile, 0, 1);
    gridLayoutFacile->addWidget(labelDeplacementFacile, 0, 2);
    labelsNomsFacile.clear();
    labelsTempsFacile.clear();
    labelsDeplacementsFacile.clear();
    QList<QList<Score> > *scores = &scores2D;
    if (!radioButtonScores2D->isChecked())
        scores = &scores2Den3D;
    for (int i = 0; i < NOMBRESCORES; i++)
    {
        labelsNomsFacile << new QLabel(tr("%1. ").arg(i + 1), tabFacile);
        labelsTempsFacile << new QLabel(tr(""), tabFacile);
        labelsDeplacementsFacile << new QLabel(tr(""), tabFacile);
        if (i < (*scores)[0].size())
        {
            labelsNomsFacile.last()->setText(labelsNomsFacile.last()->text() + (*scores)[0][i].nom);
            labelsTempsFacile.last()->setText((*scores)[0][i].temps.toString(tr("hh:mm:ss")));
            labelsDeplacementsFacile.last()->setText(QString::number((*scores)[0][i].deplacement));
        }
        gridLayoutFacile->addWidget(labelsNomsFacile.last(), i + 1, 0);
        gridLayoutFacile->addWidget(labelsTempsFacile.last(), i + 1, 1);
        gridLayoutFacile->addWidget(labelsDeplacementsFacile.last(), i + 1, 2);
    }
    tabWidgetScores->addTab(tabFacile, tr("&Facile"));

    QWidget *tabMoyen = new QWidget(dialog);
    QGridLayout *gridLayoutMoyen = new QGridLayout;
    tabMoyen->setLayout(gridLayoutMoyen);
    QLabel *labelNomMoyen = new QLabel(tr("<b>Nom</b>"), tabMoyen);
    QLabel *labelTempsMoyen = new QLabel(tr("<b>Temps</b>"), tabMoyen);
    QLabel *labelDeplacementMoyen = new QLabel(tr("<b>Déplacement</b>"), tabMoyen);
    gridLayoutMoyen->addWidget(labelNomMoyen, 0, 0, 1, 1);
    gridLayoutMoyen->addWidget(labelTempsMoyen, 0, 1, 1, 1);
    gridLayoutMoyen->addWidget(labelDeplacementMoyen, 0, 2, 1, 1);
    labelsNomsMoyen.clear();
    labelsTempsMoyen.clear();
    labelsDeplacementsMoyen.clear();
    for (int i = 0; i < NOMBRESCORES; i++)
    {
        labelsNomsMoyen << new QLabel(tr("%1. ").arg(i + 1), tabMoyen);
        labelsTempsMoyen << new QLabel(tr(""), tabMoyen);
        labelsDeplacementsMoyen << new QLabel(tr(""), tabMoyen);
        if (i < (*scores)[1].size())
        {
            labelsNomsMoyen.last()->setText(labelsNomsMoyen.last()->text() + (*scores)[1][i].nom);
            labelsTempsMoyen.last()->setText((*scores)[1][i].temps.toString(tr("hh:mm:ss")));
            labelsDeplacementsMoyen.last()->setText(QString::number((*scores)[1][i].deplacement));
        }
        gridLayoutMoyen->addWidget(labelsNomsMoyen.last(), i + 1, 0, 1, 1);
        gridLayoutMoyen->addWidget(labelsTempsMoyen.last(), i + 1, 1, 1, 1);
        gridLayoutFacile->addWidget(labelsDeplacementsMoyen.last(), i + 1, 2, 1, 1);
    }
    tabWidgetScores->addTab(tabMoyen, tr("&Moyen"));

    QWidget *tabDifficile = new QWidget(dialog);
    QGridLayout *gridLayoutDifficile = new QGridLayout;
    tabDifficile->setLayout(gridLayoutDifficile);
    QLabel *labelNomDifficile = new QLabel(tr("<b>Nom</b>"), tabDifficile);
    QLabel *labelTempsDifficile = new QLabel(tr("<b>Temps</b>"), tabDifficile);
    QLabel *labelDeplacementDifficile = new QLabel(tr("<b>Déplacement</b>"), tabDifficile);
    gridLayoutDifficile->addWidget(labelNomDifficile, 0, 0, 1, 1);
    gridLayoutDifficile->addWidget(labelTempsDifficile, 0, 1, 1, 1);
    gridLayoutDifficile->addWidget(labelDeplacementDifficile, 0, 2, 1, 1);
    labelsNomsDifficile.clear();
    labelsTempsDifficile.clear();
    for (int i = 0; i < NOMBRESCORES; i++)
    {
        labelsNomsDifficile << new QLabel(tr("%1. ").arg(i + 1), tabDifficile);
        labelsTempsDifficile << new QLabel(tr(""), tabDifficile);
        labelsDeplacementsDifficile << new QLabel(tr(""), tabDifficile);
        if (i < (*scores)[2].size())
        {
            labelsNomsDifficile.last()->setText(labelsNomsDifficile.last()->text() + (*scores)[2][i].nom);
            labelsTempsDifficile.last()->setText((*scores)[2][i].temps.toString(tr("hh:mm:ss")));
            labelsDeplacementsDifficile.last()->setText(QString::number((*scores)[2][i].deplacement));
        }
        gridLayoutDifficile->addWidget(labelsNomsDifficile.last(), i + 1, 0, 1, 1);
        gridLayoutDifficile->addWidget(labelsTempsDifficile.last(), i + 1, 1, 1, 1);
        gridLayoutFacile->addWidget(labelsDeplacementsDifficile.last(), i + 1, 2, 1, 1);
    }
    tabWidgetScores->addTab(tabDifficile, tr("&Difficile"));
    QFrame *frame = new QFrame(dialog);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(radioButtonScores2D, 0, 0);
    gridLayout->addWidget(radioButtonScores2Den3D, 0, 1);
    gridLayout->setColumnStretch(2, 1);
    gridLayout->addWidget(tabWidgetScores, 1, 0, 1, 3);
    gridLayout->addWidget(frame, 2, 0, 1, 3);
    gridLayout->addWidget(buttonBox, 3, 0, 1, 3);
    dialog->setLayout(gridLayout);

    dialog->exec();

    dialog->deleteLater();

    if (!pauseImposee)
        mettreEnPause();

    labyrinth->activer();
}

void MainWindow::quitter()
{
    if (!labyrinth->getEnregistre() && labyrinth->getPartieEnCours() && !labyrinth->getPartieTerminee())
    {
        if (!pauseImposee)
            mettreEnPause();

        if (QMessageBox::warning(this, tr("Partie non enregistrée"),
                                 tr("La partie en cours n'est pas enregistrée.\n"
                                    "Voulez-vous vraiment quitter le jeu ?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            if (!pauseImposee)
                mettreEnPause();
            return;
        }
    }

    QFile f("Labyrinth.ini");

    if (f.open(QFile::WriteOnly))// | QFile::Truncate
    {
        QDataStream data(&f);
        data.setVersion(QDataStream::Qt_4_7);

        labyrinth->enregistrer(data, chronometre->isChecked(), ms, pauseImposee,
                               emplacementMusique, audioOutput->isMuted(),
                               actionLabyrintheSeulement->isChecked(), adaptationEcran, adaptationFormats, false);

        for (int i = 0; i < 3; i++)
        {
            data << scores2D[i].size();
            for (int j = 0 ; j < scores2D[i].size(); j++)
            {
                data << scores2D[i][j].nom;
                data << scores2D[i][j].temps;
                data << scores2D[i][j].deplacement;
            }
        }
        for (int i = 0; i < 3; i++)
        {
            data << scores2Den3D[i].size();
            for (int j = 0 ; j < scores2Den3D[i].size(); j++)
            {
                data << scores2Den3D[i][j].nom;
                data << scores2Den3D[i][j].temps;
                data << scores2Den3D[i][j].deplacement;
            }
        }
        data << int(langueChoisie);
        data << dernierNomEnregistre;

        f.close();
    }

    labyrinth->arreterResolution();

    qApp->quit();
}

void MainWindow::affichage()
{
    if (!pauseImposee)
        mettreEnPause();

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Affichage"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset, Qt::Horizontal, dialog);
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), dialog, SLOT(reject()));
    connect(buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reinitialiserAffichage()));

    QGroupBox *groupBoxTaille = new QGroupBox(tr("Taille des cases"), dialog);
    spinBoxLongueurChemins = new QSpinBox(groupBoxTaille);
    spinBoxLongueurChemins->setRange(1, 100);
    spinBoxLongueurChemins->setValue(labyrinth->waysSize().width());
    QPushButton *boutonReinitialiserLongueurChemins = new QPushButton(tr("Réinitialiser"), groupBoxTaille);
    connect(boutonReinitialiserLongueurChemins, SIGNAL(clicked()), this, SLOT(reinitialiserLongueurChemins()));
    QHBoxLayout *hBoxLayoutLongueurChemins = new QHBoxLayout;
    hBoxLayoutLongueurChemins->addWidget(spinBoxLongueurChemins);
    hBoxLayoutLongueurChemins->addWidget(boutonReinitialiserLongueurChemins);
    spinBoxLargeurChemins = new QSpinBox(groupBoxTaille);
    spinBoxLargeurChemins->setRange(1, 100);
    spinBoxLargeurChemins->setValue(labyrinth->waysSize().height());
    QPushButton *boutonReinitialiserLargeurChemins = new QPushButton(tr("Réinitialiser"), groupBoxTaille);
    connect(boutonReinitialiserLargeurChemins, SIGNAL(clicked()), this, SLOT(reinitialiserLargeurChemins()));
    QHBoxLayout *hBoxLayoutLargeurChemins = new QHBoxLayout;
    hBoxLayoutLargeurChemins->addWidget(spinBoxLargeurChemins);
    hBoxLayoutLargeurChemins->addWidget(boutonReinitialiserLargeurChemins);

    spinBoxLongueurMurs = new QSpinBox(groupBoxTaille);
    spinBoxLongueurMurs->setRange(1, 100);
    spinBoxLongueurMurs->setValue(labyrinth->wallsSize().width());
    QPushButton *boutonReinitialiserLongueurMurs = new QPushButton(tr("Réinitialiser"), groupBoxTaille);
    connect(boutonReinitialiserLongueurMurs, SIGNAL(clicked()), this, SLOT(reinitialiserLongueurMurs()));
    QHBoxLayout *hBoxLayoutLongueurMurs = new QHBoxLayout;
    hBoxLayoutLongueurMurs->addWidget(spinBoxLongueurMurs);
    hBoxLayoutLongueurMurs->addWidget(boutonReinitialiserLongueurMurs);
    spinBoxLargeurMurs = new QSpinBox(groupBoxTaille);
    spinBoxLargeurMurs->setRange(1, 100);
    spinBoxLargeurMurs->setValue(labyrinth->wallsSize().height());
    QPushButton *boutonReinitialiserLargeurMurs = new QPushButton(tr("Réinitialiser"), groupBoxTaille);
    connect(boutonReinitialiserLargeurMurs, SIGNAL(clicked()), this, SLOT(reinitialiserLargeurMurs()));
    QHBoxLayout *hBoxLayoutLargeurMurs = new QHBoxLayout;
    hBoxLayoutLargeurMurs->addWidget(spinBoxLargeurMurs);
    hBoxLayoutLargeurMurs->addWidget(boutonReinitialiserLargeurMurs);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(tr("Longueur des chemins :"), hBoxLayoutLongueurChemins);
    formLayout->addRow(tr("Largeur des chemins:"), hBoxLayoutLargeurChemins);
    formLayout->addRow(tr("Longueur des murs :"), hBoxLayoutLongueurMurs);
    formLayout->addRow(tr("Largeur des murs :"), hBoxLayoutLargeurMurs);
    groupBoxTaille->setLayout(formLayout);

    QGroupBox *groupBoxFond = new QGroupBox(tr("Fond"), dialog);
    couleurFond = new QRadioButton(tr("Couleur :"), groupBoxFond);
    boutonCouleurFond = new QPushButton(tr("Choisir la couleur"), groupBoxFond);
    connect(boutonCouleurFond, SIGNAL(clicked()), this, SLOT(choisirCouleurFond()));
    QPixmap pixmapFond(32, 32);
    pixmapFond.fill(labyrinth->getTextureFond().couleur);
    boutonCouleurFond->setIcon(QIcon(pixmapFond));
    boutonCouleurFond->setProperty("Couleur", QString::number(labyrinth->getTextureFond().couleur.rgba()));
    QPushButton *boutonReinitialiserCouleurFond = new QPushButton(tr("Réinitialiser"), groupBoxFond);
    connect(boutonReinitialiserCouleurFond, SIGNAL(clicked()), this, SLOT(reinitialiserCouleurFond()));
    motifFond = new QRadioButton(tr("Motif :"), groupBoxFond);
    lineEditMotifFond = new QLineEdit(labyrinth->getTextureFond().motif, groupBoxFond);
    lineEditMotifFond->setReadOnly(true);
    QPushButton *parcourirMotifFond = new QPushButton(tr("Parcourir"), groupBoxFond);
    connect(parcourirMotifFond, SIGNAL(clicked()), this, SLOT(choisirMotifFond()));
    QPushButton *boutonReinitialiserMotifFond = new QPushButton(tr("Réinitialiser"), groupBoxFond);
    connect(boutonReinitialiserMotifFond, SIGNAL(clicked()), this, SLOT(reinitialiserMotifFond()));
    imageFond = new QRadioButton(tr("Image :"), groupBoxFond);
    lineEditImageFond = new QLineEdit(labyrinth->getTextureFond().image, groupBoxFond);
    lineEditImageFond->setReadOnly(true);
    QPushButton *parcourirImageFond = new QPushButton(tr("Parcourir"), groupBoxFond);
    connect(parcourirImageFond, SIGNAL(clicked()), this, SLOT(choisirImageFond()));
    QPushButton *boutonReinitialiserImageFond = new QPushButton(tr("Réinitialiser"), groupBoxFond);
    connect(boutonReinitialiserImageFond, SIGNAL(clicked()), this, SLOT(reinitialiserImageFond()));
    QButtonGroup *buttonGroupFond = new QButtonGroup(groupBoxFond);
    buttonGroupFond->addButton(couleurFond);
    buttonGroupFond->addButton(motifFond);
    buttonGroupFond->addButton(imageFond);
    QGridLayout *layoutGroupBoxFond = new QGridLayout;
    layoutGroupBoxFond->addWidget(couleurFond, 0, 0);
    layoutGroupBoxFond->addWidget(boutonCouleurFond, 0, 1, 1, 2);
    layoutGroupBoxFond->addWidget(boutonReinitialiserCouleurFond, 0, 3);
    layoutGroupBoxFond->addWidget(motifFond, 1, 0);
    layoutGroupBoxFond->addWidget(lineEditMotifFond, 1, 1);
    layoutGroupBoxFond->addWidget(parcourirMotifFond, 1, 2);
    layoutGroupBoxFond->addWidget(boutonReinitialiserMotifFond, 1, 3);
    layoutGroupBoxFond->addWidget(imageFond, 2, 0);
    layoutGroupBoxFond->addWidget(lineEditImageFond, 2, 1);
    layoutGroupBoxFond->addWidget(parcourirImageFond, 2, 2);
    layoutGroupBoxFond->addWidget(boutonReinitialiserImageFond, 2, 3);
    groupBoxFond->setLayout(layoutGroupBoxFond);
    switch (labyrinth->getTextureFond().typeTexture)
    {
        case QLabyrinth::TextureCouleur:
            couleurFond->setChecked(true);
            break;
        case QLabyrinth::TextureMotif:
            motifFond->setChecked(true);
            break;
        case QLabyrinth::TextureImage:
            imageFond->setChecked(true);
            break;
        default:
            break;
    }

    QGroupBox *groupBoxMur = new QGroupBox(tr("Mur"), dialog);
    couleurMur = new QRadioButton(tr("Couleur :"), groupBoxMur);
    boutonCouleurMur = new QPushButton(tr("Choisir la couleur"), groupBoxMur);
    QPixmap pixmapMur(32, 32);
    pixmapMur.fill(labyrinth->getTextureMur().couleur);
    boutonCouleurMur->setIcon(QIcon(pixmapMur));
    boutonCouleurMur->setProperty("Couleur", QString::number(labyrinth->getTextureMur().couleur.rgba()));
    connect(boutonCouleurMur, SIGNAL(clicked()), this, SLOT(choisirCouleurMur()));
    QPushButton *boutonReinitialiserCouleurMur = new QPushButton(tr("Réinitialiser"), groupBoxMur);
    connect(boutonReinitialiserCouleurMur, SIGNAL(clicked()), this, SLOT(reinitialiserCouleurMur()));
    motifMur = new QRadioButton(tr("Motif :"), groupBoxMur);
    lineEditMotifMur = new QLineEdit(labyrinth->getTextureMur().motif, groupBoxMur);
    lineEditMotifMur->setReadOnly(true);
    QPushButton *parcourirMotifMur = new QPushButton(tr("Parcourir"), groupBoxMur);
    connect(parcourirMotifMur, SIGNAL(clicked()), this, SLOT(choisirMotifMur()));
    QPushButton *boutonReinitialiserMotifMur = new QPushButton(tr("Réinitialiser"), groupBoxMur);
    connect(boutonReinitialiserMotifMur, SIGNAL(clicked()), this, SLOT(reinitialiserMotifMur()));
    imageMur = new QRadioButton(tr("Image :"), groupBoxMur);
    lineEditImageMur = new QLineEdit(labyrinth->getTextureMur().image, groupBoxMur);
    lineEditImageMur->setReadOnly(true);
    QPushButton *parcourirImageMur = new QPushButton(tr("Parcourir"), groupBoxMur);
    connect(parcourirImageMur, SIGNAL(clicked()), this, SLOT(choisirImageMur()));
    QPushButton *boutonReinitialiserImageMur = new QPushButton(tr("Réinitialiser"), groupBoxMur);
    connect(boutonReinitialiserImageMur, SIGNAL(clicked()), this, SLOT(reinitialiserImageMur()));
    QButtonGroup *buttonGroupMur = new QButtonGroup(groupBoxMur);
    buttonGroupMur->addButton(couleurMur);
    buttonGroupMur->addButton(motifMur);
    buttonGroupMur->addButton(imageMur);
    QGridLayout *layoutGroupBoxMur = new QGridLayout;
    layoutGroupBoxMur->addWidget(couleurMur, 0, 0);
    layoutGroupBoxMur->addWidget(boutonCouleurMur, 0, 1, 1, 2);
    layoutGroupBoxMur->addWidget(boutonReinitialiserCouleurMur, 0, 3);
    layoutGroupBoxMur->addWidget(motifMur, 1, 0);
    layoutGroupBoxMur->addWidget(lineEditMotifMur, 1, 1);
    layoutGroupBoxMur->addWidget(parcourirMotifMur, 1, 2);
    layoutGroupBoxMur->addWidget(boutonReinitialiserMotifMur, 1, 3);
    layoutGroupBoxMur->addWidget(imageMur, 2, 0);
    layoutGroupBoxMur->addWidget(lineEditImageMur, 2, 1);
    layoutGroupBoxMur->addWidget(parcourirImageMur, 2, 2);
    layoutGroupBoxMur->addWidget(boutonReinitialiserImageMur, 2, 3);
    groupBoxMur->setLayout(layoutGroupBoxMur);
    switch (labyrinth->getTextureMur().typeTexture)
    {
        case QLabyrinth::TextureCouleur:
            couleurMur->setChecked(true);
            break;
        case QLabyrinth::TextureMotif:
            motifMur->setChecked(true);
            break;
        case QLabyrinth::TextureImage:
            imageMur->setChecked(true);
            break;
        default:
            break;
    }

    QGroupBox *groupBoxParcours = new QGroupBox(tr("Parcours"), dialog);
    couleurParcours = new QRadioButton(tr("Couleur :"), groupBoxParcours);
    boutonCouleurParcours = new QPushButton(tr("Choisir la couleur"), groupBoxParcours);
    connect(boutonCouleurParcours, SIGNAL(clicked()), this, SLOT(choisirCouleurParcours()));
    QPixmap pixmapParcours(32, 32);
    pixmapParcours.fill(labyrinth->getTextureParcours().couleur);
    boutonCouleurParcours->setIcon(QIcon(pixmapParcours));
    boutonCouleurParcours->setProperty("Couleur", QString::number(labyrinth->getTextureParcours().couleur.rgba()));
    QPushButton *boutonReinitialiserCouleurParcours = new QPushButton(tr("Réinitialiser"), groupBoxParcours);
    connect(boutonReinitialiserCouleurParcours, SIGNAL(clicked()), this, SLOT(reinitialiserCouleurParcours()));
    motifParcours = new QRadioButton(tr("Motif :"), groupBoxParcours);
    lineEditMotifParcours = new QLineEdit(labyrinth->getTextureParcours().motif, groupBoxParcours);
    lineEditMotifParcours->setReadOnly(true);
    QPushButton *parcourirMotifParcours = new QPushButton(tr("Parcourir"), groupBoxParcours);
    connect(parcourirMotifParcours, SIGNAL(clicked()), this, SLOT(choisirMotifParcours()));
    QPushButton *boutonReinitialiserMotifParcours = new QPushButton(tr("Réinitialiser"), groupBoxParcours);
    connect(boutonReinitialiserMotifParcours, SIGNAL(clicked()), this, SLOT(reinitialiserMotifParcours()));
    imageParcours = new QRadioButton(tr("Image :"), groupBoxParcours);
    lineEditImageParcours = new QLineEdit(labyrinth->getTextureParcours().image, groupBoxParcours);
    lineEditImageParcours->setReadOnly(true);
    QPushButton *parcourirImageParcours = new QPushButton(tr("Parcourir"), groupBoxParcours);
    connect(parcourirImageParcours, SIGNAL(clicked()), this, SLOT(choisirImageParcours()));
    QPushButton *boutonReinitialiserImageParcours = new QPushButton(tr("Réinitialiser"), groupBoxParcours);
    connect(boutonReinitialiserImageParcours, SIGNAL(clicked()), this, SLOT(reinitialiserImageParcours()));
    QButtonGroup *buttonGroupParcours = new QButtonGroup(groupBoxParcours);
    buttonGroupParcours->addButton(couleurParcours);
    buttonGroupParcours->addButton(motifParcours);
    buttonGroupParcours->addButton(imageParcours);
    QGridLayout *layoutGroupBoxParcours = new QGridLayout;
    layoutGroupBoxParcours->addWidget(couleurParcours, 0, 0);
    layoutGroupBoxParcours->addWidget(boutonCouleurParcours, 0, 1, 1, 2);
    layoutGroupBoxParcours->addWidget(boutonReinitialiserCouleurParcours, 0, 3);
    layoutGroupBoxParcours->addWidget(motifParcours, 1, 0);
    layoutGroupBoxParcours->addWidget(lineEditMotifParcours, 1, 1);
    layoutGroupBoxParcours->addWidget(parcourirMotifParcours, 1, 2);
    layoutGroupBoxParcours->addWidget(boutonReinitialiserMotifParcours, 1, 3);
    layoutGroupBoxParcours->addWidget(imageParcours, 2, 0);
    layoutGroupBoxParcours->addWidget(lineEditImageParcours, 2, 1);
    layoutGroupBoxParcours->addWidget(parcourirImageParcours, 2, 2);
    layoutGroupBoxParcours->addWidget(boutonReinitialiserImageParcours, 2, 3);
    groupBoxParcours->setLayout(layoutGroupBoxParcours);
    switch (labyrinth->getTextureParcours().typeTexture)
    {
        case QLabyrinth::TextureCouleur:
            couleurParcours->setChecked(true);
            break;
        case QLabyrinth::TextureMotif:
            motifParcours->setChecked(true);
            break;
        case QLabyrinth::TextureImage:
            imageParcours->setChecked(true);
            break;
        default:
            break;
    }

    QFrame *frame = new QFrame(dialog);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(groupBoxTaille);
    vBoxLayout->addWidget(groupBoxFond);
    vBoxLayout->addWidget(groupBoxMur);
    vBoxLayout->addWidget(groupBoxParcours);
    vBoxLayout->addWidget(frame);
    vBoxLayout->addWidget(buttonBox);

    dialog->setLayout(vBoxLayout);

    if (dialog->exec() == QDialog::Rejected)
    {
        dialog->deleteLater();

        if (!pauseImposee)
            mettreEnPause();
        return;
    }

    labyrinth->setWaysSize(QSize(spinBoxLongueurChemins->value(), spinBoxLargeurChemins->value()));
    labyrinth->setWallsSize(QSize(spinBoxLongueurMurs->value(), spinBoxLargeurMurs->value()));

    QLabyrinth::Texture textureFond;
    textureFond.couleur = QColor(boutonCouleurFond->property("Couleur").toString().toUInt());
    textureFond.motif = lineEditMotifFond->text();
    textureFond.image = lineEditImageFond->text();
    if (couleurFond->isChecked())
        textureFond.typeTexture = QLabyrinth::TextureCouleur;
    else if (motifFond->isChecked())
        textureFond.typeTexture = QLabyrinth::TextureMotif;
    else
        textureFond.typeTexture = QLabyrinth::TextureImage;
    labyrinth->setTextureFond(textureFond);

    QLabyrinth::Texture textureMur;
    textureMur.couleur = QColor(boutonCouleurMur->property("Couleur").toString().toUInt());
    textureMur.motif = lineEditMotifMur->text();
    textureMur.image = lineEditImageMur->text();
    if (couleurMur->isChecked())
        textureMur.typeTexture = QLabyrinth::TextureCouleur;
    else if (motifMur->isChecked())
        textureMur.typeTexture = QLabyrinth::TextureMotif;
    else
        textureMur.typeTexture = QLabyrinth::TextureImage;
    labyrinth->setTextureMur(textureMur);

    QLabyrinth::Texture textureParcours;
    textureParcours.couleur = QColor(boutonCouleurParcours->property("Couleur").toString().toUInt());
    textureParcours.motif = lineEditMotifParcours->text();
    textureParcours.image = lineEditImageParcours->text();
    if (couleurParcours->isChecked())
        textureParcours.typeTexture = QLabyrinth::TextureCouleur;
    else if (motifParcours->isChecked())
        textureParcours.typeTexture = QLabyrinth::TextureMotif;
    else
        textureParcours.typeTexture = QLabyrinth::TextureImage;
    labyrinth->setTextureParcours(textureParcours);

    dialog->deleteLater();

    if (!pauseImposee)
        mettreEnPause();

    labyrinth->activer();
}

void MainWindow::musique()
{
    if (!pauseImposee)
        mettreEnPause();

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Musique"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset, Qt::Horizontal, dialog);
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), dialog, SLOT(reject()));
    connect(buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reinitialiserMusique()));

    lineEditMusique = new QLineEdit(emplacementMusique, dialog);
    lineEditMusique->setReadOnly(true);
    QLabel *label = new QLabel(tr("Musique :"), dialog);
    checkBoxMuet = new QCheckBox(tr("&Muet"));
    checkBoxMuet->setChecked(audioOutput->isMuted());
    QPushButton *parcourir = new QPushButton(tr("&Parcourir"), dialog);

    connect(parcourir, SIGNAL(clicked()), this, SLOT(parcourirMusique()));

    QFrame *frame = new QFrame(dialog);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(label, 0, 0, 1, 1);
    gridLayout->addWidget(lineEditMusique, 0, 1, 1, 1);
    gridLayout->addWidget(parcourir, 0, 2, 1, 1);
    gridLayout->addWidget(checkBoxMuet, 0, 3, 1, 1);
    gridLayout->addWidget(frame, 1, 0, 1, 4);
    gridLayout->addWidget(buttonBox, 2, 0, 1, 4);
    dialog->setLayout(gridLayout);

    if (dialog->exec() == QDialog::Rejected)
    {
        dialog->deleteLater();

        if (!pauseImposee)
            mettreEnPause();
        return;
    }

    if (emplacementMusique != lineEditMusique->text())
    {
        emplacementMusique = lineEditMusique->text();
        mediaPlayer->setSource(emplacementMusique);

        mediaPlayer->stop();

        if (QFileInfo(emplacementMusique).exists())
            mediaPlayer->play();
    }

    audioOutput->setMuted(checkBoxMuet->isChecked());

    dialog->deleteLater();

    if (!pauseImposee)
        mettreEnPause();
}

void MainWindow::butDuJeu()
{
    if (!pauseImposee)
        mettreEnPause();

    QDialog dialog(this);
    dialog.setWindowTitle(tr("But du jeu"));

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, Qt::Horizontal, &dialog);
    buttonBox.setCenterButtons(true);
    connect(buttonBox.button(QDialogButtonBox::Ok), SIGNAL(clicked()), &dialog, SLOT(accept()));

    QLabel label(tr("Le but du labyrinthe est de trouver\n"
                    "la sortie le plus vite possible en surmontant\n"
                    "les épreuves des modes choisis.\n"), &dialog);
    label.setAlignment(Qt::AlignCenter);

    QVBoxLayout vBoxLayout;
    vBoxLayout.addWidget(&label);
    vBoxLayout.addWidget(&buttonBox);
    dialog.setLayout(&vBoxLayout);

    dialog.exec();

    if (!pauseImposee)
        mettreEnPause();
}

void MainWindow::commandes()
{
    if (!pauseImposee)
        mettreEnPause();

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Commandes"));

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, Qt::Horizontal, &dialog);
    buttonBox.setCenterButtons(true);
    connect(buttonBox.button(QDialogButtonBox::Ok), SIGNAL(clicked()), &dialog, SLOT(accept()));

    QLabel label(tr("Pour se déplacer dans le labyrinthe,\n"
                    "vous pouvez utiliser soit les touches\n"
                    "directionnelles, soit la souris."), &dialog);
    label.setAlignment(Qt::AlignCenter);

    QVBoxLayout vBoxLayout;
    vBoxLayout.addWidget(&label);
    vBoxLayout.addWidget(&buttonBox);
    dialog.setLayout(&vBoxLayout);

    dialog.exec();

    if (!pauseImposee)
        mettreEnPause();
}

void MainWindow::aPropos()
{
    if (!pauseImposee)
        mettreEnPause();

    QDialog dialog(this);
    dialog.setWindowTitle(tr("À propos du Labyrinthe ..."));

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, Qt::Horizontal, &dialog);
    buttonBox.setCenterButtons(true);
    connect(buttonBox.button(QDialogButtonBox::Ok), SIGNAL(clicked()), &dialog, SLOT(accept()));

    QLabel label(tr("Labyrinthe %1\n\nLe labyrinthe est un jeu gratuit et\n"
                    "libre de distribution réalisé par LIVET Julien\n"
                    "à l'aide des bibliothèques Qt %2 et FMOD,\n"
                    "et de LIVET Claire pour la traduction italienne, ainsi\n"
                    "que de LIVET Benjamin pour la traduction allemande.\n").arg(VERSION).arg(qVersion()), &dialog);
    label.setAlignment(Qt::AlignCenter);

    QVBoxLayout vBoxLayout;
    vBoxLayout.addWidget(&label);
    vBoxLayout.addWidget(&buttonBox);
    dialog.setLayout(&vBoxLayout);

    dialog.exec();

    if (!pauseImposee)
        mettreEnPause();
}

void MainWindow::stateChanged(int)
{
    labyrinth->activer();
}

void MainWindow::choisirCouleurFond()
{
    QColor couleur = QColorDialog::getColor(QColor(boutonCouleurFond->property("Couleur").toString().toUInt()),
                                            boutonCouleurFond, tr("Choisir une couleur de fond"));

    if (!couleur.isValid())
        return;

    QPixmap pixmapFond(32, 32);
    pixmapFond.fill(couleur);
    boutonCouleurFond->setIcon(QIcon(pixmapFond));
    boutonCouleurFond->setProperty("Couleur", QString::number(couleur.rgba()));
}

void MainWindow::reinitialiserCouleurFond()
{
    QPixmap pixmapFond(32, 32);
    pixmapFond.fill(COULEURFOND);
    boutonCouleurFond->setIcon(QIcon(pixmapFond));
    boutonCouleurFond->setProperty("Couleur", QString::number(COULEURFOND.rgba()));
}

void MainWindow::choisirMotifFond()
{
    QString s = lineEditMotifFond->text();
    if (!QFileInfo(s).exists())
        s = QApplication::applicationDirPath() + QString("/Images");
    QString image = QFileDialog::getOpenFileName(lineEditMotifFond, tr("Choisir un motif de fond"), s,
                                                 tr("Images (*.jpg *.png *.bmp *.gif)"));

    if (image.isEmpty())
        return;

    lineEditMotifFond->setText(image);
}

void MainWindow::reinitialiserMotifFond()
{
    lineEditMotifFond->setText(MOTIFFOND);
}

void MainWindow::choisirImageFond()
{
    QString s = lineEditImageFond->text();
    if (!QFileInfo(s).exists() || s.startsWith(QString(":/")))
        s = QApplication::applicationDirPath() + QString("/Images");
    QString image = QFileDialog::getOpenFileName(lineEditImageFond, tr("Choisir une image de fond"), s,
                                                 tr("Images (*.jpg *.png *.bmp *.gif)"));

    if (image.isEmpty())
        return;

    lineEditImageFond->setText(image);
}

void MainWindow::reinitialiserImageFond()
{
    lineEditImageFond->setText(IMAGEFOND);
}

void MainWindow::choisirCouleurMur()
{
    QColor couleur = QColorDialog::getColor(QColor(boutonCouleurMur->property("Couleur").toString().toUInt()),
                                            boutonCouleurMur, tr("Choisir une couleur de mur"));

    if (!couleur.isValid())
        return;

    QPixmap pixmapMur(32, 32);
    pixmapMur.fill(couleur);
    boutonCouleurMur->setIcon(QIcon(pixmapMur));
    boutonCouleurMur->setProperty("Couleur", QString::number(couleur.rgba()));
}

void MainWindow::reinitialiserCouleurMur()
{
    QPixmap pixmapMur(32, 32);
    pixmapMur.fill(COULEURMUR);
    boutonCouleurMur->setIcon(QIcon(pixmapMur));
    boutonCouleurMur->setProperty("Couleur", QString::number(COULEURMUR.rgba()));
}

void MainWindow::choisirMotifMur()
{
    QString s = lineEditMotifMur->text();
    if (!QFileInfo(s).exists() || s.startsWith(QString(":/")))
        s = QApplication::applicationDirPath() + QString("/Images");
    QString image = QFileDialog::getOpenFileName(lineEditMotifMur, tr("Choisir un motif de mur"), s,
                                                 tr("Images (*.jpg *.png *.bmp *.gif)"));

    if (image.isEmpty())
        return;

    lineEditMotifMur->setText(image);
}

void MainWindow::reinitialiserMotifMur()
{
    lineEditMotifMur->setText(MOTIFMUR);
}

void MainWindow::choisirImageMur()
{
    QString s = lineEditImageMur->text();
    if (!QFileInfo(s).exists() || s.startsWith(QString(":/")))
        s = QApplication::applicationDirPath() + QString("/Images");
    QString image = QFileDialog::getOpenFileName(lineEditImageMur, tr("Choisir une image de mur"), s,
                                                 tr("Images (*.jpg *.png *.bmp *.gif)"));

    if (image.isEmpty())
        return;

    lineEditImageMur->setText(image);
}

void MainWindow::reinitialiserImageMur()
{
    lineEditImageMur->setText(IMAGEMUR);
}

void MainWindow::choisirCouleurParcours()
{
    QColor couleur = QColorDialog::getColor(QColor(boutonCouleurParcours->property("Couleur").toString().toUInt()),
                                            boutonCouleurParcours, tr("Choisir une couleur de parcours"));

    if (!couleur.isValid())
        return;

    QPixmap pixmapParcours(32, 32);
    pixmapParcours.fill(couleur);
    boutonCouleurParcours->setIcon(QIcon(pixmapParcours));
    boutonCouleurParcours->setProperty("Couleur", QString::number(couleur.rgba()));
}

void MainWindow::reinitialiserCouleurParcours()
{
    QPixmap pixmapParcours(32, 32);
    pixmapParcours.fill(COULEURPARCOURS);
    boutonCouleurParcours->setIcon(QIcon(pixmapParcours));
    boutonCouleurParcours->setProperty("Couleur", QString::number(COULEURPARCOURS.rgba()));
}

void MainWindow::choisirMotifParcours()
{
    QString s = lineEditMotifParcours->text();
    if (!QFileInfo(s).dir().exists() || s.startsWith(QString(":/")))
        s = QApplication::applicationDirPath() + QString("/Images");
    QString image = QFileDialog::getOpenFileName(lineEditMotifParcours, tr("Choisir un motif de parcours"), s,
                                                 tr("Images (*.jpg *.png *.bmp *.gif)"));

    if (image.isEmpty())
        return;

    lineEditMotifParcours->setText(image);
}

void MainWindow::reinitialiserMotifParcours()
{
    lineEditMotifParcours->setText(MOTIFPARCOURS);
}

void MainWindow::choisirImageParcours()
{
    QString s = lineEditImageParcours->text();
    if (!QFileInfo(s).exists() || s.startsWith(QString(":/")))
        s = QApplication::applicationDirPath() + QString("/Images");
    QString image = QFileDialog::getOpenFileName(lineEditImageParcours, tr("Choisir une image de parcours"), s,
                                                 tr("Images (*.jpg *.png *.bmp *.gif)"));

    if (image.isEmpty())
        return;

    lineEditImageParcours->setText(image);
}

void MainWindow::reinitialiserImageParcours()
{
    lineEditImageParcours->setText(IMAGEPARCOURS);
}

void MainWindow::reinitialiserLongueurChemins()
{
    spinBoxLongueurChemins->setValue(WAYSSIZE.width());
}

void MainWindow::reinitialiserLargeurChemins()
{
    spinBoxLargeurChemins->setValue(WAYSSIZE.height());
}

void MainWindow::reinitialiserLongueurMurs()
{
    spinBoxLongueurMurs->setValue(WALLSSIZE.width());
}

void MainWindow::reinitialiserLargeurMurs()
{
    spinBoxLargeurMurs->setValue(WALLSSIZE.height());
}

void MainWindow::reinitialiserAffichage()
{
    reinitialiserCouleurFond();
    reinitialiserMotifFond();
    reinitialiserImageFond();
    reinitialiserCouleurMur();
    reinitialiserMotifMur();
    reinitialiserImageMur();
    reinitialiserCouleurParcours();
    reinitialiserMotifParcours();
    reinitialiserImageParcours();
    reinitialiserLongueurChemins();
    reinitialiserLargeurChemins();
    reinitialiserLongueurMurs();
    reinitialiserLargeurMurs();

    couleurFond->setChecked(true);
    couleurMur->setChecked(true);
    couleurParcours->setChecked(true);
}

void MainWindow::pleinEcran()
{
    if (actionPleinEcran->isChecked())
        setWindowState(windowState() | Qt::WindowFullScreen);
    else if (isFullScreen())
        setWindowState(windowState() ^ Qt::WindowFullScreen);
}

void MainWindow::labyrintheSeulement()
{
    if (actionLabyrintheSeulement->isChecked())
    {
        menuBar()->setVisible(false);
        groupBoxNiveau->setVisible(false);
        groupBoxChronometre->setVisible(false);
        groupBoxDeplacement->setVisible(false);
    }
    else
    {
        menuBar()->setVisible(true);
        groupBoxNiveau->setVisible(true);
        groupBoxChronometre->setVisible(true);
        groupBoxDeplacement->setVisible(true);
    }
}

void MainWindow::stateChangedAdapation(int state)
{
    groupBoxTailleLabyrinthe->setDisabled(state == Qt::Checked);
    checkBoxFormats->setDisabled(state == Qt::Checked);
    boutonConfigurerFormats->setDisabled(state == Qt::Checked);

    adaptationEcran = (state == Qt::Checked);

    int longueur = QGuiApplication::screenAt(pos())->size().width() / labyrinth->getTailleCase().width();
    if (!(longueur % 2))
    {
        longueur++;
        if (longueur * labyrinth->getTailleCase().width() > QGuiApplication::screenAt(pos())->size().width())
            longueur -= 2;
    }
    int largeur = QGuiApplication::screenAt(pos())->size().height() / labyrinth->getTailleCase().height();
    if (!(largeur % 2))
    {
        largeur++;
        if (largeur * labyrinth->getTailleCase().height() > QGuiApplication::screenAt(pos())->size().height())
            largeur -= 2;
    }

    spinBoxLongueur->setValue(longueur);
    spinBoxLargeur->setValue(largeur);
}

void MainWindow::stateChangedFormats(int state)
{
    groupBoxTailleLabyrinthe->setDisabled(state == Qt::Checked);
    checkBoxAdaptation->setDisabled(state == Qt::Checked);

    adaptationFormats = (state == Qt::Checked);

    int longueurOrigine = 0;
    int largeurOrigine = 0;

    if (printer->pageRect(QPrinter::Millimeter).width() > printer->pageRect(QPrinter::Millimeter).height())
    {
        longueurOrigine = printer->pageRect(QPrinter::Millimeter).width();
        largeurOrigine = printer->pageRect(QPrinter::Millimeter).height();
    }
    else
    {
        longueurOrigine = printer->pageRect(QPrinter::Millimeter).height();
        largeurOrigine = printer->pageRect(QPrinter::Millimeter).width();
    }

    int longueur = longueurOrigine / labyrinth->getTailleCase().width();
    int largeur = largeurOrigine / labyrinth->getTailleCase().height();

    if (!(longueur % 2))
    {
        longueur++;
        if (longueur * labyrinth->getTailleCase().width() > longueurOrigine)
            longueur -= 2;
    }
    if (!(largeur % 2))
    {
        largeur++;
        if (largeur * labyrinth->getTailleCase().height() > largeurOrigine)
            largeur -= 2;
    }

    spinBoxLongueur->setValue(longueur);
    spinBoxLargeur->setValue(largeur);
}

void MainWindow::modes()
{
    if (!pauseImposee)
        mettreEnPause();

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Modes"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset, Qt::Horizontal, dialog);
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), dialog, SLOT(reject()));
    connect(buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reinitialiserModes()));

    tabWidgetModes = new TabWidget(this);

    //Mode obscurité
    QWidget *obscurite = new QWidget(tabWidgetModes);
    QLabel *couleurObscurite = new QLabel(tr("Couleur :"), obscurite);
    boutonCouleurObscurite = new QPushButton(tr("Choisir la couleur"), obscurite);
    connect(boutonCouleurObscurite, SIGNAL(clicked()), this, SLOT(choisirCouleurObscurite()));
    QPixmap pixmapObscurite(32, 32);
    pixmapObscurite.fill(labyrinth->getModeLabyrinthe().couleurObscurite);
    boutonCouleurObscurite->setIcon(QIcon(pixmapObscurite));
    boutonCouleurObscurite->setProperty("Couleur", QString::number(labyrinth->getModeLabyrinthe().couleurObscurite.rgba()));
    QPushButton *boutonReinitialiserCouleurObscurite = new QPushButton(tr("Réinitialiser"), obscurite);
    connect(boutonReinitialiserCouleurObscurite, SIGNAL(clicked()), this, SLOT(reinitialiserCouleurObscurite()));
    QLabel *rayonObscurite = new QLabel(tr("Rayon :"), obscurite);
    spinBoxRayonObscurite = new QSpinBox(obscurite);
    spinBoxRayonObscurite->setRange(1, 20);
    spinBoxRayonObscurite->setValue(labyrinth->getModeLabyrinthe().rayonObscurite);
    QPushButton *boutonReinitialiserRayonObscurite = new QPushButton(tr("Réinitialiser"), obscurite);
    connect(boutonReinitialiserRayonObscurite, SIGNAL(clicked()), this, SLOT(reinitialiserRayonObscurite()));
    QGridLayout *gridLayoutObscurite = new QGridLayout;
    gridLayoutObscurite->addWidget(couleurObscurite, 0, 0);
    gridLayoutObscurite->addWidget(boutonCouleurObscurite, 0, 1);
    gridLayoutObscurite->addWidget(boutonReinitialiserCouleurObscurite, 0, 2);
    gridLayoutObscurite->addWidget(rayonObscurite, 1, 0);
    gridLayoutObscurite->addWidget(spinBoxRayonObscurite, 1, 1);
    gridLayoutObscurite->addWidget(boutonReinitialiserRayonObscurite, 1, 2);
    obscurite->setLayout(gridLayoutObscurite);
    tabWidgetModes->addTab(obscurite, tr("&Obscurité"));
    QCheckBox *checkBoxObscurite = new QCheckBox(obscurite);
    checkBoxObscurite->setChecked(labyrinth->getModeLabyrinthe().mode & QLabyrinth::Obscurite);
    tabWidgetModes->tabBar()->setTabButton(0, QTabBar::LeftSide, checkBoxObscurite);

    //Mode rotation
    QWidget *rotation = new QWidget(tabWidgetModes);
    QLabel *angle = new QLabel(tr("Angle :"), rotation);
    spinBoxAngleRotation = new QSpinBox(rotation);
    spinBoxAngleRotation->setRange(0, 359);
    spinBoxAngleRotation->setValue(labyrinth->getModeLabyrinthe().angleRotation);
    spinBoxAngleRotation->setSuffix(tr("°"));
    checkBoxAngleRotationAuHasard = new QCheckBox(tr("Au hasard"), rotation);
    checkBoxAngleRotationAuHasard->setChecked(labyrinth->getModeLabyrinthe().angleRotationAuHasard);
    QPushButton *boutonReinitialiserAngleRotation = new QPushButton(tr("Réinitialiser"), rotation);
    connect(boutonReinitialiserAngleRotation, SIGNAL(clicked()), this, SLOT(reinitialiserAngleRotation()));
    QLabel *intervalle = new QLabel(tr("Intervalle :"), rotation);
    spinBoxIntervalleRotation = new QSpinBox(rotation);
    spinBoxIntervalleRotation->setRange(INTERVALLEROTATIONMIN, INTERVALLEROTATIONMAX);
    spinBoxIntervalleRotation->setValue(labyrinth->getModeLabyrinthe().intervalleRotation);
    spinBoxIntervalleRotation->setSuffix(tr(" ms"));
    checkBoxIntervalleRotationAuHasard = new QCheckBox(tr("Au hasard"), rotation);
    checkBoxIntervalleRotationAuHasard->setChecked(labyrinth->getModeLabyrinthe().intervalleRotationAuHasard);
    QPushButton *boutonReinitialiserIntervalleRotation = new QPushButton(tr("Réinitialiser"), rotation);
    connect(boutonReinitialiserIntervalleRotation, SIGNAL(clicked()), this, SLOT(reinitialiserIntervalleRotation()));
    checkBoxSensRotation = new QCheckBox(tr("Sens horaire"), rotation);
    checkBoxSensRotation->setChecked(labyrinth->getModeLabyrinthe().sensRotation == 1);
    checkBoxSensRotationAuHasard = new QCheckBox(tr("Au hasard"), rotation);
    checkBoxSensRotationAuHasard->setChecked(labyrinth->getModeLabyrinthe().sensRotationAuHasard);
    QPushButton *boutonReinitialiserSensRotation = new QPushButton(tr("Réinitialiser"), rotation);
    connect(boutonReinitialiserSensRotation, SIGNAL(clicked()), this, SLOT(reinitialiserSensRotation()));
    checkBoxRotationFixe = new QCheckBox(tr("Fixe"), rotation);
    checkBoxRotationFixe->setChecked(labyrinth->getModeLabyrinthe().rotationFixe);
    QGridLayout *gridLayoutRotation = new QGridLayout;
    gridLayoutRotation->addWidget(angle, 0, 0);
    gridLayoutRotation->addWidget(spinBoxAngleRotation, 0, 1);
    gridLayoutRotation->addWidget(checkBoxAngleRotationAuHasard, 0, 2);
    gridLayoutRotation->addWidget(boutonReinitialiserAngleRotation, 0, 3);
    gridLayoutRotation->addWidget(intervalle, 1, 0);
    gridLayoutRotation->addWidget(spinBoxIntervalleRotation, 1, 1);
    gridLayoutRotation->addWidget(checkBoxIntervalleRotationAuHasard, 1, 2);
    gridLayoutRotation->addWidget(boutonReinitialiserIntervalleRotation, 1, 3);
    gridLayoutRotation->addWidget(checkBoxSensRotation, 2, 0, 1, 2);
    gridLayoutRotation->addWidget(checkBoxSensRotationAuHasard, 2, 2);
    gridLayoutRotation->addWidget(boutonReinitialiserSensRotation, 2, 3);
    gridLayoutRotation->addWidget(checkBoxRotationFixe, 3, 0, 1, 4);
    rotation->setLayout(gridLayoutRotation);
    QCheckBox *checkBoxRotation = new QCheckBox(rotation);
    checkBoxRotation->setChecked(labyrinth->getModeLabyrinthe().mode & QLabyrinth::Rotation);
    if (labyrinth->getTypeLabyrinthe() == QLabyrinth::Labyrinthe2Den3D)
        rotation->hide();
    else
    {
        tabWidgetModes->addTab(rotation, tr("&Rotation"));
        tabWidgetModes->tabBar()->setTabButton(1, QTabBar::LeftSide, checkBoxRotation);
    }

    //Mode distorsion
    QWidget *distorsion = new QWidget(tabWidgetModes);
    QLabel *pourcentageDistorsionVerticale = new QLabel(tr("Pourcentage :"), distorsion);
    spinBoxPourcentageDistorsionVerticale = new QSpinBox(distorsion);
    spinBoxPourcentageDistorsionVerticale->setRange(0, 100);
    spinBoxPourcentageDistorsionVerticale->setValue(labyrinth->getModeLabyrinthe().pourcentageDistorsionVerticale);
    spinBoxPourcentageDistorsionVerticale->setSuffix(tr("%"));
    checkBoxPourcentageDistorsionVerticaleAuHasard = new QCheckBox(tr("Au hasard"), distorsion);
    checkBoxPourcentageDistorsionVerticaleAuHasard->setChecked(labyrinth->getModeLabyrinthe().pourcentageDistorsionVerticaleAuHasard);
    QPushButton *boutonReinitialiserPourcentageDistorsionVerticale = new QPushButton(tr("Réinitialiser"), distorsion);
    connect(boutonReinitialiserPourcentageDistorsionVerticale, SIGNAL(clicked()), this, SLOT(reinitialiserPourcentageDistorsionVerticale()));
    QLabel *intervalleDistorsionVerticale = new QLabel(tr("Intervalle :"), distorsion);
    spinBoxIntervalleDistorsionVerticale = new QSpinBox(distorsion);
    spinBoxIntervalleDistorsionVerticale->setRange(INTERVALLEDISTORSIONVERTICALEMIN, INTERVALLEDISTORSIONVERTICALEMAX);
    spinBoxIntervalleDistorsionVerticale->setValue(labyrinth->getModeLabyrinthe().intervalleDistorsionVerticale);
    spinBoxIntervalleDistorsionVerticale->setSuffix(tr(" ms"));
    checkBoxIntervalleDistorsionVerticaleAuHasard = new QCheckBox(tr("Au hasard"), distorsion);
    checkBoxIntervalleDistorsionVerticaleAuHasard->setChecked(labyrinth->getModeLabyrinthe().intervalleDistorsionVerticaleAuHasard);
    QPushButton *boutonReinitialiserIntervalleDistorsionVerticale = new QPushButton(tr("Réinitialiser"), distorsion);
    connect(boutonReinitialiserIntervalleDistorsionVerticale, SIGNAL(clicked()), this, SLOT(reinitialiserIntervalleDistorsionVerticale()));
    checkBoxDistorsionVerticaleFixe = new QCheckBox(tr("Fixe"), rotation);
    checkBoxDistorsionVerticaleFixe->setChecked(labyrinth->getModeLabyrinthe().distorsionVerticaleFixe);
    QGroupBox *groupBoxDistorsionVerticale = new QGroupBox(tr("Verticale"), distorsion);
    QGridLayout *gridLayoutGroupBoxDistorsionVerticale = new QGridLayout;
    gridLayoutGroupBoxDistorsionVerticale->addWidget(pourcentageDistorsionVerticale, 0, 0);
    gridLayoutGroupBoxDistorsionVerticale->addWidget(spinBoxPourcentageDistorsionVerticale, 0, 1);
    gridLayoutGroupBoxDistorsionVerticale->addWidget(checkBoxPourcentageDistorsionVerticaleAuHasard, 0, 2);
    gridLayoutGroupBoxDistorsionVerticale->addWidget(boutonReinitialiserPourcentageDistorsionVerticale, 0, 3);
    gridLayoutGroupBoxDistorsionVerticale->addWidget(intervalleDistorsionVerticale, 1, 0);
    gridLayoutGroupBoxDistorsionVerticale->addWidget(spinBoxIntervalleDistorsionVerticale, 1, 1);
    gridLayoutGroupBoxDistorsionVerticale->addWidget(checkBoxIntervalleDistorsionVerticaleAuHasard, 1, 2);
    gridLayoutGroupBoxDistorsionVerticale->addWidget(boutonReinitialiserIntervalleDistorsionVerticale, 1, 3);
    gridLayoutGroupBoxDistorsionVerticale->addWidget(checkBoxDistorsionVerticaleFixe, 2, 0, 1, 4);
    groupBoxDistorsionVerticale->setLayout(gridLayoutGroupBoxDistorsionVerticale);
    QLabel *pourcentageDistorsionHorizontale = new QLabel(tr("Pourcentage :"), distorsion);
    spinBoxPourcentageDistorsionHorizontale = new QSpinBox(distorsion);
    spinBoxPourcentageDistorsionHorizontale->setRange(0, 100);
    spinBoxPourcentageDistorsionHorizontale->setValue(labyrinth->getModeLabyrinthe().pourcentageDistorsionHorizontale);
    spinBoxPourcentageDistorsionHorizontale->setSuffix(tr("%"));
    checkBoxPourcentageDistorsionHorizontaleAuHasard = new QCheckBox(tr("Au hasard"), distorsion);
    checkBoxPourcentageDistorsionHorizontaleAuHasard->setChecked(labyrinth->getModeLabyrinthe().pourcentageDistorsionHorizontaleAuHasard);
    QPushButton *boutonReinitialiserPourcentageDistorsionHorizontale = new QPushButton(tr("Réinitialiser"), distorsion);
    connect(boutonReinitialiserPourcentageDistorsionHorizontale, SIGNAL(clicked()), this, SLOT(reinitialiserPourcentageDistorsionHorizontale()));
    QLabel *intervalleDistorsionHorizontale = new QLabel(tr("Intervalle :"), distorsion);
    spinBoxIntervalleDistorsionHorizontale = new QSpinBox(distorsion);
    spinBoxIntervalleDistorsionHorizontale->setRange(INTERVALLEDISTORSIONHORIZONTALEMIN, INTERVALLEDISTORSIONHORIZONTALEMAX);
    spinBoxIntervalleDistorsionHorizontale->setValue(labyrinth->getModeLabyrinthe().intervalleDistorsionHorizontale);
    spinBoxIntervalleDistorsionHorizontale->setSuffix(tr(" ms"));
    checkBoxIntervalleDistorsionHorizontaleAuHasard = new QCheckBox(tr("Au hasard"), distorsion);
    checkBoxIntervalleDistorsionHorizontaleAuHasard->setChecked(labyrinth->getModeLabyrinthe().intervalleDistorsionHorizontaleAuHasard);
    QPushButton *boutonReinitialiserIntervalleDistorsionHorizontale = new QPushButton(tr("Réinitialiser"), distorsion);
    connect(boutonReinitialiserIntervalleDistorsionHorizontale, SIGNAL(clicked()), this, SLOT(reinitialiserIntervalleDistorsionHorizontale()));
    checkBoxDistorsionSynchronisee = new QCheckBox(tr("Distorsion synchronisée"), distorsion);
    checkBoxDistorsionSynchronisee->setChecked(labyrinth->getModeLabyrinthe().distorsionSynchronisee);
    connect(checkBoxDistorsionSynchronisee, SIGNAL(stateChanged(int)), this, SLOT(synchroniserDistorsion(int)));
    checkBoxDistorsionSynchroniseeAuHasard = new QCheckBox(tr("Au hasard"), distorsion);
    checkBoxDistorsionSynchroniseeAuHasard->setChecked(labyrinth->getModeLabyrinthe().distorsionSynchroniseeAuHasard);
    QPushButton *boutonReinitialiserDistorsionSynchronisee = new QPushButton(tr("Réinitialiser"), distorsion);
    connect(boutonReinitialiserDistorsionSynchronisee, SIGNAL(clicked()), this, SLOT(reinitialiserDistorsionSynchronisee()));
    checkBoxDistorsionHorizontaleFixe = new QCheckBox(tr("Fixe"), rotation);
    checkBoxDistorsionHorizontaleFixe->setChecked(labyrinth->getModeLabyrinthe().distorsionHorizontaleFixe);
    synchroniserDistorsion(checkBoxDistorsionSynchronisee->checkState());
    QGroupBox *groupBoxDistorsionHorizontale = new QGroupBox(tr("Horizontale"), distorsion);
    QGridLayout *gridLayoutGroupBoxDistorsionHorizontale = new QGridLayout;
    gridLayoutGroupBoxDistorsionHorizontale->addWidget(pourcentageDistorsionHorizontale, 0, 0);
    gridLayoutGroupBoxDistorsionHorizontale->addWidget(spinBoxPourcentageDistorsionHorizontale, 0, 1);
    gridLayoutGroupBoxDistorsionHorizontale->addWidget(checkBoxPourcentageDistorsionHorizontaleAuHasard, 0, 2);
    gridLayoutGroupBoxDistorsionHorizontale->addWidget(boutonReinitialiserPourcentageDistorsionHorizontale, 0, 3);
    gridLayoutGroupBoxDistorsionHorizontale->addWidget(intervalleDistorsionHorizontale, 1, 0);
    gridLayoutGroupBoxDistorsionHorizontale->addWidget(spinBoxIntervalleDistorsionHorizontale, 1, 1);
    gridLayoutGroupBoxDistorsionHorizontale->addWidget(checkBoxIntervalleDistorsionHorizontaleAuHasard, 1, 2);
    gridLayoutGroupBoxDistorsionHorizontale->addWidget(boutonReinitialiserIntervalleDistorsionHorizontale, 1, 3);
    gridLayoutGroupBoxDistorsionHorizontale->addWidget(checkBoxDistorsionHorizontaleFixe, 2, 0, 1, 4);
    groupBoxDistorsionHorizontale->setLayout(gridLayoutGroupBoxDistorsionHorizontale);
    QGridLayout *gridLayoutDistorsion = new QGridLayout;
    gridLayoutDistorsion->addWidget(groupBoxDistorsionVerticale, 0, 0, 1, 3);
    gridLayoutDistorsion->addWidget(groupBoxDistorsionHorizontale, 1, 0, 1, 3);
    gridLayoutDistorsion->addWidget(checkBoxDistorsionSynchronisee, 2, 0);
    gridLayoutDistorsion->addWidget(checkBoxDistorsionSynchroniseeAuHasard, 2, 1);
    gridLayoutDistorsion->addWidget(boutonReinitialiserDistorsionSynchronisee, 2, 2);
    distorsion->setLayout(gridLayoutDistorsion);
    QCheckBox *checkBoxDistorsion = new QCheckBox(distorsion);
    checkBoxDistorsion->setChecked(labyrinth->getModeLabyrinthe().mode & QLabyrinth::Distorsion);
    if (labyrinth->getTypeLabyrinthe() == QLabyrinth::Labyrinthe2Den3D)
        distorsion->hide();
    else
    {
        tabWidgetModes->addTab(distorsion, tr("&Distorsion"));
        tabWidgetModes->tabBar()->setTabButton(2, QTabBar::LeftSide, checkBoxDistorsion);
    }

    //Mode cisaillement
    QWidget *cisaillement = new QWidget(tabWidgetModes);
    QLabel *pourcentageCisaillementVertical = new QLabel(tr("Pourcentage :"), cisaillement);
    spinBoxPourcentageCisaillementVertical = new QSpinBox(cisaillement);
    spinBoxPourcentageCisaillementVertical->setRange(0, 100);
    spinBoxPourcentageCisaillementVertical->setValue(labyrinth->getModeLabyrinthe().pourcentageCisaillementVertical);
    spinBoxPourcentageCisaillementVertical->setSuffix(tr("%"));
    checkBoxPourcentageCisaillementVerticalAuHasard = new QCheckBox(tr("Au hasard"), cisaillement);
    checkBoxPourcentageCisaillementVerticalAuHasard->setChecked(labyrinth->getModeLabyrinthe().pourcentageCisaillementVerticalAuHasard);
    QPushButton *boutonReinitialiserPourcentageCisaillementVertical = new QPushButton(tr("Réinitialiser"), cisaillement);
    connect(boutonReinitialiserPourcentageCisaillementVertical, SIGNAL(clicked()), this, SLOT(reinitialiserPourcentageCisaillementVertical()));
    QLabel *intervalleCisaillementVertical = new QLabel(tr("Intervalle :"), cisaillement);
    spinBoxIntervalleCisaillementVertical = new QSpinBox(cisaillement);
    spinBoxIntervalleCisaillementVertical->setRange(INTERVALLECISAILLEMENTVERTICALMIN, INTERVALLECISAILLEMENTVERTICALMAX);
    spinBoxIntervalleCisaillementVertical->setValue(labyrinth->getModeLabyrinthe().intervalleCisaillementVertical);
    spinBoxIntervalleCisaillementVertical->setSuffix(tr(" ms"));
    checkBoxIntervalleCisaillementVerticalAuHasard = new QCheckBox(tr("Au hasard"), cisaillement);
    checkBoxIntervalleCisaillementVerticalAuHasard->setChecked(labyrinth->getModeLabyrinthe().intervalleCisaillementVerticalAuHasard);
    QPushButton *boutonReinitialiserIntervalleCisaillementVertical = new QPushButton(tr("Réinitialiser"), cisaillement);
    connect(boutonReinitialiserIntervalleCisaillementVertical, SIGNAL(clicked()), this, SLOT(reinitialiserIntervalleCisaillementVertical()));
    checkBoxCisaillementVerticalFixe = new QCheckBox(tr("Fixe"), rotation);
    checkBoxCisaillementVerticalFixe->setChecked(labyrinth->getModeLabyrinthe().cisaillementVerticalFixe);
    QGroupBox *groupBoxCisaillementVertical = new QGroupBox(tr("Vertical"), cisaillement);
    QGridLayout *gridLayoutGroupBoxCisaillementVertical = new QGridLayout;
    gridLayoutGroupBoxCisaillementVertical->addWidget(pourcentageCisaillementVertical, 0, 0);
    gridLayoutGroupBoxCisaillementVertical->addWidget(spinBoxPourcentageCisaillementVertical, 0, 1);
    gridLayoutGroupBoxCisaillementVertical->addWidget(checkBoxPourcentageCisaillementVerticalAuHasard, 0, 2);
    gridLayoutGroupBoxCisaillementVertical->addWidget(boutonReinitialiserPourcentageCisaillementVertical, 0, 3);
    gridLayoutGroupBoxCisaillementVertical->addWidget(intervalleCisaillementVertical, 1, 0);
    gridLayoutGroupBoxCisaillementVertical->addWidget(spinBoxIntervalleCisaillementVertical, 1, 1);
    gridLayoutGroupBoxCisaillementVertical->addWidget(checkBoxIntervalleCisaillementVerticalAuHasard, 1, 2);
    gridLayoutGroupBoxCisaillementVertical->addWidget(boutonReinitialiserIntervalleCisaillementVertical, 1, 3);
    gridLayoutGroupBoxCisaillementVertical->addWidget(checkBoxCisaillementVerticalFixe, 2, 0, 1, 4);
    groupBoxCisaillementVertical->setLayout(gridLayoutGroupBoxCisaillementVertical);
    QLabel *pourcentageCisaillementHorizontal = new QLabel(tr("Pourcentage :"), cisaillement);
    spinBoxPourcentageCisaillementHorizontal = new QSpinBox(cisaillement);
    spinBoxPourcentageCisaillementHorizontal->setRange(0, 100);
    spinBoxPourcentageCisaillementHorizontal->setValue(labyrinth->getModeLabyrinthe().pourcentageCisaillementHorizontal);
    spinBoxPourcentageCisaillementHorizontal->setSuffix(tr("%"));
    checkBoxPourcentageCisaillementHorizontalAuHasard = new QCheckBox(tr("Au hasard"), cisaillement);
    checkBoxPourcentageCisaillementHorizontalAuHasard->setChecked(labyrinth->getModeLabyrinthe().pourcentageCisaillementHorizontalAuHasard);
    QPushButton *boutonReinitialiserPourcentageCisaillementHorizontal = new QPushButton(tr("Réinitialiser"), cisaillement);
    connect(boutonReinitialiserPourcentageCisaillementHorizontal, SIGNAL(clicked()), this, SLOT(reinitialiserPourcentageCisaillementHorizontal()));
    QLabel *intervalleCisaillementHorizontal = new QLabel(tr("Intervalle :"), cisaillement);
    spinBoxIntervalleCisaillementHorizontal = new QSpinBox(cisaillement);
    spinBoxIntervalleCisaillementHorizontal->setRange(INTERVALLECISAILLEMENTHORIZONTALMIN, INTERVALLECISAILLEMENTHORIZONTALMAX);
    spinBoxIntervalleCisaillementHorizontal->setValue(labyrinth->getModeLabyrinthe().intervalleCisaillementHorizontal);
    spinBoxIntervalleCisaillementHorizontal->setSuffix(tr(" ms"));
    checkBoxIntervalleCisaillementHorizontalAuHasard = new QCheckBox(tr("Au hasard"), cisaillement);
    checkBoxIntervalleCisaillementHorizontalAuHasard->setChecked(labyrinth->getModeLabyrinthe().intervalleCisaillementHorizontalAuHasard);
    QPushButton *boutonReinitialiserIntervalleCisaillementHorizontal = new QPushButton(tr("Réinitialiser"), cisaillement);
    connect(boutonReinitialiserIntervalleCisaillementHorizontal, SIGNAL(clicked()), this, SLOT(reinitialiserIntervalleCisaillementHorizontal()));
    checkBoxCisaillementSynchronise = new QCheckBox(tr("Cisaillement synchronisé"), cisaillement);
    checkBoxCisaillementSynchronise->setChecked(labyrinth->getModeLabyrinthe().cisaillementSynchronise);
    connect(checkBoxCisaillementSynchronise, SIGNAL(stateChanged(int)), this, SLOT(synchroniserCisaillement(int)));
    checkBoxCisaillementSynchroniseAuHasard = new QCheckBox(tr("Au hasard"), cisaillement);
    checkBoxCisaillementSynchroniseAuHasard->setChecked(labyrinth->getModeLabyrinthe().cisaillementSynchroniseAuHasard);
    QPushButton *boutonReinitialiserCisaillementSynchronise = new QPushButton(tr("Réinitialiser"), cisaillement);
    connect(boutonReinitialiserCisaillementSynchronise, SIGNAL(clicked()), this, SLOT(reinitialiserCisaillementSynchronise()));
    checkBoxCisaillementHorizontalFixe = new QCheckBox(tr("Fixe"), rotation);
    checkBoxCisaillementHorizontalFixe->setChecked(labyrinth->getModeLabyrinthe().cisaillementHorizontalFixe);
    synchroniserCisaillement(checkBoxCisaillementSynchronise->checkState());
    QGroupBox *groupBoxCisaillementHorizontal = new QGroupBox(tr("Horizontal"), cisaillement);
    QGridLayout *gridLayoutGroupBoxCisaillementHorizontal = new QGridLayout;
    gridLayoutGroupBoxCisaillementHorizontal->addWidget(pourcentageCisaillementHorizontal, 0, 0);
    gridLayoutGroupBoxCisaillementHorizontal->addWidget(spinBoxPourcentageCisaillementHorizontal, 0, 1);
    gridLayoutGroupBoxCisaillementHorizontal->addWidget(checkBoxPourcentageCisaillementHorizontalAuHasard, 0, 2);
    gridLayoutGroupBoxCisaillementHorizontal->addWidget(boutonReinitialiserPourcentageCisaillementHorizontal, 0, 3);
    gridLayoutGroupBoxCisaillementHorizontal->addWidget(intervalleCisaillementHorizontal, 1, 0);
    gridLayoutGroupBoxCisaillementHorizontal->addWidget(spinBoxIntervalleCisaillementHorizontal, 1, 1);
    gridLayoutGroupBoxCisaillementHorizontal->addWidget(checkBoxIntervalleCisaillementHorizontalAuHasard, 1, 2);
    gridLayoutGroupBoxCisaillementHorizontal->addWidget(boutonReinitialiserIntervalleCisaillementHorizontal, 1, 3);
    gridLayoutGroupBoxCisaillementHorizontal->addWidget(checkBoxCisaillementHorizontalFixe, 2, 0, 1, 4);
    groupBoxCisaillementHorizontal->setLayout(gridLayoutGroupBoxCisaillementHorizontal);
    QGridLayout *gridLayoutCisaillement = new QGridLayout;
    gridLayoutCisaillement->addWidget(groupBoxCisaillementVertical, 0, 0, 1, 3);
    gridLayoutCisaillement->addWidget(groupBoxCisaillementHorizontal, 1, 0, 1, 3);
    gridLayoutCisaillement->addWidget(checkBoxCisaillementSynchronise, 2, 0);
    gridLayoutCisaillement->addWidget(checkBoxCisaillementSynchroniseAuHasard, 2, 1);
    gridLayoutCisaillement->addWidget(boutonReinitialiserCisaillementSynchronise, 2, 2);
    cisaillement->setLayout(gridLayoutCisaillement);
    QCheckBox *checkBoxCisaillement = new QCheckBox(cisaillement);
    checkBoxCisaillement->setChecked(labyrinth->getModeLabyrinthe().mode & QLabyrinth::Cisaillement);
    if (labyrinth->getTypeLabyrinthe() == QLabyrinth::Labyrinthe2Den3D)
        cisaillement->hide();
    else
    {
        tabWidgetModes->addTab(cisaillement, tr("&Cisaillement"));
        tabWidgetModes->tabBar()->setTabButton(3, QTabBar::LeftSide, checkBoxCisaillement);
    }

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(tabWidgetModes, 0, 0);
    gridLayout->addWidget(buttonBox, 1, 0);
    dialog->setLayout(gridLayout);

    if (dialog->exec() == QDialog::Rejected)
    {
        dialog->deleteLater();

        if (!pauseImposee)
            mettreEnPause();
        return;
    }

    QLabyrinth::ModeLabyrinthe mode;
    mode.mode = QLabyrinth::Aucun;
    if (checkBoxObscurite->isChecked())
        mode.mode |= QLabyrinth::Obscurite;
    mode.couleurObscurite = QColor(boutonCouleurObscurite->property("Couleur").toString().toUInt());
    mode.rayonObscurite = spinBoxRayonObscurite->value();
    if (checkBoxRotation->isChecked())
        mode.mode |= QLabyrinth::Rotation;
    mode.angleRotation = spinBoxAngleRotation->value();
    mode.angleRotationAuHasard = checkBoxAngleRotationAuHasard->isChecked();
    mode.intervalleRotation = spinBoxIntervalleRotation->value();
    mode.intervalleRotationAuHasard = checkBoxIntervalleRotationAuHasard->isChecked();
    mode.sensRotation = (checkBoxSensRotation->isChecked()) ? 1 : -1;
    mode.sensRotationAuHasard = checkBoxSensRotationAuHasard->isChecked();
    mode.rotationFixe = checkBoxRotationFixe->isChecked();
    if (checkBoxDistorsion->isChecked())
        mode.mode |= QLabyrinth::Distorsion;
    mode.pourcentageDistorsionVerticale = spinBoxPourcentageDistorsionVerticale->value();
    mode.pourcentageDistorsionHorizontale = spinBoxPourcentageDistorsionHorizontale->value();
    mode.pourcentageDistorsionVerticaleAuHasard = checkBoxPourcentageDistorsionVerticaleAuHasard->isChecked();
    mode.pourcentageDistorsionHorizontaleAuHasard = checkBoxPourcentageDistorsionHorizontaleAuHasard->isChecked();
    mode.intervalleDistorsionVerticale = spinBoxIntervalleDistorsionVerticale->value();
    mode.intervalleDistorsionHorizontale = spinBoxIntervalleDistorsionHorizontale->value();
    mode.intervalleDistorsionVerticaleAuHasard = checkBoxIntervalleDistorsionVerticaleAuHasard->isChecked();
    mode.intervalleDistorsionHorizontaleAuHasard = checkBoxIntervalleDistorsionHorizontaleAuHasard->isChecked();
    mode.distorsionVerticaleFixe = checkBoxDistorsionVerticaleFixe->isChecked();
    mode.distorsionHorizontaleFixe = checkBoxDistorsionHorizontaleFixe->isChecked();
    mode.distorsionSynchronisee = checkBoxDistorsionSynchronisee->isChecked();
    mode.distorsionSynchroniseeAuHasard = checkBoxDistorsionSynchroniseeAuHasard->isChecked();
    if (checkBoxCisaillement->isChecked())
        mode.mode |= QLabyrinth::Cisaillement;
    mode.pourcentageCisaillementVertical = spinBoxPourcentageCisaillementVertical->value();
    mode.pourcentageCisaillementHorizontal = spinBoxPourcentageCisaillementHorizontal->value();
    mode.pourcentageCisaillementVerticalAuHasard = checkBoxPourcentageCisaillementVerticalAuHasard->isChecked();
    mode.pourcentageCisaillementHorizontalAuHasard = checkBoxPourcentageCisaillementHorizontalAuHasard->isChecked();
    mode.intervalleCisaillementVertical = spinBoxIntervalleCisaillementVertical->value();
    mode.intervalleCisaillementHorizontal = spinBoxIntervalleCisaillementHorizontal->value();
    mode.intervalleCisaillementVerticalAuHasard = checkBoxIntervalleCisaillementVerticalAuHasard->isChecked();
    mode.intervalleCisaillementHorizontalAuHasard = checkBoxIntervalleCisaillementHorizontalAuHasard->isChecked();
    mode.cisaillementVerticalFixe = checkBoxCisaillementVerticalFixe->isChecked();
    mode.cisaillementHorizontalFixe = checkBoxCisaillementHorizontalFixe->isChecked();
    mode.cisaillementSynchronise = checkBoxCisaillementSynchronise->isChecked();
    mode.cisaillementSynchroniseAuHasard = checkBoxCisaillementSynchroniseAuHasard->isChecked();

    if (!labyrinth->getEnregistre() && labyrinth->getPartieEnCours() && !labyrinth->getPartieTerminee())
    {
        if (QMessageBox::warning(this, tr("Partie non enregistrée"),
                                 tr("La partie en cours n'est pas enregistrée.\n"
                                    "Voulez-vous vraiment faire une nouvelle partie ?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            if (!pauseImposee)
                mettreEnPause();
            return;
        }
    }

    temps = QTime(0, 0, 0);
    ms = 0;
    timer->stop();
    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(tr("hh:mm:ss"))));
    nouveau = true;
    pauseImposee = false;

    chronometre->setDisabled(false);
    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(true);
    actionResoudre->setDisabled(true);
    actionMettreEnPause->setDisabled(true);
    actionEffacerChemin->setDisabled(false);

    labyrinth->nouveau(labyrinth->getNiveau(), labyrinth->getLongueur(), labyrinth->getLargeur(),
                       labyrinth->getAlgorithme(), labyrinth->getTypeLabyrinthe(),
                       labyrinth->getFormeLabyrinthe(), &mode);

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(false);
    actionResoudre->setDisabled(false);
    actionMettreEnPause->setDisabled(false);
}

void MainWindow::changementCategorieScores()
{
    QList<QList<Score> > *scores = &scores2D;

    if (!radioButtonScores2D->isChecked())
        scores = &scores2Den3D;

    for (int i = 0; i < NOMBRESCORES; i++)
    {
        labelsNomsFacile[i]->setText(tr("%1. ").arg(i + 1));
        labelsTempsFacile[i]->setText(tr(""));
        labelsDeplacementsFacile[i]->setText(tr(""));
        labelsNomsMoyen[i]->setText(tr("%1. ").arg(i + 1));
        labelsTempsMoyen[i]->setText(tr(""));
        labelsDeplacementsMoyen[i]->setText(tr(""));
        labelsNomsDifficile[i]->setText(tr("%1. ").arg(i + 1));
        labelsTempsDifficile[i]->setText(tr(""));
        labelsDeplacementsDifficile[i]->setText(tr(""));
        if (i < (*scores)[0].size())
        {
            labelsNomsFacile[i]->setText(labelsNomsFacile[i]->text() + (*scores)[0][i].nom);
            labelsTempsFacile[i]->setText((*scores)[0][i].temps.toString(tr("hh:mm:ss")));
            labelsDeplacementsFacile[i]->setText(QString::number((*scores)[0][i].deplacement));
        }
        if (i < (*scores)[1].size())
        {
            labelsNomsMoyen[i]->setText(labelsNomsMoyen[i]->text() + (*scores)[1][i].nom);
            labelsTempsMoyen[i]->setText((*scores)[1][i].temps.toString(tr("hh:mm:ss")));
            labelsDeplacementsMoyen[i]->setText(QString::number((*scores)[1][i].deplacement));
        }
        if (i < (*scores)[2].size())
        {
            labelsNomsDifficile[i]->setText(labelsNomsDifficile[i]->text() + (*scores)[2][i].nom);
            labelsTempsDifficile[i]->setText((*scores)[2][i].temps.toString(tr("hh:mm:ss")));
            labelsDeplacementsDifficile[i]->setText(QString::number((*scores)[2][i].deplacement));
        }
    }
}

void MainWindow::reinitialiserScores()
{
    QList<QList<Score> > *scores = &scores2D;

    if (!radioButtonScores2D->isChecked())
        scores = &scores2Den3D;

    (*scores)[tabWidgetScores->currentIndex()].clear();

    if (!tabWidgetScores->currentIndex())
    {
        for (int i = 0; i < NOMBRESCORES; i++)
        {
            labelsNomsFacile[i]->setText(tr("%1. ").arg(i + 1));
            labelsTempsFacile[i]->setText(tr(""));
            labelsDeplacementsFacile[i]->setText(tr(""));
        }
    }
    else if (tabWidgetScores->currentIndex() == 1)
    {
        for (int i = 0; i < NOMBRESCORES; i++)
        {
            labelsNomsMoyen[i]->setText(tr("%1. ").arg(i + 1));
            labelsTempsMoyen[i]->setText(tr(""));
            labelsDeplacementsMoyen[i]->setText(tr(""));
        }
    }
    else
    {
        for (int i = 0; i < NOMBRESCORES; i++)
        {
            labelsNomsDifficile[i]->setText(tr("%1. ").arg(i + 1));
            labelsTempsDifficile[i]->setText(tr(""));
            labelsDeplacementsDifficile[i]->setText(tr(""));
        }
    }
}

void MainWindow::miseEnPage()
{
    if (!pauseImposee)
        mettreEnPause();

    QPageSetupDialog pageSetup(printer, this);

    pageSetup.exec();

    if (!pauseImposee)
        mettreEnPause();
}
void MainWindow::configurerFormats()
{
    QPageSetupDialog pageSetup(printer, this);

    pageSetup.exec();

    if (checkBoxFormats->isChecked())
        stateChangedFormats(Qt::Checked);
}

void MainWindow::apercuLabyrinthe(QPrinter *p)
{
    labyrinth->apercu(p);
}

void MainWindow::reinitialiserMusique()
{
    lineEditMusique->setText(MUSIQUE);
    checkBoxMuet->setChecked(MUET);
}

void MainWindow::parcourirMusique()
{
    QString s = lineEditMusique->text();
    if (!QFileInfo(s).exists())
        s = QApplication::applicationDirPath();
    QString texte = QFileDialog::getOpenFileName(this, tr("Choisir une musique"), s,
                                                 tr("Musiques (*.wav *.ogg *.mp3 *.wma *.mid)"));

    if (texte.isNull())
        return;

    lineEditMusique->setText(texte);
}

void MainWindow::afficherTrace()
{
    labyrinth->setAfficherTrace(actionAfficherTrace->isChecked());
}

void MainWindow::effacerChemin()
{
    labyrinth->setEffacerChemin(actionEffacerChemin->isChecked());
}

void MainWindow::changerEnregistrement()
{
    setWindowTitle(((labyrinth->getEnregistre()) ? QString() : QString("*")) + tr("%1 - Labyrinthe").arg(nomPartie));

    actionEnregistrer->setDisabled(labyrinth->getEnregistre());
}

void MainWindow::pause()
{
    pauseImposee = actionMettreEnPause->isChecked();

    if (pauseImposee)
        actionMettreEnPause->setText(tr("Repren&dre"));
    else
        actionMettreEnPause->setText(tr("&Mettre en pause"));

    mettreEnPause();
}

void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);

    if (event->type() == QEvent::WindowStateChange)
    {
        Qt::WindowStates stateWindow = dynamic_cast<QWindowStateChangeEvent *>(event)->oldState();

        if (stateWindow != windowState())
            if (((stateWindow & Qt::WindowFullScreen) && !(windowState() & Qt::WindowFullScreen))
                || (!(stateWindow & Qt::WindowFullScreen) && (windowState() & Qt::WindowFullScreen)))
                actionPleinEcran->setChecked(isFullScreen());
    }
    else if (event->type() == QEvent::LanguageChange)
        actualiserLangue();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->setAccepted(false);

    quitter();
}

void MainWindow::type()
{
    if (!labyrinth->getEnregistre() && labyrinth->getPartieEnCours() && !labyrinth->getPartieTerminee())
    {
        if (!pauseImposee)
            mettreEnPause();

        if (QMessageBox::warning(this, tr("Partie non enregistrée"),
                                 tr("La partie en cours n'est pas enregistrée.\n"
                                    "Voulez-vous vraiment faire une nouvelle partie ?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            if (!pauseImposee)
                mettreEnPause();
            if (labyrinth->getTypeLabyrinthe() == QLabyrinth::Labyrinthe2D)
                actionLabyrinthe2D->setChecked(true);
            else// if (labyrinth->getTypeLabyrinthe() == QLabyrinth::Labyrinthe2Den3D)
                actionLabyrinthe2Den3D->setChecked(true);
            return;
        }
    }

    temps = QTime(0, 0, 0);
    ms = 0;
    timer->stop();
    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(tr("hh:mm:ss"))));
    nouveau = true;

    chronometre->setDisabled(false);
    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(true);
    actionResoudre->setDisabled(true);
    actionMettreEnPause->setDisabled(true);
    actionEffacerChemin->setDisabled(false);

    QLabyrinth::TypeLabyrinthe type = QLabyrinth::Labyrinthe2D;

    if (actionLabyrinthe2Den3D->isChecked())
        type = QLabyrinth::Labyrinthe2Den3D;

    labyrinth->nouveau(labyrinth->getNiveau(), labyrinth->getLongueur(),
                       labyrinth->getLargeur(), labyrinth->getAlgorithme(),
                       type, labyrinth->getFormeLabyrinthe(), 0);

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(false);
    actionResoudre->setDisabled(false);
    actionMettreEnPause->setDisabled(false);
}

void MainWindow::algorithmeGeneration()
{
    if (!labyrinth->getEnregistre() && labyrinth->getPartieEnCours() && !labyrinth->getPartieTerminee())
    {
        if (!pauseImposee)
            mettreEnPause();

        if (QMessageBox::warning(this, tr("Partie non enregistrée"),
                                 tr("La partie en cours n'est pas enregistrée.\n"
                                    "Voulez-vous vraiment faire une nouvelle partie ?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            if (!pauseImposee)
                mettreEnPause();
            actionsAlgorithmesGeneration[int(labyrinth->getAlgorithme())]->setChecked(true);
            return;
        }
    }

    temps = QTime(0, 0, 0);
    ms = 0;
    timer->stop();
    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(tr("hh:mm:ss"))));
    nouveau = true;

    chronometre->setDisabled(false);
    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(true);
    actionResoudre->setDisabled(true);
    actionMettreEnPause->setDisabled(true);
    actionEffacerChemin->setDisabled(false);

    labyrinth->nouveau(labyrinth->getNiveau(), labyrinth->getLongueur(),
                       labyrinth->getLargeur(),
                       QLabyrinth::Algorithme(actionsAlgorithmesGeneration.indexOf(qobject_cast<QAction *>(sender()))),
                       labyrinth->getTypeLabyrinthe(), labyrinth->getFormeLabyrinthe(), 0);

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(false);
    actionResoudre->setDisabled(false);
    actionMettreEnPause->setDisabled(false);
}

void MainWindow::algorithmeResolution()
{
    unsigned int typeResolution{0};

    if (qobject_cast<QAction*>(sender())->text() == tr("&A-Star"))
        typeResolution = 0;
    else if (qobject_cast<QAction*>(sender())->text() == tr("Main &droite au mur"))
        typeResolution = 1;
    else if (qobject_cast<QAction*>(sender())->text() == tr("Main &gauche au mur"))
        typeResolution = 2;
    else if (qobject_cast<QAction*>(sender())->text() == tr("A&veugle"))
        typeResolution = 3;

    labyrinth->setTypeResolution(typeResolution);
}

void MainWindow::reinitialiserModes()
{
    switch (tabWidgetModes->currentIndex())
    {
        case 0:
            reinitialiserCouleurObscurite();
            reinitialiserRayonObscurite();
            break;
        case 1:
            reinitialiserAngleRotation();
            reinitialiserIntervalleRotation();
            reinitialiserSensRotation();
            checkBoxRotationFixe->setChecked(ROTATIONFIXE);
            break;
        case 2:
            reinitialiserPourcentageDistorsionVerticale();
            reinitialiserIntervalleDistorsionVerticale();
            reinitialiserPourcentageDistorsionHorizontale();
            reinitialiserIntervalleDistorsionHorizontale();
            reinitialiserDistorsionSynchronisee();
            checkBoxDistorsionVerticaleFixe->setChecked(DISTORSIONVERTICALEFIXE);
            checkBoxDistorsionHorizontaleFixe->setChecked(DISTORSIONHORIZONTALEFIXE);
            break;
        case 3:
            reinitialiserPourcentageCisaillementVertical();
            reinitialiserIntervalleCisaillementVertical();
            reinitialiserPourcentageCisaillementHorizontal();
            reinitialiserIntervalleCisaillementHorizontal();
            reinitialiserCisaillementSynchronise();
            checkBoxCisaillementVerticalFixe->setChecked(CISAILLEMENTVERTICALFIXE);
            checkBoxCisaillementHorizontalFixe->setChecked(CISAILLEMENTHORIZONTALFIXE);
            break;
        default:
            break;
    }
}

void MainWindow::choisirCouleurObscurite()
{
    QColor couleur = QColorDialog::getColor(QColor(boutonCouleurObscurite->property("Couleur").toString().toUInt()),
                                            boutonCouleurObscurite, tr("Choisir une couleur de fond"));

    if (!couleur.isValid())
        return;

    QPixmap pixmapObscurite(32, 32);
    pixmapObscurite.fill(couleur);
    boutonCouleurObscurite->setIcon(QIcon(pixmapObscurite));
    boutonCouleurObscurite->setProperty("Couleur", QString::number(couleur.rgba()));
}

void MainWindow::reinitialiserCouleurObscurite()
{
    QPixmap pixmapObscurite(32, 32);
    pixmapObscurite.fill(COULEUROBSCURITE);
    boutonCouleurObscurite->setIcon(QIcon(pixmapObscurite));
    boutonCouleurObscurite->setProperty("Couleur", QString::number(COULEUROBSCURITE.rgba()));
}

void MainWindow::reinitialiserRayonObscurite()
{
    spinBoxRayonObscurite->setValue(RAYONOBSCURITE);
}

void MainWindow::reinitialiserAngleRotation()
{
    spinBoxAngleRotation->setValue(ANGLEROTATION);
    checkBoxAngleRotationAuHasard->setChecked(ANGLEROTATIONAUHASARD);
}

void MainWindow::reinitialiserIntervalleRotation()
{
    spinBoxIntervalleRotation->setValue(INTERVALLEROTATION);
    checkBoxIntervalleRotationAuHasard->setChecked(INTERVALLEROTATIONAUHASARD);
}

void MainWindow::reinitialiserSensRotation()
{
    checkBoxSensRotation->setChecked(SENSROTATION == 1);
    checkBoxSensRotationAuHasard->setChecked(SENSROTATIONAUHASARD);
}

void MainWindow::reinitialiserPourcentageDistorsionVerticale()
{
    spinBoxPourcentageDistorsionVerticale->setValue(POURCENTAGEDISTORSIONVERTICALE);
    checkBoxPourcentageDistorsionVerticaleAuHasard->setChecked(INTERVALLEDISTORSIONVERTICALEAUHASARD);
}

void MainWindow::reinitialiserPourcentageDistorsionHorizontale()
{
    spinBoxPourcentageDistorsionHorizontale->setValue(POURCENTAGEDISTORSIONHORIZONTALE);
    checkBoxPourcentageDistorsionHorizontaleAuHasard->setChecked(INTERVALLEDISTORSIONHORIZONTALEAUHASARD);
}

void MainWindow::reinitialiserIntervalleDistorsionVerticale()
{
    spinBoxIntervalleDistorsionVerticale->setValue(INTERVALLEDISTORSIONVERTICALE);
    checkBoxIntervalleDistorsionVerticaleAuHasard->setChecked(INTERVALLEDISTORSIONVERTICALEAUHASARD);
}

void MainWindow::reinitialiserIntervalleDistorsionHorizontale()
{
    spinBoxIntervalleDistorsionHorizontale->setValue(INTERVALLEDISTORSIONHORIZONTALE);
    checkBoxIntervalleDistorsionHorizontaleAuHasard->setChecked(INTERVALLEDISTORSIONHORIZONTALEAUHASARD);
}

void MainWindow::reinitialiserDistorsionSynchronisee()
{
    checkBoxDistorsionSynchronisee->setChecked(DISTORSIONSYNCHRONISEE);
    checkBoxDistorsionSynchroniseeAuHasard->setChecked(DISTORSIONSYNCHRONISEEAUHASARD);
}

void MainWindow::reinitialiserPourcentageCisaillementVertical()
{
    spinBoxPourcentageCisaillementVertical->setValue(POURCENTAGECISAILLEMENTVERTICAL);
    checkBoxPourcentageCisaillementVerticalAuHasard->setChecked(INTERVALLECISAILLEMENTVERTICALAUHASARD);
}

void MainWindow::reinitialiserPourcentageCisaillementHorizontal()
{
    spinBoxPourcentageCisaillementHorizontal->setValue(POURCENTAGECISAILLEMENTHORIZONTAL);
    checkBoxPourcentageCisaillementHorizontalAuHasard->setChecked(INTERVALLECISAILLEMENTHORIZONTALAUHASARD);
}

void MainWindow::reinitialiserIntervalleCisaillementVertical()
{
    spinBoxIntervalleCisaillementVertical->setValue(INTERVALLECISAILLEMENTVERTICAL);
    checkBoxIntervalleCisaillementVerticalAuHasard->setChecked(INTERVALLECISAILLEMENTVERTICALAUHASARD);
}

void MainWindow::reinitialiserIntervalleCisaillementHorizontal()
{
    spinBoxIntervalleCisaillementHorizontal->setValue(INTERVALLECISAILLEMENTHORIZONTAL);
    checkBoxIntervalleCisaillementHorizontalAuHasard->setChecked(INTERVALLECISAILLEMENTHORIZONTALAUHASARD);
}

void MainWindow::reinitialiserCisaillementSynchronise()
{
    checkBoxCisaillementSynchronise->setChecked(CISAILLEMENTSYNCHRONISE);
    checkBoxCisaillementSynchroniseAuHasard->setChecked(CISAILLEMENTSYNCHRONISEAUHASARD);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::FileOpen)
    {
        chargerPartie(((QFileOpenEvent *)event)->file());
        return true;
    }
    else
        return QObject::eventFilter(obj, event);
}

void MainWindow::chargerPartie(const QString &fichier)
{
    if (fichier == QString("/S") || fichier == QString("/s"))//Lancement de l'écran de veille
    {
        audioOutput->setMuted(true);
        actionPleinEcran->setChecked(false);
        actionLabyrintheSeulement->setChecked(false);
        actionResolutionProgressive->setChecked(false);
        actionPleinEcran->trigger();
        actionLabyrintheSeulement->trigger();
        actionResolutionProgressive->trigger();
        labyrinth->passerEnModeEcranDeVeille();

        return;
    }
    else if (fichier == QString("/p"))//Aperçu de l'écran de veille
    {
        hide();
        QTimer::singleShot(0, qApp, SLOT(quit()));
        return;
    }
    else if (fichier.startsWith(QString("/c")))//Paramétrage de l'écran de veille
    {
        hide();
        QTimer::singleShot(0, qApp, SLOT(quit()));
        return;
    }

    QFile f(fichier);

    if (f.open(QFile::ReadOnly))
    {
        mettreEnPause();

        fichierEnregistrement = fichier;
        nomPartie = QFileInfo(fichier).baseName();

        temps = QTime(0, 0, 0);
        ms = 0;
        timer->stop();
        bool chrono, muet, labySeulement;
        QString s;

        QDataStream data(&f);
        data.setVersion(QDataStream::Qt_4_7);

        //data >> labyrinthe 2D (ou 2D en 3D) ou 3D;

        labyrinth->charger(data, chrono, ms, s, muet, labySeulement, adaptationEcran, adaptationFormats);

        f.close();

        chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(tr("hh:mm:ss"))));
        nouveau = false;

        QString n;

        if (labyrinth->getNiveau() == QLabyrinth::Facile)
        {
            n = tr("Facile");
            actionFacile->setChecked(true);
        }
        else if (labyrinth->getNiveau() == QLabyrinth::Moyen)
        {
            n = tr("Moyen");
            actionMoyen->setChecked(true);
        }
        else if (labyrinth->getNiveau() == QLabyrinth::Difficile)
        {
            n = tr("Difficile");
            actionDifficile->setChecked(true);
        }
        else
        {
            n = tr("Personnalisé");
            actionPersonnalise->setChecked(true);
        }

        niveau->setText(tr("Niveau : %1 (%2x%3)").arg(n).arg(labyrinth->getLongueur()).arg(labyrinth->getLargeur()));
        deplacement->setText(tr("Déplacement : %1").arg(labyrinth->getNombreDeplacement()));

        chronometre->setDisabled(labyrinth->getPartieEnCours());
        chronometre->setChecked(chrono);
        for (auto& a : actionsAlgorithmesResolution)
            a->setDisabled(labyrinth->getEnResolution());
        actionResoudre->setDisabled(labyrinth->getEnResolution());
        actionMettreEnPause->setDisabled(labyrinth->getPartieTerminee());
        actionMettreEnPause->setChecked(labyrinth->getPartieEnPause());
        pauseImposee = labyrinth->getPartieEnPause();
        actionEffacerChemin->setDisabled(labyrinth->getPartieEnCours());
        actionEffacerChemin->setChecked(labyrinth->getEffacerChemin());
        actionsAlgorithmesResolution[labyrinth->getTypeResolution()]->setChecked(true);
        actionResolutionProgressive->setChecked(labyrinth->getResolutionProgressive());
        actionLabyrintheSeulement->setChecked(labySeulement);
        if (labyrinth->getTypeLabyrinthe() == QLabyrinth::Labyrinthe2D)
            actionLabyrinthe2D->setChecked(true);
        else// if (labyrinth->getTypeLabyrinthe() == QLabyrinth::Labyrinthe2Den3D)
            actionLabyrinthe2Den3D->setChecked(true);
        if (labySeulement)
            labyrintheSeulement();

        if (emplacementMusique != s)
        {
            emplacementMusique = s;
            mediaPlayer->setSource(emplacementMusique);

            mediaPlayer->stop();

            audioOutput->setMuted(muet);

            if (QFileInfo(emplacementMusique).exists())
                mediaPlayer->play();
        }

        if (chronometre->isChecked() && labyrinth->getPartieEnCours() && !pauseImposee)
        {
            timer->start();
            elapsedTimer.start();
        }

        labyrinth->activer();
    }
    else
    {
        QMessageBox::warning(this, tr("Impossible de charger la partie"),
                             tr("Il est impossible d'ouvrir le fichier pour charger la partie."), QMessageBox::Ok);

        if (!labyrinth->getEnregistre() && labyrinth->getPartieEnCours() && !pauseImposee)
            mettreEnPause();
    }
}

void MainWindow::actualiserLangue()
{
    QString locale;

    switch (langueChoisie)
    {
        case Systeme:
        default:
            locale = QLocale::system().name().section('_', 0, 0);
            if (locale != QString("fr") && locale != QString("en") && locale != QString("es")
                && locale != QString("de") && locale != QString("it"))
                locale = QString("en");
            break;
        case Francais:
            locale = QString("fr");
            break;
        case Anglais:
            locale = QString("en");
            break;
        case Espagnol:
            locale = QString("es");
            break;
        case Allemand:
            locale = QString("de");
            break;
        case Italien:
            locale = QString("it");
            break;
    }

    qApp->removeTranslator(translatorLabyrinthe);
    qApp->removeTranslator(translatorQt);
    qApp->removeTranslator(translatorQtBase);
    translatorLabyrinthe->load(QString(":/Traductions/Labyrinth_") + locale);
    translatorQt->load(QString(":/Traductions/qt_") + locale);
    translatorQtBase->load(QString(":/Traductions/qtbase_") + locale);
    qApp->installTranslator(translatorLabyrinthe);
    qApp->installTranslator(translatorQt);
    qApp->installTranslator(translatorQtBase);
}

void MainWindow::langue()
{
    if (!pauseImposee)
        mettreEnPause();

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Langue"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset, Qt::Horizontal, dialog);
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), dialog, SLOT(reject()));
    connect(buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reinitialiserLangue()));

    QLabel *label = new QLabel(tr("Langue :"), dialog);
    QLabel *label2 = new QLabel(tr("Le redémarrage de l'application est nécessaire."), dialog);
    comboBoxLangue = new QComboBox(dialog);
    comboBoxLangue->addItems(QStringList() << tr("Système") << tr("Français") << tr("Anglais")
                                           << tr("Espagnol") << tr("Allemand") << tr("Italien"));
    comboBoxLangue->setCurrentIndex(int(langueChoisie));

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(label, 0, 0);
    gridLayout->addWidget(comboBoxLangue, 0, 1);
    gridLayout->addWidget(label2, 1, 0, 1, 2);
    gridLayout->addWidget(buttonBox, 2, 0, 1, 2);

    dialog->setLayout(gridLayout);

    if (dialog->exec() == QDialog::Rejected)
    {
        dialog->deleteLater();

        if (!pauseImposee)
            mettreEnPause();
        return;
    }

    langueChoisie = Langue(comboBoxLangue->currentIndex());

    if (!pauseImposee)
        mettreEnPause();
}

void MainWindow::reinitialiserLangue()
{
    comboBoxLangue->setCurrentIndex(0);
}

void MainWindow::actualiserDeplacement()
{
    deplacement->setText(tr("Déplacement : %1").arg(labyrinth->getNombreDeplacement()));
}

QString MainWindow::getNomPartie() const
{
    return nomPartie;
}

void MainWindow::synchroniserDistorsion(int state)
{
    if (state == Qt::Checked)
    {
        connect(spinBoxPourcentageDistorsionVerticale, SIGNAL(valueChanged(int)), spinBoxPourcentageDistorsionHorizontale, SLOT(setValue(int)));
        connect(spinBoxPourcentageDistorsionHorizontale, SIGNAL(valueChanged(int)), spinBoxPourcentageDistorsionVerticale, SLOT(setValue(int)));
        connect(checkBoxPourcentageDistorsionVerticaleAuHasard, SIGNAL(toggled(bool)), checkBoxPourcentageDistorsionHorizontaleAuHasard, SLOT(setChecked(bool)));
        connect(checkBoxPourcentageDistorsionHorizontaleAuHasard, SIGNAL(toggled(bool)), checkBoxPourcentageDistorsionVerticaleAuHasard, SLOT(setChecked(bool)));
        connect(spinBoxIntervalleDistorsionVerticale, SIGNAL(valueChanged(int)), spinBoxIntervalleDistorsionHorizontale, SLOT(setValue(int)));
        connect(spinBoxIntervalleDistorsionHorizontale, SIGNAL(valueChanged(int)), spinBoxIntervalleDistorsionVerticale, SLOT(setValue(int)));
        connect(checkBoxIntervalleDistorsionVerticaleAuHasard, SIGNAL(toggled(bool)), checkBoxIntervalleDistorsionHorizontaleAuHasard, SLOT(setChecked(bool)));
        connect(checkBoxIntervalleDistorsionHorizontaleAuHasard, SIGNAL(toggled(bool)), checkBoxIntervalleDistorsionVerticaleAuHasard, SLOT(setChecked(bool)));
        connect(checkBoxDistorsionVerticaleFixe, SIGNAL(toggled(bool)), checkBoxDistorsionHorizontaleFixe, SLOT(setChecked(bool)));
        connect(checkBoxDistorsionHorizontaleFixe, SIGNAL(toggled(bool)), checkBoxDistorsionVerticaleFixe, SLOT(setChecked(bool)));
    }
    else
    {
        disconnect(spinBoxPourcentageDistorsionVerticale, SIGNAL(valueChanged(int)), spinBoxPourcentageDistorsionHorizontale, SLOT(setValue(int)));
        disconnect(spinBoxPourcentageDistorsionHorizontale, SIGNAL(valueChanged(int)), spinBoxPourcentageDistorsionVerticale, SLOT(setValue(int)));
        disconnect(checkBoxPourcentageDistorsionVerticaleAuHasard, SIGNAL(toggled(bool)), checkBoxPourcentageDistorsionHorizontaleAuHasard, SLOT(setChecked(bool)));
        disconnect(checkBoxPourcentageDistorsionHorizontaleAuHasard, SIGNAL(toggled(bool)), checkBoxPourcentageDistorsionVerticaleAuHasard, SLOT(setChecked(bool)));
        disconnect(spinBoxIntervalleDistorsionVerticale, SIGNAL(valueChanged(int)), spinBoxIntervalleDistorsionHorizontale, SLOT(setValue(int)));
        disconnect(spinBoxIntervalleDistorsionHorizontale, SIGNAL(valueChanged(int)), spinBoxIntervalleDistorsionVerticale, SLOT(setValue(int)));
        disconnect(checkBoxIntervalleDistorsionVerticaleAuHasard, SIGNAL(toggled(bool)), checkBoxIntervalleDistorsionHorizontaleAuHasard, SLOT(setChecked(bool)));
        disconnect(checkBoxIntervalleDistorsionHorizontaleAuHasard, SIGNAL(toggled(bool)), checkBoxIntervalleDistorsionVerticaleAuHasard, SLOT(setChecked(bool)));
        disconnect(checkBoxDistorsionVerticaleFixe, SIGNAL(toggled(bool)), checkBoxDistorsionHorizontaleFixe, SLOT(setChecked(bool)));
        disconnect(checkBoxDistorsionHorizontaleFixe, SIGNAL(toggled(bool)), checkBoxDistorsionVerticaleFixe, SLOT(setChecked(bool)));
    }
}

void MainWindow::synchroniserCisaillement(int state)
{
    if (state == Qt::Checked)
    {
        connect(spinBoxPourcentageCisaillementVertical, SIGNAL(valueChanged(int)), spinBoxPourcentageCisaillementHorizontal, SLOT(setValue(int)));
        connect(spinBoxPourcentageCisaillementHorizontal, SIGNAL(valueChanged(int)), spinBoxPourcentageCisaillementVertical, SLOT(setValue(int)));
        connect(checkBoxPourcentageCisaillementVerticalAuHasard, SIGNAL(toggled(bool)), checkBoxPourcentageCisaillementHorizontalAuHasard, SLOT(setChecked(bool)));
        connect(checkBoxPourcentageCisaillementHorizontalAuHasard, SIGNAL(toggled(bool)), checkBoxPourcentageCisaillementVerticalAuHasard, SLOT(setChecked(bool)));
        connect(spinBoxIntervalleCisaillementVertical, SIGNAL(valueChanged(int)), spinBoxIntervalleCisaillementHorizontal, SLOT(setValue(int)));
        connect(spinBoxIntervalleCisaillementHorizontal, SIGNAL(valueChanged(int)), spinBoxIntervalleCisaillementVertical, SLOT(setValue(int)));
        connect(checkBoxIntervalleCisaillementVerticalAuHasard, SIGNAL(toggled(bool)), checkBoxIntervalleCisaillementHorizontalAuHasard, SLOT(setChecked(bool)));
        connect(checkBoxIntervalleCisaillementHorizontalAuHasard, SIGNAL(toggled(bool)), checkBoxIntervalleCisaillementVerticalAuHasard, SLOT(setChecked(bool)));
        connect(checkBoxCisaillementVerticalFixe, SIGNAL(toggled(bool)), checkBoxCisaillementHorizontalFixe, SLOT(setChecked(bool)));
        connect(checkBoxCisaillementHorizontalFixe, SIGNAL(toggled(bool)), checkBoxCisaillementVerticalFixe, SLOT(setChecked(bool)));
    }
    else
    {
        disconnect(spinBoxPourcentageCisaillementVertical, SIGNAL(valueChanged(int)), spinBoxPourcentageCisaillementHorizontal, SLOT(setValue(int)));
        disconnect(spinBoxPourcentageCisaillementHorizontal, SIGNAL(valueChanged(int)), spinBoxPourcentageCisaillementVertical, SLOT(setValue(int)));
        disconnect(checkBoxPourcentageCisaillementVerticalAuHasard, SIGNAL(toggled(bool)), checkBoxPourcentageCisaillementHorizontalAuHasard, SLOT(setChecked(bool)));
        disconnect(checkBoxPourcentageCisaillementHorizontalAuHasard, SIGNAL(toggled(bool)), checkBoxPourcentageCisaillementVerticalAuHasard, SLOT(setChecked(bool)));
        disconnect(spinBoxIntervalleCisaillementVertical, SIGNAL(valueChanged(int)), spinBoxIntervalleCisaillementHorizontal, SLOT(setValue(int)));
        disconnect(spinBoxIntervalleCisaillementHorizontal, SIGNAL(valueChanged(int)), spinBoxIntervalleCisaillementVertical, SLOT(setValue(int)));
        disconnect(checkBoxIntervalleCisaillementVerticalAuHasard, SIGNAL(toggled(bool)), checkBoxIntervalleCisaillementHorizontalAuHasard, SLOT(setChecked(bool)));
        disconnect(checkBoxIntervalleCisaillementHorizontalAuHasard, SIGNAL(toggled(bool)), checkBoxIntervalleCisaillementVerticalAuHasard, SLOT(setChecked(bool)));
        disconnect(checkBoxCisaillementVerticalFixe, SIGNAL(toggled(bool)), checkBoxCisaillementHorizontalFixe, SLOT(setChecked(bool)));
        disconnect(checkBoxCisaillementHorizontalFixe, SIGNAL(toggled(bool)), checkBoxCisaillementVerticalFixe, SLOT(setChecked(bool)));
    }
}

bool MainWindow::getAdaptationTailleEcran() const
{
    return adaptationEcran;
}

void MainWindow::setAdaptationTailleEcran(bool oui)
{
    if (adaptationEcran == oui)
        return;

    adaptationEcran = oui;

    int longueur = QGuiApplication::screenAt(pos())->size().width() / labyrinth->getTailleCase().width();
    if (!(longueur % 2))
    {
        longueur++;
        if (longueur * labyrinth->getTailleCase().width() > QGuiApplication::screenAt(pos())->size().width())
            longueur -= 2;
    }
    int largeur = QGuiApplication::screenAt(pos())->size().height() / labyrinth->getTailleCase().height();
    if (!(largeur % 2))
    {
        largeur++;
        if (largeur * labyrinth->getTailleCase().height() > QGuiApplication::screenAt(pos())->size().height())
            largeur -= 2;
    }

    labyrinth->nouveau(QLabyrinth::Personnalise, longueur, largeur, labyrinth->getAlgorithme(),
                       labyrinth->getTypeLabyrinthe(), labyrinth->getFormeLabyrinthe(), 0);

    QString n = tr("Personnalisé");

    if (labyrinth->getNiveau() == QLabyrinth::Facile)
    {
        n = tr("Facile");
        actionFacile->setChecked(true);
    }
    else if (labyrinth->getNiveau() == QLabyrinth::Moyen)
    {
        n = tr("Moyen");
        actionMoyen->setChecked(true);
    }
    else if (labyrinth->getNiveau() == QLabyrinth::Difficile)
    {
        n = tr("Difficile");
        actionDifficile->setChecked(true);
    }

    niveau->setText(tr("Niveau : %1 (%2x%3)").arg(n).arg(labyrinth->getLongueur()).arg(labyrinth->getLargeur()));

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(false);
    actionResoudre->setDisabled(false);
    actionMettreEnPause->setDisabled(false);

    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(tr("hh:mm:ss"))));

    if (labyrinth->getLongueur() * labyrinth->getLargeur() > LONGUEURDIFFICILE * LARGEURDIFFICILE
        && !labyrinth->enModeEcranDeVeille())
    {
        labyrinth->setResolutionProgressive(false);
        actionResolutionProgressive->setChecked(false);
    }

    labyrinth->activer();
}

bool MainWindow::getAdaptationTaillePapier() const
{
    return adaptationFormats;
}

void MainWindow::setAdaptationTaillePapier(bool oui)
{
    if (adaptationFormats == oui)
        return;

    adaptationFormats = oui;

    temps = QTime(0, 0, 0);
    ms = 0;
    timer->stop();
    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(tr("hh:mm:ss"))));
    deplacement->setText(tr("Déplacement : %1").arg(0));
    nouveau = true;
    pauseImposee = false;
    nomPartie = tr("Partie");
    setWindowTitle(((labyrinth->getEnregistre()) ? QString() : QString("*")) + tr("%1 - Labyrinthe").arg(nomPartie));

    chronometre->setDisabled(false);
    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(true);
    actionResoudre->setDisabled(true);
    actionMettreEnPause->setDisabled(true);
    actionEffacerChemin->setDisabled(false);

    int longueurOrigine = 0;
    int largeurOrigine = 0;

    if (printer->pageRect(QPrinter::Millimeter).width() > printer->pageRect(QPrinter::Millimeter).height())
    {
        longueurOrigine = printer->pageRect(QPrinter::Millimeter).width();
        largeurOrigine = printer->pageRect(QPrinter::Millimeter).height();
    }
    else
    {
        longueurOrigine = printer->pageRect(QPrinter::Millimeter).height();
        largeurOrigine = printer->pageRect(QPrinter::Millimeter).width();
    }

    int longueur = longueurOrigine / labyrinth->getTailleCase().width();
    int largeur = largeurOrigine / labyrinth->getTailleCase().height();

    if (!(longueur % 2))
    {
        longueur++;
        if (longueur * labyrinth->getTailleCase().width() > longueurOrigine)
            longueur -= 2;
    }
    if (!(largeur % 2))
    {
        largeur++;
        if (largeur * labyrinth->getTailleCase().height() > largeurOrigine)
            largeur -= 2;
    }

    labyrinth->nouveau(QLabyrinth::Personnalise, longueur, largeur, labyrinth->getAlgorithme(),
                       labyrinth->getTypeLabyrinthe(), labyrinth->getFormeLabyrinthe(), 0);

    QString n = tr("Personnalisé");

    if (labyrinth->getNiveau() == QLabyrinth::Facile)
    {
        n = tr("Facile");
        actionFacile->setChecked(true);
    }
    else if (labyrinth->getNiveau() == QLabyrinth::Moyen)
    {
        n = tr("Moyen");
        actionMoyen->setChecked(true);
    }
    else if (labyrinth->getNiveau() == QLabyrinth::Difficile)
    {
        n = tr("Difficile");
        actionDifficile->setChecked(true);
    }

    niveau->setText(tr("Niveau : %1 (%2x%3)").arg(n).arg(labyrinth->getLongueur()).arg(labyrinth->getLargeur()));

    for (auto& a : actionsAlgorithmesResolution)
        a->setDisabled(false);
    actionResoudre->setDisabled(false);
    actionMettreEnPause->setDisabled(false);

    chronometre->setText(tr("&Chronomètre : %1").arg(temps.addMSecs(ms).toString(tr("hh:mm:ss"))));

    if (labyrinth->getLongueur() * labyrinth->getLargeur() > LONGUEURDIFFICILE * LARGEURDIFFICILE
        && !labyrinth->enModeEcranDeVeille())
    {
        labyrinth->setResolutionProgressive(false);
        actionResolutionProgressive->setChecked(false);
    }

    labyrinth->activer();
}
