//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"

static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_load_background_image(Fl_Menu_* o, void* v){
	TraceUI* pUI = whoami(o);

	char* newfile = fl_file_chooser("Open Image?", "*.bmp", NULL);

	if (newfile != NULL) {
		pUI->raytracer->loadBackground(newfile);
	}
}
void TraceUI::cb_clear_background_image(Fl_Menu_* o, void* v){
	TraceUI* pUI = whoami(o);
	pUI->raytracer->clearBackground();
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
	((TraceUI*)(o->user_data()))->updateDepth();
}

void TraceUI::cb_ambientSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAmbient = double(((Fl_Slider *)o)->value());
	((TraceUI*)(o->user_data()))->updateAmbient();
}

void TraceUI::cb_threSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nThreshold = double(((Fl_Slider *)o)->value());
	((TraceUI*)(o->user_data()))->updateThre();
}

void TraceUI::cb_antiSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAntidepth = int(((Fl_Slider *)o)->value());
	((TraceUI*)(o->user_data()))->updateAnti();
}

void TraceUI::cb_accelBox(Fl_Widget* o, void* v){
	((TraceUI*)(o->user_data()))->m_nAccelEnabled = char(((Fl_Check_Button *)o)->value());
	((TraceUI*)(o->user_data()))->updateAccel();
}


void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );
		((TraceUI*)(o->user_data()))->updateDepth();
		((TraceUI*)(o->user_data()))->updateThre();
		((TraceUI*)(o->user_data()))->updateAmbient();
		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				switch (((TraceUI*)(o->user_data()))->m_nRaytracingMethod)
				{
				case NORMAL:
					pUI->raytracer->tracePixel(x, y);
					break;
				case SAMPLE:
					pUI->raytracer->tracePixelSample(x, y);
					break;
				case ADAPT:
					pUI->raytracer->tracePixelAdapt(x, y);
					break;
				default:
					cout << "Choice Error, the choice box has invalid value for tracing method!" << endl;
				}
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_RaytraceMethodChoice(Fl_Widget* o, void* v)
{
	
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}
double TraceUI::getThre()
{
	return m_nThreshold;
}

int TraceUI::getAnti()
{
	return m_nAntidepth;
}


double TraceUI::getAmbient()
{
	return m_nAmbient;
}
bool TraceUI::getAccelMode()
{
	return m_nAccelEnabled;
}
void TraceUI::updateDepth(){
	raytracer->setDepth(getDepth());
}
void TraceUI::updateThre(){
	raytracer->setThre(getThre());
}
void TraceUI::updateAmbient(){
	raytracer->setAmbient(getAmbient());
}
void TraceUI::updateAccel(){
	raytracer->setAccelMode(getAccelMode());
}

void TraceUI::updateAnti(){
	raytracer->setAnti(getAnti());
}


// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...", FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image }, 
		{ "L&oad Background...", FL_ALT + 'o', (Fl_Callback *)TraceUI::cb_load_background_image },
		{ "&Clear Background...", FL_ALT + 'c', (Fl_Callback *)TraceUI::cb_clear_background_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;
	m_nThreshold = 0.0;
	m_nAmbient = 0.0;
	m_nAccelEnabled = false;
	m_nAntidepth = 1;
	m_nspotEnabled = false;
	m_nRaytracingMethod = NORMAL;
	m_mainWindow = new Fl_Window(100, 40, 320, 300, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 320, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		// install slider size
		m_threSlider = new Fl_Value_Slider(10, 80, 170, 20, "Adaption threshold");
		m_threSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_threSlider->type(FL_HOR_NICE_SLIDER);
		m_threSlider->labelfont(FL_COURIER);
		m_threSlider->labelsize(12);
		m_threSlider->minimum(0);
		m_threSlider->maximum(1);
		m_threSlider->step(0.01);
		m_threSlider->value(m_nThreshold);
		m_threSlider->align(FL_ALIGN_RIGHT);
		m_threSlider->callback(cb_threSlides);

		
		m_antiSlider = new Fl_Value_Slider(10, 165, 170, 20, "Antialiasing depth");
		m_antiSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_antiSlider->type(FL_HOR_NICE_SLIDER);
		m_antiSlider->labelfont(FL_COURIER);
		m_antiSlider->labelsize(12);
		m_antiSlider->minimum(1);
		m_antiSlider->maximum(5);
		m_antiSlider->step(1);
		m_antiSlider->value(m_nAntidepth);
		m_antiSlider->align(FL_ALIGN_RIGHT);
		m_antiSlider->callback(cb_antiSlides);
		
		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 105, 180, 20, "Ambient light");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
		m_sizeSlider->labelfont(FL_COURIER);
		m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(0.0);
		m_sizeSlider->maximum(1.0);
		m_sizeSlider->step(0.01);
		m_sizeSlider->value(m_nAmbient);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_ambientSlides);

		m_accelBox = new Fl_Check_Button(140, 130, 20, 20, "Accelarate Shading");
		m_accelBox->user_data((void*)(this));	// record self to be used by static callback functions
		m_accelBox->callback(cb_accelBox);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}