//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

enum
{
	NORMAL = 0,
	SAMPLE = 1,
	ADAPT = 2
};

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;

	//Used to control the adption threshold
	Fl_Slider*			m_threSlider;
	//Used to control the depth of adaption antialiasing
	Fl_Slider*			m_antiSlider;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	Fl_Check_Button*		m_accelBox;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);
	
	int			getSize();
	int			getDepth();
	double		getThre();
	int			getAnti();
	double		getAmbient();
	bool		getAccelMode();

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	int			m_nRaytracingMethod;
	double			m_nAmbient;

	double		m_nThreshold;

	bool		m_nspotEnabled;

	bool		m_nAccelEnabled;

	//Contorl the anti alasing depth
	int			m_nAntidepth;

	void updateDepth();
	void updateThre();
	void updateAnti();
	void updateAmbient();
	void updateAccel();

// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_load_background_image(Fl_Menu_* o, void* v);
	static void cb_clear_background_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_ambientSlides(Fl_Widget* o, void* v);
	static void cb_threSlides(Fl_Widget* o, void* v);
	static void cb_accelBox(Fl_Widget* o, void* v);
	static void cb_antiSlides(Fl_Widget* o, void* v);

	


	

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);

	static void cb_RaytraceMethodChoice(Fl_Widget* o, void* v);
};

#endif
