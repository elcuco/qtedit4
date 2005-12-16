#include <QTextBrowser>
#include <QAction>
#include <QWidget>
#include <QMessageBox>

#include "textdisplay.h"
#include "helpdisplay.h"


HelpDisplay::HelpDisplay( QString helpSource, QWidget *parent )
	:TextDisplay( NULL, parent )
{
	homeURL = helpSource;
	helpBrowser = new QTextBrowser( this );
	helpBrowser->setSource( QUrl::fromLocalFile(homeURL) );
	editor = helpBrowser;
	
//QMessageBox::warning( NULL, "go home", "working"	);

	actionBackward = new QAction( QIcon(":images/back.png"), tr("&Back"), this);
	actionBackward->setShortcut(tr("Alt+Left"));
	actionBackward->setStatusTip(tr("Go to the previous page"));
	actionBackward->setEnabled( false );
	connect( actionBackward, SIGNAL(triggered()), helpBrowser, SLOT(backward()) );
	connect( helpBrowser, SIGNAL(backwardAvailable(bool)), actionBackward, SLOT(setEnabled(bool)) );
	
	actionForward = new QAction( QIcon(":images/forward.png"), tr("&Forward"), this);
	actionForward->setShortcut(tr("Alt+Right"));
	actionForward->setStatusTip(tr("Go to the next page"));
	actionForward->setEnabled( false );
	connect( actionForward, SIGNAL(triggered()), helpBrowser, SLOT(forward()) );
	connect( helpBrowser, SIGNAL(forwardAvailable(bool)), actionForward, SLOT(setEnabled(bool)) );

	actionZoomIn = new QAction( QIcon(":images/viewmag+.png"), tr("&Zoom in"), this);
	actionZoomIn->setShortcut(tr("Ctrl++"));
	actionZoomIn->setStatusTip(tr("Zoom in the text"));
	connect( actionZoomIn, SIGNAL(triggered()), helpBrowser, SLOT(zoomIn()) );	

	actionZoomOut = new QAction( QIcon(":images/viewmag-.png"), tr("&Zoom out"), this);
	actionZoomOut->setShortcut(tr("Ctrl+-"));
	actionZoomOut->setStatusTip(tr("Zoom out the text"));
	connect( actionZoomOut, SIGNAL(triggered()), helpBrowser, SLOT(zoomOut()) );	
	
	actionGoHome = new QAction( QIcon(":images/gohome.png"), tr("&Home"), this);
	actionGoHome->setShortcut(tr("Ctrl+Home"));
	actionGoHome->setStatusTip(tr("Go to the home page"));
	connect( actionGoHome, SIGNAL(triggered()), this, SLOT(goHome()) );

	// now this is a huge hack...  we need to break the original layout
	// and then insert the same widgets (almost)
	layout->addWidget( this->editor );
	layout->addWidget( this->internalFind );
	layout->addWidget( this->internalReplace );
	layout->addWidget( this->internalGotoLine );
	
	toolbar->clear();
	toolbar->addAction( actionBackward );
	toolbar->addAction( actionForward );
	toolbar->addAction( actionGoHome );
	toolbar->addSeparator();
	toolbar->addAction( actionZoomIn );
	toolbar->addAction( actionZoomOut );
	toolbar->addSeparator();
	toolbar->addAction( actionCopy );
}

HelpDisplay::~HelpDisplay()
{
	delete helpBrowser;
	delete actionForward;
	delete actionBackward;
	editor = NULL;
}

void HelpDisplay::goHome()
{
	//
	helpBrowser->setSource( QUrl::fromLocalFile(homeURL) );
}
