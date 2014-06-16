================================================================================
    MICROSOFT FOUNDATION CLASS LIBRARY : 3Demo Project Overview
===============================================================================

The application wizard has created this 3Demo application for
you.  This application not only demonstrates the basics of using the Microsoft
Foundation Classes but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your 3Demo application.

3Demo.vcproj
    This is the main project file for VC++ projects generated using an application wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    application wizard.

3Demo.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    C3DemoApp application class.

3Demo.cpp
    This is the main application source file that contains the application
    class C3DemoApp.

3Demo.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
    Visual C++. Your project resources are in 1033.

res\3Demo.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file 3Demo.rc.

res\My3Demo.rc2
    This file contains resources that are not edited by Microsoft
    Visual C++. You should place all resources not editable by
    the resource editor in this file.

/////////////////////////////////////////////////////////////////////////////

For the main frame window:
    The project includes a standard MFC interface.

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.

res\Toolbar.bmp
    This bitmap file is used to create tiled images for the toolbar.
    The initial toolbar and status bar are constructed in the CMainFrame
    class. Edit this toolbar bitmap using the resource editor, and
    update the IDR_MAINFRAME TOOLBAR array in 3Demo.rc to add
    toolbar buttons.
/////////////////////////////////////////////////////////////////////////////

The application wizard creates one document type and one view:

3DemoDoc.h, 3DemoDoc.cpp - the document
    These files contain your C3DemoDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via C3DemoDoc::Serialize).

3DemoView.h, 3DemoView.cpp - the view of the document
    These files contain your C3DemoView class.
    C3DemoView objects are used to view C3DemoDoc objects.





/////////////////////////////////////////////////////////////////////////////

Other Features:

ActiveX Controls
    The application includes support to use ActiveX controls.

Printing and Print Preview support
    The application wizard has generated code to handle the print, print setup, and print preview
    commands by calling member functions in the CView class from the MFC library.

/////////////////////////////////////////////////////////////////////////////

Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named 3Demo.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

3Demo.manifest
	Application manifest files are used by Windows XP to describe an applications
	dependency on specific versions of Side-by-Side assemblies. The loader uses this
	information to load the appropriate assembly from the assembly cache or private
	from the application. The Application manifest  maybe included for redistribution
	as an external .manifest file that is installed in the same folder as the application
	executable or it may be included in the executable in the form of a resource.
/////////////////////////////////////////////////////////////////////////////

Other notes:

The application wizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, you will need
to redistribute the MFC DLLs. If your application is in a language
other than the operating system's locale, you will also have to
redistribute the corresponding localized resources MFC90XXX.DLL.
For more information on both of these topics, please see the section on
redistributing Visual C++ applications in MSDN documentation.

/////////////////////////////////////////////////////////////////////////////
2014��5��30��02:14:00
1.ֻ���ڱ�ըģ��Ĺ�����m_bexlosionʱ����������ʾ������
�����������Ϊ�ڱ�ը�����п������ĸ����أ�render��ʼ����û�п���
ĳ�����ء�������


2014��5��30��18:46:29
���sence1���س���ʧ�ܵ������
Ϊʲôû�м�������ϵͳ���Ѿ�������render�������أ�����stone��alphas.bmp����ͼƬ��sky.bmp
��û�������á����������Ĺؼ���Ӧ����LoadSenTex�������������ԭ��

2014��6��13��01:41:11
1.���طɻ�ģ�Ͳ��趨���η��й켣���ɻ����ʶ�ȡ�ɹ���
2.to do��pane�Ļ�����Ҫ�����������꣬����direction��ȷ����4����ֱ��ƽ��?!

2014��6��14��17:35:58
1.�����sence3�ķɻ�ģ�ͺ�����Ļ���
2.sence2��ɳĮ�������Ļ��Ƴ��������⡣
  lookat��translate���෴�Ĳ�������draw�����е�lookat�滻���ˣ����� �ɻ�ģ�ͻ��Ʋ�������
  ����ԭ���Ƿɻ���λ�ú͵��εĲ��̫���ڻ���ʱ�������draw�����ɡ�
  