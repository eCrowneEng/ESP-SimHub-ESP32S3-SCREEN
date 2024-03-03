#ifndef __SHCUSTOMPROTOCOL_H__
#define __SHCUSTOMPROTOCOL_H__

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <map>

#define TFT_BL 2 // backlight pin

// 4827S043 - 480x270, no touch
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */,
    0 /* hsync_polarity */, 1 /* hsync_front_porch */, 1 /* hsync_pulse_width */, 43 /* hsync_back_porch */,
    0 /* vsync_polarity */, 3 /* vsync_front_porch */, 1 /* vsync_pulse_width */, 12 /* vsync_back_porch */,
    1 /* pclk_active_neg */, 10000000 /* prefer_speed */);

// https://github.com/eCrowneEng/ESP-SimHub-ESP32S3-SCREEN/issues/1
// 8048S043 - 800x480, capacitive touch
//Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
//    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
//    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
//    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
//    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */,
//    0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 16 /* hsync_back_porch */,
//    0 /* vsync_polarity */, 4 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 4 /* vsync_back_porch */,
//    1 /* pclk_active_neg */, 16000000 /* prefer_speed */);


Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    PIXEL_WIDTH /* width */, PIXEL_HEIGHT /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */);

static const int SCREEN_WIDTH = PIXEL_WIDTH;
static const int SCREEN_HEIGHT = PIXEL_HEIGHT;
static const int X_CENTER = SCREEN_WIDTH / 2;
static const int Y_CENTER = SCREEN_HEIGHT / 2;
static const int ROWS = 5;
static const int COLS = 5;
static const int CELL_WIDTH = SCREEN_WIDTH / COLS;
static const int HALF_CELL_WIDTH = CELL_WIDTH / 2;
static const int CELL_HEIGHT = SCREEN_HEIGHT / ROWS;
static const int HALF_CELL_HEIGHT = CELL_HEIGHT / 2;
static const int COL[] = {0, CELL_WIDTH, CELL_WIDTH * 2, CELL_WIDTH * 3, CELL_WIDTH * 4, CELL_WIDTH * 6, CELL_WIDTH * 7};
static const int ROW[] = {0, CELL_HEIGHT, CELL_HEIGHT * 2, CELL_HEIGHT * 3, CELL_HEIGHT * 4, CELL_HEIGHT * 6, CELL_HEIGHT * 7};

#include <GFXHelpers.h>

std::map<String, String> prevData;
std::map<String, int32_t> prevColor;

class SHCustomProtocol {
private:
	// Global variables
	int rpmPercent = 50;
	int prev_rpmPercent = 50;
	int rpmRedLineSetting = 90;
	String gear = "N";
	String prev_gear;
	String speed = "0";
	String currentLapTime = "00:00.00";
	String lastLapTime = "00:00.00";
	String bestLapTime = "00:00.00";
	String sessionBestLiveDeltaSeconds = "0.000";
	String sessionBestLiveDeltaProgressSeconds = "0.00";
	String tyrePressureFrontLeft = "00.0";
	String tyrePressureFrontRight = "00.0";
	String tyrePressureRearLeft = "00.0";
	String tyrePressureRearRight = "00.0";
	String tcLevel = "0";
	String tcActive = "0";
	String absLevel = "0";
	String absActive = "0";
	String isTCCutNull = "True";
	String tcTcCut = "0  0";
	String brakeBias = "0";
	String brake = "0";
	String lapInvalidated = "False";

	int cellTitleHeight = 0;
	bool hasReceivedData = false;
public:
	void setup() {
		gfx->begin();
	    gfx->fillScreen(BLACK);

	#ifdef TFT_BL
		pinMode(TFT_BL, OUTPUT);
		digitalWrite(TFT_BL, HIGH);
	#endif
		gfx->setTextColor(RGB565(0xff, 0x66, 0x00));
		drawCentreCentreString(DEVICE_NAME, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 4, gfx);
		gfx->setCursor(0, 0);
		gfx->setTextColor(WHITE);
		gfx->setTextSize(1);
	}

	// Called when new data is coming from computer
	void read() {
		if (!hasReceivedData) {
			hasReceivedData = true;
			gfx->fillScreen(BLACK);
		}
		String full = "";

		speed = FlowSerialReadStringUntil(';').toInt();
		gear = FlowSerialReadStringUntil(';');
		rpmPercent = FlowSerialReadStringUntil(';').toInt();
		rpmRedLineSetting = FlowSerialReadStringUntil(';').toInt();
		currentLapTime = FlowSerialReadStringUntil(';');
		lastLapTime = FlowSerialReadStringUntil(';');
		bestLapTime = FlowSerialReadStringUntil(';');
		sessionBestLiveDeltaSeconds = FlowSerialReadStringUntil(';');
		sessionBestLiveDeltaProgressSeconds = FlowSerialReadStringUntil(';');
		tyrePressureFrontLeft  = FlowSerialReadStringUntil(';');
		tyrePressureFrontRight  = FlowSerialReadStringUntil(';');
		tyrePressureRearLeft  = FlowSerialReadStringUntil(';');
		tyrePressureRearRight  = FlowSerialReadStringUntil(';');
		tcLevel  = FlowSerialReadStringUntil(';');
		tcActive  = FlowSerialReadStringUntil(';');
		absLevel  = FlowSerialReadStringUntil(';');
		absActive  = FlowSerialReadStringUntil(';');
		isTCCutNull  = FlowSerialReadStringUntil(';');
		tcTcCut  = FlowSerialReadStringUntil(';');
		brakeBias  = FlowSerialReadStringUntil(';');
		brake  = FlowSerialReadStringUntil(';');
		lapInvalidated  = FlowSerialReadStringUntil(';');

		const String rest = FlowSerialReadStringUntil('\n');
	}

	// Called once per arduino loop, timing can't be predicted, 
	// but it's called between each command sent to the arduino
	void loop() {
		if (!hasReceivedData) {
			return;
		}
		drawRpmMeter(0, 0, SCREEN_WIDTH, CELL_HEIGHT);
		// this takes 2 cells in height, hence CELL_HEIGHT is the half point
		drawGear(COL[2] + HALF_CELL_WIDTH, ROW[1] + CELL_HEIGHT);

		// First+Second Column (Lap times)
		drawCell(COL[0], ROW[1], bestLapTime, "bestLapTime", "Best Lap", "left");
		drawCell(COL[0], ROW[2], lastLapTime, "lastLapTime", "Last Lap", "left");
		drawCell(COL[0], ROW[3], currentLapTime, "currenLapTime", "Current Lap", "left", lapInvalidated == "True" ? RED : WHITE);

		// Third Column (speed)
		drawCell(COL[2], ROW[3], speed, "speed", "Speed", "center");

		// Fourth+Fifth Column (delta)
		drawCell(SCREEN_WIDTH, ROW[1], sessionBestLiveDeltaSeconds, "sessionBestLiveDeltaSeconds", "Delta", "right", sessionBestLiveDeltaSeconds.indexOf('-') >= 0 ? GREEN : RED);
		drawCell(SCREEN_WIDTH, ROW[2], sessionBestLiveDeltaProgressSeconds, "sessionBestLiveDeltaProgressSeconds", "Delta P", "right", sessionBestLiveDeltaProgressSeconds.indexOf('-') >= 0 ? GREEN : RED);
		

		// (TC, ABS, BB)
		if (isTCCutNull == "False") {
			drawCell(COL[0], ROW[4], tcTcCut, "tcTcCut", "TC TC2", "center", YELLOW);
		} else {
			drawCell(COL[0], ROW[4], tcLevel, "tcLevel", "TC", "center", YELLOW);
		}
		drawCell(COL[1], ROW[4], absLevel, "absLevel", "ABS", "center", BLUE);
		drawCell(COL[2], ROW[4], brakeBias, "brakeBias", "BB", "center", MAGENTA);

		// (tyre pressure)
		drawCell(COL[3], ROW[3], tyrePressureFrontLeft, "tyrePressureFrontLeft", "FL", "center", CYAN);
		drawCell(COL[4], ROW[3], tyrePressureFrontRight, "tyrePressureFrontRight", "FR", "center", CYAN);
		drawCell(COL[3], ROW[4], tyrePressureRearLeft, "tyrePressureRearLeft", "RL", "center", CYAN);
		drawCell(COL[4], ROW[4], tyrePressureRearRight, "tyrePressureRearRight", "RR", "center", CYAN);
	}

	void idle() {
	}

	void drawGear(int32_t x, int32_t y)
	{
		// draw gear only when it changes
		if (gear != prev_gear)
		{
			gfx->setTextColor(YELLOW, BLACK);
			auto fontSize = 10;
			drawCentreCentreString(gear, x, y, fontSize, gfx, 1 * PIXEL_PER_MM, 0.5 * PIXEL_PER_MM);
			prev_gear = gear;
		}
	}

	boolean isDrawGearRpmRedRec()
	{
		if (rpmPercent >= rpmRedLineSetting)
		{
			return true;
		}
		return false;
	}

	void drawRpmMeter(int32_t x, int32_t y, int width, int height)
	{
		int meterWidth = (width * rpmPercent) / 100;

		int yPlusOne = y + 1;
		int innerWidth = width - meterWidth - 1;
		int innerHeight = height - 4;

		if (prev_rpmPercent > rpmPercent)
		{
			gfx->fillRect(meterWidth, yPlusOne, innerWidth, innerHeight, BLACK); // clear the part after the current rect width
		}

		if (rpmPercent >= rpmRedLineSetting)
		{
			gfx->fillRect(x, yPlusOne, meterWidth - 2, innerHeight, RED);
		}
		else if (rpmPercent >= rpmRedLineSetting - 5)
		{
			gfx->fillRect(x, yPlusOne, meterWidth - 2, innerHeight, ORANGE);
		}
		else
		{
			gfx->fillRect(x, yPlusOne, meterWidth - 2, innerHeight, GREEN);
		}

		// draw the frame only if it's not there
		if (prev_rpmPercent == 50) gfx->drawRect(x, y, width, height-2, WHITE);
		
		prev_rpmPercent = rpmPercent;
	}

	void drawCell(int32_t x, int32_t y, String data, String id, String name = "Data", String align = "center", int32_t color = WHITE, int fontSize = 3)
	{
		if (cellTitleHeight == 0) {
			gfx->setTextSize(2);
			int16_t x1 = 0;
			int16_t y1 = 0;
			uint16_t width = 0;
			uint16_t height = 0;
			gfx->getTextBounds(name, 0, 0, &x1, &y1, &width, &height);
			cellTitleHeight = height;
		}
		const static int hPadding = 5;
		const static int vPadding = 4;
		const static int titleAreaHeight = cellTitleHeight + 8;

		gfx->setTextColor(color, BLACK);

		const bool dataChanged =  (prevData[id] != data);
		const bool colorChanged =  (prevColor[id] != color);

		if (dataChanged) {

			if (align == "left")
			{
				
				if (colorChanged) gfx->drawRoundRect(x, y, CELL_WIDTH * 2 - 1, CELL_HEIGHT - 2, 4, color);		// Rectangle
				if (colorChanged) drawString(name, x + hPadding, y + vPadding, 2, gfx);						// Title
				drawString(data, x + hPadding, y + titleAreaHeight, fontSize, gfx); // Data
			}
			else if (align == "right")
			{
				if (colorChanged) gfx->drawRoundRect(x - (CELL_WIDTH * 2), y, CELL_WIDTH * 2 - 1, CELL_HEIGHT - 2, 5, color); // Rectangle
				if (colorChanged) drawRightString(name, x - hPadding, y + vPadding, 2, gfx);						// Title
				drawRightString(data, x - hPadding, y + titleAreaHeight, fontSize, gfx);	  // Data
			}
			else // "center"
			{
				if (colorChanged) gfx->drawRoundRect(x, y, CELL_WIDTH - 2, CELL_HEIGHT - 2, 5, color);	 // Rectangle
				if (colorChanged) drawCentreString(name, x + HALF_CELL_WIDTH, y + vPadding, 2, gfx);			 // Title
				drawCentreString(data, x + HALF_CELL_WIDTH, y + titleAreaHeight, fontSize, gfx); // Data
			}

			// Clean the previous data if it was wider
			if (prevData[id].length() > data.length())
			{
				// variables where we will store the results of getTextBounds
				int16_t x1 = 0;
				int16_t y1 = 0;
				uint16_t width = 0;
				uint16_t height = 0;
				
				auto dataY = y + titleAreaHeight;
				// calculate the size of the rectangle to "clear"
				gfx->getTextBounds(prevData[id], x, dataY, &x1, &y1, &width, &height);

				// depending on the datum of our text, we need to adjust the coordinates, because our text
				//  has different boundaries
				if (align == "left")
				{
					clearTextArea(x + hPadding, dataY, width, height, Datum::left_top, gfx);
				}
				else if (align == "right")
				{
					clearTextArea(x - hPadding, dataY, width, height, Datum::right_top, gfx);
				}
				else
				{
					clearTextArea(x + HALF_CELL_WIDTH, dataY, width, height, Datum::center_top, gfx);
				}
			}

			prevData[id] = data;
			prevColor[id] = color;
		}

	}
};
#endif
