#include "Game.h"

Game::Game()
{
	mWindow = nullptr;
	mRenderer = nullptr;
	mIsRunning = true;
	mTicksCount = 0;
	mPaddlePos = { 0,0 };
	mPaddle2Pos = { 0,0 };
	mBallPos = { 0,0 };
	mBallVel = { 0,0 };
	mPaddleDir = 0;
	mPaddle2Dir = 0;
}

bool Game::Initialize()
{
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);

	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100, // Top left x-coordinate of window
		100, // Top left y-coordinate of window
		mWindowW, // Width of window
		mWindowH, // Height of window
		0 // Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1, // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	//Set values for inital gameobject positions;
	mPaddlePos.x = 40;
	mPaddlePos.y = mWindowH /2;
	mPaddle2Pos.x = mWindowW - 40;
	mPaddle2Pos.y = mWindowH / 2;
	//mBallPos.x = mWindowW / 2;
	//mBallPos.y = mWindowH / 2;

	ball1.ballPos.x = mWindowW / 2;
	ball1.ballPos.y = mWindowH / 2 + 20;
	ball2.ballPos.x = mWindowW / 2;
	ball2.ballPos.y = mWindowH / 2 - 20;

	//initial ball velocity
	ball1.ballVel = { -200.0f, 235.0f };
	ball2.ballVel = { 200.0f, -235.0f };

	//add balls to vector
	balls.push_back(ball1);
	balls.push_back(ball2);

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::Shutdown()
{
	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);
	SDL_Quit();
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If this is an SDL_QUIT event, end loop
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}

	mPaddle2Dir = 0;
	if (state[SDL_SCANCODE_I])
	{
		mPaddle2Dir -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		mPaddle2Dir += 1;
	}
}

void Game::UpdateGame()
{
	//wait until 16ms has elasped since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();

	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	//move paddle
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;

		// Make sure paddle doesn't move off screen!
		if (mPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos.y > (mWindowH - paddleH / 2.0f - thickness))
		{
			mPaddlePos.y = mWindowH - paddleH / 2.0f - thickness;
		}
	}

	//move paddle 2
	if (mPaddle2Dir != 0)
	{
		mPaddle2Pos.y += mPaddle2Dir * 300.0f * deltaTime;

		// Make sure paddle 2 doesn't move off screen!
		if (mPaddle2Pos.y < (paddleH / 2.0f + thickness))
		{
			mPaddle2Pos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddle2Pos.y > (mWindowH - paddleH / 2.0f - thickness))
		{
			mPaddle2Pos.y = mWindowH - paddleH / 2.0f - thickness;
		}
	}

	//move balls
	for (Ball& ball : balls)
	{
		ball.ballPos.x += ball.ballVel.x * deltaTime;
		ball.ballPos.y += ball.ballVel.y * deltaTime;
	}

	for (Ball& ball : balls)
	{
		//Collision with top wall
		if (ball.ballPos.y <= thickness && ball.ballVel.y < 0.0f)
		{
			ball.ballVel.y *= -1;
		}
		//Collision with bottom wall
		if (ball.ballPos.y >= mWindowH - thickness && ball.ballVel.y > 0.0f)
		{
			ball.ballVel.y *= -1;
		}
		//Collision with left wall
		if (ball.ballPos.x <= 0 && ball.ballVel.x < 0.0f)
		{
			ball.ballVel.x *= -1;
		}
		//Collision with right wall
		if (ball.ballPos.x >= mWindowW - thickness && ball.ballVel.x > 0.0f)
		{
			ball.ballVel.x *= -1;
		}

		//Collision with paddles
		float diff = abs((ball.ballPos.y + thickness / 2) - (mPaddlePos.y + paddleH / 2));

		//Collision with paddle 1
		if (
			// Our y-difference is small enough
			diff <= paddleH / 2.0f &&
			// Ball is at the correct x-position
			ball.ballPos.x + thickness/2 <= (mPaddlePos.x + paddleW + 1) && (ball.ballPos.x + thickness / 2 >= mPaddlePos.x) &&
			// The ball is moving to the left
			ball.ballVel.x < 0.0f
			)
		{
			ball.ballVel.x *= -1.0f;
		}

		//Collision with paddle 2
		float diff2 = abs((ball.ballPos.y + thickness / 2) - (mPaddle2Pos.y + paddleH / 2));

		if (
			// Our y-difference is small enough
			diff2 <= paddleH / 2.0f && 
			// Ball is at the correct x-position
			ball.ballPos.x + thickness / 2 <= (mPaddle2Pos.x + paddleW) && (ball.ballPos.x + thickness / 2 >= mPaddle2Pos.x - 1) &&
			// The ball is moving to the Right
			ball.ballVel.x > 0.0f
			)
		{
			ball.ballVel.x *= -1.0f;
		}

	}
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(
		mRenderer,
		0, // R
		0, // G 
		255, // B
		255 // A
	);

	SDL_RenderClear(mRenderer);

	//TODO - Drawing current scene 
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	SDL_Rect topWall {
		0, // Top left x
		0, // Top left y
		mWindowW, // Width
		thickness // Height
	};

	SDL_Rect bottomWall{
		0, // Top left x
		mWindowH - thickness, // Top left y
		mWindowW, // Width
		thickness // Height
	};

	SDL_Rect leftWall{
		0, // Top left x
		0, // Top left y
		thickness, // Width
		mWindowH // Height
	};

	SDL_Rect rightWall{
		mWindowW - thickness, // Top left x
		0, // Top left y
		thickness, // Width
		mWindowH // Height
	};

	SDL_Rect firstBall{
		static_cast<int>(balls[0].ballPos.x - thickness / 2),
		static_cast<int>(balls[0].ballPos.y - thickness / 2),
		thickness,
		thickness
	};

	SDL_Rect secondBall{
		static_cast<int>(balls[1].ballPos.x - thickness / 2),
		static_cast<int>(balls[1].ballPos.y - thickness / 2),
		thickness,
		thickness
	};

	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x - paddleW / 2),
		static_cast<int>(mPaddlePos.y - paddleH / 2),
		paddleW,
		paddleH
	};

	SDL_Rect paddle2{
		static_cast<int>(mPaddle2Pos.x - paddleW / 2),
		static_cast<int>(mPaddle2Pos.y - paddleH / 2),
		paddleW,
		paddleH
	};

	SDL_RenderFillRect(mRenderer, &topWall);
	SDL_RenderFillRect(mRenderer, &bottomWall);
	SDL_RenderFillRect(mRenderer, &leftWall);
	SDL_RenderFillRect(mRenderer, &rightWall);
	SDL_RenderFillRect(mRenderer, &firstBall);
	SDL_RenderFillRect(mRenderer, &secondBall);
	SDL_RenderFillRect(mRenderer, &paddle);
	SDL_RenderFillRect(mRenderer, &paddle2);

	SDL_RenderPresent(mRenderer);
}
