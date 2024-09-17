// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "WinDinkedit.h"

#include "WinDinkeditDoc.h"
#include "LeftView.h"
#include "Globals.h"
#include "Map.h"
#include "Interface.h"
#include "SpriteLibrary.h"
#include "SpriteLibraryEntry.h"
#include "WinDinkeditView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_COMMAND(IDM_REFRESH_LIST, OnRefreshList)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_DELETE_SPRITE_ENTRY, OnDeleteSpriteEntry)
	ON_COMMAND(IDM_RENAME_SPRITE_ENTRY, OnRenameSpriteEntry)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftView construction/destruction

CLeftView::CLeftView()
{
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView drawing

void CLeftView::OnDraw(CDC* pDC)
{
	CWinDinkeditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}


void CLeftView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: You may populate your TreeView with items by directly accessing
	//  its tree control through a call to GetTreeCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CWinDinkeditDoc* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinDinkeditDoc)));
	return (CWinDinkeditDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftView message handlers

void CLeftView::UpdateVisionTree()
{
	CTreeCtrl& tree = GetTreeCtrl();

	HTREEITEM vision_num;

	// first empty the subtree out
	while((vision_num = tree.GetChildItem(visions_root)) != NULL)
	{
		tree.DeleteItem(vision_num);
	}

	char buffer[5];

	// always add 0
	wsprintf(buffer, "%d", 0);
	tree.InsertItem(buffer, visions_root);

	// add back the rest
	for (int i = 1; i < VISIONS_TRACKED; i++)
	{
		if (vision_used[i] != 0)
		{
			wsprintf(buffer, "%d", i);
			tree.InsertItem(buffer, visions_root);
		}
	}
}

void CLeftView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	keyPressed((UCHAR)nChar);
}

void CLeftView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	keyReleased((UCHAR)nChar);
}

void CLeftView::UpdateSpriteLibraryTree()
{
	CTreeCtrl& tree = GetTreeCtrl();

	HTREEITEM sprite_node;

	// first empty the subtree out
	while((sprite_node = tree.GetChildItem(sprite_library_root)) != NULL)
	{
		tree.DeleteItem(sprite_node);
	}

	char buffer[MAX_SPRITE_NAME_LENGTH];

	current_map->sprite_library.getNextInitialize();

	while (current_map->sprite_library.getNext(buffer))
	{
		tree.InsertItem(buffer, sprite_library_root);
	}
	tree.SortChildren(sprite_library_root);
}

#define MAX_C_FILENAME_LENGTH 20

void CLeftView::UpdateScriptTree()
{
	HANDLE hFind;
	WIN32_FIND_DATA dataFind;
	BOOL bMoreFiles = TRUE;

	CTreeCtrl& tree = GetTreeCtrl();

	char full_path[250];
	wsprintf(full_path, "%sstory\\*.c", current_map->dmod_path);

	char filename[MAX_C_FILENAME_LENGTH];

	hFind = FindFirstFile(full_path, &dataFind);

	// get all the *.c files in the directory
	while(hFind != INVALID_HANDLE_VALUE && bMoreFiles == TRUE)
	{
		int length = strlen(dataFind.cFileName) - 2;

		if (length >= MAX_C_FILENAME_LENGTH)
			length = MAX_C_FILENAME_LENGTH - 1;

		strncpy(filename, dataFind.cFileName, length);
		filename[length] = 0;

		// make lower
		for (int i = 1; i < MAX_C_FILENAME_LENGTH - 1; i++)
		{
			filename[i] = tolower(filename[i]);
		}

		tree.InsertItem(filename, script_root);

		bMoreFiles = FindNextFile(hFind, &dataFind);
	}

	FindClose(hFind);
}

int CLeftView::PopulateTree()
{
	CTreeCtrl& tree = GetTreeCtrl();

	tree.DeleteAllItems();

	visions_root = tree.InsertItem("Visions In Use");
	script_root = tree.InsertItem("Scripts");
	sprite_library_root = tree.InsertItem("Sprite Library");

	SetWindowLong(tree, GWL_STYLE, 
		WS_VISIBLE | WS_CHILD | TVS_HASLINES | WS_EX_CLIENTEDGE | TVS_SINGLEEXPAND);
	// TVS_HASBUTTONS | TVS_LINESATROOT

	UpdateScriptTree();
	UpdateSpriteLibraryTree();
	UpdateVisionTree();
//	tree.Expand(visions_root, TVE_EXPAND);
	tree.SelectItem(visions_root);

	tree.SortChildren(script_root);

	return true;
}

void CLeftView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (!current_map)
		return;

	// need to get selected item
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM filename = tree.GetSelectedItem();

	if (filename == NULL)
		return;

	char buffer[250];
	
	strcpy(buffer, tree.GetItemText(filename).operator LPCTSTR());

	if (script_root == tree.GetParentItem(filename))
	{
		editScript(buffer);
	}
	else if (visions_root == tree.GetParentItem(filename))
	{
		setVision(atoi(buffer));
	}
	else if (sprite_library_root == tree.GetParentItem(filename))
	{
		current_map->sprite_library.getSprite(buffer);
	}
	
	CTreeView::OnLButtonDblClk(nFlags, point);
}

void CLeftView::OnRefreshList() 
{
	PopulateTree();
}

void CLeftView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (!current_map)
		return;

	RECT location;

	GetWindowRect(&location);

	// need to get selected item
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM filename = tree.GetSelectedItem();

	if (filename == NULL)
		return;

	char buffer[250];
	
	strcpy(buffer, tree.GetItemText(filename).operator LPCTSTR());

	if (sprite_library_root == tree.GetParentItem(filename))
	{
		CMenu menuPopup;
		menuPopup.LoadMenu(IDR_SPRITE_ENTRY);
		menuPopup.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, point.x + location.left, point.y + location.top, this);

	}
	else
	{
		CMenu menuPopup;
		menuPopup.LoadMenu(IDR_SIDEBAR);
		menuPopup.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, point.x + location.left, point.y + location.top, this);
	}

	
	CTreeView::OnRButtonDown(nFlags, point);
}

void CLeftView::OnDeleteSpriteEntry() 
{
	// need to get selected item
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM filename = tree.GetSelectedItem();

	if (filename == NULL)
		return;

	char buffer[250];
	
	strcpy(buffer, tree.GetItemText(filename).operator LPCTSTR());

	current_map->sprite_library.removeSprite(buffer);	
}

void CLeftView::OnRenameSpriteEntry() 
{
	// TODO: Add your command handler code here
	// need to get selected item
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM filename = tree.GetSelectedItem();

	if (filename == NULL)
		return;

	char old_name[250];
	
	strcpy(old_name, tree.GetItemText(filename).operator LPCTSTR());


	// create the dialog box
	SpriteLibraryEntry newdlg(this);

	newdlg.m_sprite_name = old_name;

	// set the title of the dialog box
	char title_buffer[300];
	wsprintf(title_buffer, "Rename sprite entry %s", old_name);
	//newdlg.SetWindowText(title_buffer);

	// now show the dialog box
	mainWnd->GetRightPane()->gainFocus();
	int retcode = newdlg.DoModal();

	// check if ok or cancel was pressed
	if (retcode == IDOK)
	{
		char new_name[MAX_SPRITE_NAME_LENGTH];
		strcpy(new_name, newdlg.m_sprite_name.operator LPCTSTR());

		current_map->sprite_library.renameSprite(old_name, new_name);
	}
}
