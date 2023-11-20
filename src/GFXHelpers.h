#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#pragma once

enum Datum { left_top = 1, center_top = 2, right_top = 3, center_center = 4 };


int adjustX(int posX, int width, Datum align) {
	switch(align) {
		case Datum::left_top:
			// the X coordinate references the left edge of the rect
			return posX;
			break;
		case Datum::center_center:
		case Datum::center_top:
			// the X coordinate references the center point of the rect
			return posX - width /2;
			break;
		case Datum::right_top:
			// the X coordinate references the right edge of the rect
			return posX - width;
			break;
	}
}

int adjustY(int posY, int height, Datum align) {
	switch(align) {
		case Datum::left_top:
		case Datum::center_top:
		case Datum::right_top:
			return posY;
			break;
		case Datum::center_center:
			return posY - height /2;
			break;
	}
}

/**
 * Draw at a position, with a specified fixed reference.
 *  If datum is set as Center-Center, it means that the coordinates are given at the center point of whatever text is passed
 *  so, the text will be centered around those coordinates
 * 
 *  Font sprites may not be centered in their allocated area. Use offsets to better center text. 
 * 	 Positive offset in X moves text right; Positive offset in Y moves text down
 */
void drawStringWithDatum(String text, int posX, int posY, int fontSize, Datum datum, Arduino_GFX *gfx, int xOffset = 0, int yOffset = 0) {
	gfx->setTextSize(fontSize);
	int16_t x1 = 0;
	int16_t y1 = 0;
	uint16_t width = 0;
	uint16_t height = 0;
	gfx->getTextBounds(text, 0, 0, &x1, &y1, &width, &height);

	auto adjustedX = adjustX(posX, width, datum) + xOffset;
	auto adjustedY = adjustY(posY, height, datum) + yOffset;
	gfx->setCursor(adjustedX, adjustedY);
	gfx->print(text);
	// gfx->drawRect(adjustedX, adjustedY, width, height, RED);
}


void drawString(String text, int posX, int posY, int fontSize, Arduino_GFX *gfx, int xOffset = 0, int yOffset = 0) {
	drawStringWithDatum(text, posX, posY, fontSize, Datum::left_top, gfx, xOffset, yOffset);
}

void drawRightString(String text, int posX, int posY, int fontSize, Arduino_GFX *gfx, int xOffset = 0, int yOffset = 0) {
	drawStringWithDatum(text, posX, posY, fontSize, Datum::right_top, gfx, xOffset, yOffset);
}

void drawCentreString(String text, int posX, int posY, int fontSize, Arduino_GFX *gfx, int xOffset = 0, int yOffset = 0) {
	drawStringWithDatum(text, posX, posY, fontSize, Datum::center_top, gfx, xOffset, yOffset);
}

void drawCentreCentreString(String text, int posX, int posY, int fontSize, Arduino_GFX *gfx, int xOffset = 0, int yOffset = 0) {
	drawStringWithDatum(text, posX, posY, fontSize, Datum::center_center, gfx, xOffset, yOffset);
}


void clearTextArea(int posX, int posY, int width, int height, Datum align, Arduino_GFX *gfx, uint16_t clearColor = BLACK) {
	auto finalX = adjustX(posX, width, align);
	auto finalY = adjustY(posY, height, align);
	gfx->fillRect(finalX, finalY, width, height, clearColor);
}


int terminalCursor = 0;

void clearTerminal(Arduino_GFX *gfx) {
	gfx->fillScreen(BLACK);
	terminalCursor = 0;
}

void terminalPrintln(String text, Arduino_GFX *gfx) {
	if (terminalCursor > 200) {
		clearTerminal(gfx);
	}
	gfx->setCursor(0, terminalCursor * 12);
	gfx->setTextColor(WHITE);
	gfx->setTextSize(1);
	gfx->println(text);
	terminalCursor++;
}

