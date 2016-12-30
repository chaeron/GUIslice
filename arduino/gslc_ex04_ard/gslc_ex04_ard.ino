//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 04 (Arduino): Dynamic content
//     Demonstrates push buttons, checkboxes and slider controls
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
// - To support a reasonable number of GUI elements, it is recommended to
//   use a CPU that provides more than 2KB of SRAM.
//

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"


// Defines for resources


// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT,E_ELEM_TXT_COUNT,E_ELEM_PROGRESS,
      E_ELEM_CHECK1,E_ELEM_RADIO1,E_ELEM_RADIO2,E_ELEM_SLIDER,E_ELEM_TXT_SLIDER};
enum {E_FONT_BTN,E_FONT_TXT};
enum {E_GROUP1};

bool        m_bQuit = false;

// Free-running counter for display
unsigned    m_nCount = 0;

// Instantiate the GUI
#define MAX_FONT            3
gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsXGauge               m_sXGauge;
gslc_tsXCheckbox            m_asXCheck[3];
gslc_tsXSlider              m_sXSlider;

#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    15
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];

#define MAX_STR             15


// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElem,gslc_teTouch eTouch,int nX,int nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}


// Create page elements
bool InitOverlays()
{
  gslc_tsElem*  pElem;
  
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN);

  
  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);
  
  // Create background box
  pElem = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create Quit button with text label
  static const char mstr1[] PROGMEM = "Quit";
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){160,80,80,40},(char*)mstr1,strlen_P(mstr1),E_FONT_BTN,&CbBtnQuit);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);

  // Create counter
  static const char mstr2[] PROGMEM = "Count:";
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,60,50,10},
    (char*)mstr2,strlen_P(mstr2),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);    
  static char mstr3[8] = ""; // Provide space for large counter value
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){80,60,50,10},
    mstr3,8,E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_YELLOW);

  // Create progress bar
  static const char mstr4[] PROGMEM = "Progress:";
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,80,50,10},
    (char*)mstr4,strlen_P(mstr4),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);    
  pElem = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,
    (gslc_tsRect){80,80,50,10},0,100,0,GSLC_COL_GREEN,false);
  
  // Create checkbox 1
  static const char mstr5[] PROGMEM = "Check1:";  
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,100,20,20},
    (char*)mstr5,strlen_P(mstr5),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);    
  pElem = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK1,E_PG_MAIN,&m_asXCheck[0],
    (gslc_tsRect){80,100,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_BLUE_LT2,false);

  // Create radio 1
  static const char mstr6[] PROGMEM = "Radio1:";    
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,135,20,20},
    (char*)mstr6,strlen_P(mstr6),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);    
  pElem = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO1,E_PG_MAIN,&m_asXCheck[1],
    (gslc_tsRect){80,135,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_ORANGE,false);
  gslc_ElemSetGroup(pElem,E_GROUP1);

  // Create radio 2
  static const char mstr7[] PROGMEM = "Radio2:";     
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,160,20,20},
    (char*)mstr7,strlen_P(mstr7),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);    
  pElem = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO2,E_PG_MAIN,&m_asXCheck[2],
    (gslc_tsRect){80,160,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_ORANGE,false);
  gslc_ElemSetGroup(pElem,E_GROUP1);
    
  // Create slider
  pElem = gslc_ElemXSliderCreate(&m_gui,E_ELEM_SLIDER,E_PG_MAIN,&m_sXSlider,
    (gslc_tsRect){160,140,100,20},0,100,60,5,false);
  gslc_ElemXSliderSetStyle(pElem,true,(gslc_tsColor){0,0,128},10,
          5,(gslc_tsColor){64,64,64});
  static char mstr8[15] = "Slider: ???"; // Provide space for counter value  
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_SLIDER,E_PG_MAIN,(gslc_tsRect){160,160,80,20},
    mstr8,15,E_FONT_TXT);

  return true;
}


void setup()
{
  bool                bOk = true;
  char                acTxt[MAX_STR];

  // -----------------------------------
  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // Load Fonts
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,"",1);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,"",1);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }



  // -----------------------------------
  // Start display
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // Save some element references for quick access
  gslc_tsElem*  pElemCnt        = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);
  gslc_tsElem*  pElemProgress   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS);
  gslc_tsElem*  pElemSlider     = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_SLIDER);
  gslc_tsElem*  pElemSliderTxt  = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_SLIDER);

  // -----------------------------------
  // Main event loop
  
  m_bQuit = false;
  while (!m_bQuit) {

    // General counter
    m_nCount++;

    // -----------------------------------

    // Update elements on active page
    snprintf(acTxt,MAX_STR,"%u",m_nCount);
    gslc_ElemSetTxtStr(pElemCnt,acTxt);

    gslc_ElemXGaugeUpdate(pElemProgress,((m_nCount/1)%100));
    
    // NOTE: A more efficient method is to move the following
    //       code into the slider position callback function.
    //       Please see example 07.
    int nPos = gslc_ElemXSliderGetPos(pElemSlider);  
    snprintf(acTxt,MAX_STR,"Slider: %u",nPos);
    gslc_ElemSetTxtStr(pElemSliderTxt,acTxt);
    
    // Periodically call GUIslice update function    
    gslc_Update(&m_gui);

    // Slow down updates
    delay(100);

  } // bQuit

  // Read checkbox state
  // - Either read individual checkboxes
  //   bool bCheck = gslc_ElemXCheckboxGetState(&m_gui,E_ELEM_CHECK1);
  // - Or find one in the group that was checked (eg. for radio buttons)
  //   gslc_tsElem* pElem = gslc_ElemXCheckboxFindChecked(&m_gui,GSLC_GROUP_ID_NONE);

  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

}

void loop() { }

