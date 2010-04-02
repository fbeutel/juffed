#ifndef __JUFFED_JUFFED_H__
#define __JUFFED_JUFFED_H__

class QActionGroup;
class QMenu;

namespace Juff {
	class Project;
	class StatusLabel;
}


#include <QMap>

#include "Enums.h"
#include "JuffMW.h"
#include "DocViewer.h"
#include "DocManager.h"
#include "DocHandlerInt.h"
#include "PluginManager.h"
#include "PluginNotifier.h"


// TODO : move this to a plugin!
#include "ProjectTree.h"


class JuffEd : public Juff::PluginNotifier, public Juff::DocHandlerInt {
Q_OBJECT
public:
	JuffEd();
	virtual ~JuffEd();

	QWidget* mainWindow() const;

	// implementation of DocHandlerInt interface
	virtual Juff::Document* curDoc() const;
	virtual void openDoc(const QString&);
	virtual void closeDoc(const QString&);
	virtual void saveDoc(const QString&);
	virtual int docCount() const;
	virtual QStringList docList() const;

public slots:
	void slotFileNew();
	void slotFileOpen();
	void slotFileSave();
	void slotFileSaveAs();
	void slotFileSaveAll();
	void slotFileReload();
	void slotFileRename();
	void slotFileClose();
	void slotFileCloseAll();
	void slotFilePrint();
	void slotFileExit();


	void slotPrjNew();
	void slotPrjOpen();
	void slotPrjRename();
	void slotPrjClose();
	void slotPrjSaveAs();
	void slotPrjAddFile();

	void slotEditUndo();
	void slotEditRedo();
	void slotEditCut();
	void slotEditCopy();
	void slotEditPaste();

	void slotFind();
	void slotFindNext();
	void slotFindPrev();
	void slotReplace();
	void slotGotoLine();
	
	void slotWrapWords();
	void slotShowLineNumbers();
	void slotShowWhitespaces();
	void slotShowLineEndings();
	void slotZoomIn();
	void slotZoomOut();
	void slotZoom100();
	void slotFullscreen();
	
	void slotOpenWithCharset();
	void slotSetCharset();
	
private slots:
	void onDocModified(bool);
	void onDocCursorMoved(int, int);
	void onDocTextChanged();
	void onDocLineCountChanged(int);
	void onDocActivated(Juff::Document*);
	void onCloseRequested(bool&);

private:
	/**
	* This methods opens or activates a doc with given file 
	* name or creates a new empty doc if \param fileName is empty.
	*/
	Juff::Document* createDoc(const QString& fileName);

	void createProject(const QString& fileName);
	bool closeProject();

	QString projectName() const;
	void initDoc(Juff::Document*);
	void initCharsetMenus();
	void initPlugins();
	void loadProject();
	QString openDialogDirectory() const;
	void reportError(const QString&);
	
	/**
	* If the \param document is modified asks a question if we
	* want to save the doc.
	* Returns:
	*    true    : if the doc was closed (saved or not)
	*    false   : if the user interrupted closing
	*/
	bool closeDocWithConfirmation(Juff::Document* document);
	
	/**
	* Saves the document. The document MUST NOT be Noname - use saveDocAs instead.
	* Returns:
	*    true    : if the document was saved successfully
	*    false   : if there was an error
	*/
	bool saveDoc(Juff::Document* document);
	
	/**
	* Asks for a file name and saves the \param document with it.
	* Returns:
	*    true    : if the document was saved successfully
	*    false   : if saving failed or was interrupted
	*/
	bool saveDocAs(Juff::Document* document);
	
	/**
	* Updated menus according to current document's properties and document's type.
	*/
	void updateMenus(Juff::Document*);

	void updateGUI(Juff::Document*);
	void updateLineCount(Juff::Document*);
	void updateCursorPos(Juff::Document*);

	// fields
	Juff::Project* prj_;
	JuffMW* mw_;
	DocViewer* viewer_;
	DocManager* docManager_;
	QMap<Juff::MenuID, QMenu*> menus_;
	QMenu* charsetMenu_;
	QMenu* openWithCharsetMenu_;
	QMenu* setCharsetMenu_;
	QMenu* prjMenu_;
	QActionGroup* openWithCharsetGr_;
	QActionGroup* setCharsetGr_;
	
	Juff::StatusLabel* posL_;
	Juff::StatusLabel* nameL_;
	Juff::StatusLabel* charsetL_;
	Juff::StatusLabel* linesL_;
	
	ProjectTree* tree_;
	PluginManager pluginMgr_;
};

#endif // __JUFFED_JUFFED_H__
