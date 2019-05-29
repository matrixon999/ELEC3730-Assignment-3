#include "Ass-03.h"

uint8_t ReadJoystick()
{
  // read in values for all joystick pins
  uint8_t JOY_A = (((GPIO_TypeDef*)GPIOE)->IDR & JOY_A_Pin) != (uint32_t)GPIO_PIN_RESET;
  uint8_t JOY_B = (((GPIO_TypeDef*)GPIOE)->IDR & JOY_B_Pin) != (uint32_t)GPIO_PIN_RESET;
  uint8_t JOY_C = (((GPIO_TypeDef*)GPIOE)->IDR & JOY_C_Pin) != (uint32_t)GPIO_PIN_RESET;
  uint8_t JOY_D = (((GPIO_TypeDef*)GPIOE)->IDR & JOY_D_Pin) != (uint32_t)GPIO_PIN_RESET;

  // check all values, and return correct one.
  if(JOY_A == 0) return 'L';
  else if(JOY_B == 0) return 'U';
  else if(JOY_C == 0) return 'D';
  else if(JOY_D == 0) return 'R';
  else return 0;

  // yes this could have been done more elegantly, but for the sake of easy comprehension
}

uint8_t GetJoystick()
{
  // keep track of previous value
  static uint8_t prev = Left;

  // debouncing check
  // get the value once
  uint8_t joy_value = ReadJoystick();
  // wait 10 milliseconds
  HAL_Delay(10);
  // get the value again
  uint8_t joy_value2 = ReadJoystick();
  // make sure the two values are the same and that it is different to the previous state
  if(joy_value == joy_value2 && joy_value != prev)
  {
    // if so, set previous value, and return new
    prev = joy_value;
    return prev;
  }
  else
  {
    // else, return 0 for no change
    return 0;
  }
}



void run_snake_time()
{
	osMutexWait(myMutex01Handle, osWaitForever);
	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	osMutexRelease(myMutex01Handle);

	uint8_t Joystick;

	int width = 10;
	int height = 10;

	vector pos = {100, 100};
	int x_speed = 1;
	int y_speed = 0;

	int length = 1;
	vector* body = malloc(sizeof(vector) * length);
	if(body == NULL) {
		safe_printf(CONSOLE_RED("Snake couldn't allocate\n"));
		return;
	}
	body[length - 1] = pos;

	srand(HAL_GetTick());
	vector food = { (rand() % 32) * 10, (rand() % 24) * 10 };

	int score = 0;

	bool update = true;
	uint32_t start = HAL_GetTick();
	uint32_t last = HAL_GetTick();
	uint32_t elapsed = 0;
	uint16_t update_time = 250;

	while(1)
	{
		last = HAL_GetTick();
		elapsed += last - start;
		if(elapsed > update_time) {
			update = true;
			elapsed = 0;
		}
		start = HAL_GetTick();

		// get input
		Joystick = GetJoystick();

		switch(Joystick)
		{
			case Up: {
				x_speed = 0;
				y_speed = -1;
			} break;

			case Down: {
				x_speed = 0;
				y_speed = 1;
			} break;

			case Left: {
				x_speed = -1;
				y_speed = 0;
			} break;

			case Right: {
				x_speed = 1;
				y_speed = 0;
			} break;
		}

		if(update) {

			// logic section

			// draw over last snake bit with black
			osMutexWait(myMutex01Handle, osWaitForever);
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillRect(body[0].x-1, body[0].y-1, width, height);
			osMutexRelease(myMutex01Handle);

			// update head
			pos.x += x_speed * width;
			pos.y += y_speed * height;
			body[length - 1] = pos;

			// check self collision
			for(int i = 0; i < length-1; i++) {
				if(pos.x == body[i].x && pos.y == body[i].y)
				{
					safe_printf(CONSOLE_RED("You died\n"));
					osMutexWait(myMutex01Handle, osWaitForever);
					BSP_LCD_SetFont(&Font24);
					BSP_LCD_SetTextColor(LCD_COLOR_RED);
					BSP_LCD_DisplayStringAt(320 / 2, 240 / 2, "YOU DIED", CENTER_MODE);
					BSP_LCD_SetFont(&Font12);
					osMutexRelease(myMutex01Handle);
					set_snake_time(false);
					free(body);
					osDelay(3000);
					return;
				}
			}

			// update body parts
			for(int i = 0; i < length-1; i++) body[i] = body[i+1];

			// check wall collisions
			if(pos.x < 0 || pos.x > 320 || pos.y < 0 || pos.y > 240) {
				safe_printf(CONSOLE_RED("You died\n"));
				osMutexWait(myMutex01Handle, osWaitForever);
				BSP_LCD_SetFont(&Font24);
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				BSP_LCD_DisplayStringAt(320 / 2, 240 / 2, "YOU DIED", CENTER_MODE);
				BSP_LCD_SetFont(&Font12);
				osMutexRelease(myMutex01Handle);
				set_snake_time(false);
				free(body);
				osDelay(3000);
				return;
			}

			// draw new snake bit
			osMutexWait(myMutex01Handle, osWaitForever);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillRect(pos.x-1, pos.y-1, width, height);
			osMutexRelease(myMutex01Handle);

			// eat food
			if(pos.x == food.x && pos.y == food.y)
			{
				// update food and length
				length++;
				score++;

				// update speed
				update_time /= 1.10;

				// move food
				food.x = (rand() % 32) * 10;
				food.y = (rand() % 24) * 10;

				// allocate more memory for body parts
				body = realloc(body, sizeof(pos) * length);
				if(body == NULL) {
					safe_printf(CONSOLE_RED("Snake couldn't allocate\n"));
					return;
				}

				// add new body part
				body[length - 1] = pos;
			}


			//draw section

			osMutexWait(myMutex01Handle, osWaitForever);
			//BSP_LCD_Clear(LCD_COLOR_BLACK);

			// draw food
			BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
			BSP_LCD_FillRect(food.x-1, food.y-1, width, height);

			// draw score
			char *score_str;
			asprintf(&score_str, "%d", score);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 0, score_str, LEFT_MODE);
			free(score_str);

			osMutexRelease(myMutex01Handle);

			update = false;
		}
	}
}
