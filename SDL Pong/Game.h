#pragma once
#include<SDL2/SDL.h>
#include<vector>

struct Vector2
{
	float x;
	float y;
};

struct Ball
{
	Vector2 ballPos;
	Vector2 ballVel;
};

class Game
{
public:
	Game();
	// Initialize the game
	bool Initialize();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void Shutdown();
private:
	// Helper functions for the game loop
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	// Window created by SDL
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	//Window Variables
	const int mWindowH = 768;
	const int mWindowW = 1024;

	//Delta Time
	Uint32 mTicksCount;

	//Game Logic
	int mPaddleDir;
	int mPaddle2Dir;
	std::vector<Ball> balls;

	//Game objects
	Vector2 mPaddlePos, mPaddle2Pos, mBallPos, mBallVel;
	Ball ball1, ball2;
	const int paddleH = 100;
	const int paddleW = 15;
	const int thickness = 15;

	// Game should continue to run
	bool mIsRunning;
};

