//     $Date: 2018-05-22 06:24:02 +1000 (Tue, 22 May 2018) $
// $Revision: 1330 $
//   $Author: Peter $

#include "Ass-03.h"

//
// Task that reads input from the front panel display. It uses a timer
// to periodically signal the task to check if the touch panel is being
// pressed. If it has, the position location is put onto a message queue.
// Debouncing of the input is also done to ensure that multiple messages
// are not generated.
//
// Function also provided to read from the message queue.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//
// Suggested update is to send a character that represents the button
// being pressed rather than the position on the front panel. Can also improve
// the setting of the DEBOUNCE_COUNT limit

// The number of times to register the front panel as being pressed
#define ON_COUNT   1
#define OFF_COUNT 20

// function to check if button was touched
bool isPress(Rectangle rec, int x, int y)
{
	return (x > rec.x && y > rec.y && x < (rec.x + rec.width) && y < (rec.y + rec.height));
}

// convert x,y coordinates of touch to enum representation of button press
ButtonPress getButtonPress(int x, int y)
{
	for(int i = 0; i < sizeof(rectangles) / sizeof(Rectangle); i++)
	{
		if(isPress(rectangles[i], x, y)) {
			return rectangles[i].bp;
		}
	}

	return 0;
}

bool get_touch_point(Coordinate *display) {
	osMutexWait(myMutex01Handle, osWaitForever);
	uint8_t result = BSP_TP_GetDisplayPoint(display);
	osMutexRelease(myMutex01Handle);
	return result != 0;
}

void Ass_03_Task_03(void const * argument)
{
	int8_t pressed_count=ON_COUNT;  // Debounce counter (not pressed)
	uint16_t pressed_num=0;         // Number of times a key is pressed
	Coordinate display;             // Pressed location

	osSignalWait(1,osWaitForever);
	safe_printf("Hello from Task 3 - Front Panel (detects touch screen presses)\n");

	// loop forever
	while (1)
	{
		// get touch position
		if (get_touch_point(&display))
		{
			// Not pressed: reset debounce counter
			if (pressed_count < 0)
			{
				// increase press counter
				pressed_count++;
				if (pressed_count == 0)
				{
					pressed_count = ON_COUNT;
				}
			}
			else
			{
				pressed_count = ON_COUNT;
			}
		}
		else
		{
			// Pressed: count down debounce unless already expired
			if (pressed_count > 0)
			{
				pressed_count--;
				if (pressed_count == 0)
				{
					// Debounced: queue key pressed message
					pressed_num++;
					//safe_printf("Task 3: %d (sent %3d,%3d)\n", pressed_num, display.x, display.y);
					ButtonPress bp = getButtonPress(display.x, display.y);
					osMessagePut (myQueue01Handle, (uint32_t)(bp), 0);
					//osMessagePut (myQueue01Handle, (uint32_t)((display.x << 16) + display.y), 0);
					pressed_count = -OFF_COUNT;
				}
			}
			else
			{
				pressed_count = -OFF_COUNT;
			}
		}

		// Wait before checking key pressed again
		osSemaphoreWait(myBinarySem04Handle, osWaitForever);
	}
}
