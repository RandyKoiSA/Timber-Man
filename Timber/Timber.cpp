// include important C++ libraries
#include "pch.h"
#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>
using namespace sf;

// Function declaration
void updateBranches(int seed);
const int NUM_BRANCHES = 6;
Sprite spriteBranch[NUM_BRANCHES];
// where is the player / branch
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

int main()
{
	// create a video mode object
	VideoMode vm(1920, 1080);
	// create and open a window for the game
	RenderWindow window(vm, "Timber Game", Style::Default);

	/**
	Background properties
	*/
	// create a texture to hold a graphic on the GPU
	Texture textureBackground;
	// load a graphic into the texture
	textureBackground.loadFromFile("graphics/background.png");
	// create a sprite
	Sprite spriteBackground;
	// attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);
	// set the spriteBackground the cover the screen
	spriteBackground.setPosition(0, 0);

	/**
	Tree Trunk properties
	*/
	// make a tree sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);
	// prepare the bee
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(100, 800);
	// is the bee currently moving?
	bool beeActive = false;
	// how fast can the bee fly
	float beeSpeed = 0.0f;

	/**
	Cloud properties
	*/
	// make 3 cloud sprites from 1 texture
	Texture textureCloud;
	// load 1 new texture
	textureCloud.loadFromFile("graphics/cloud.png");
	// create 3 new sprites with the same texsture
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);
	// position the cloud off screen
	spriteCloud1.setPosition(200, 0);
	spriteCloud2.setPosition(600, 250);
	spriteCloud3.setPosition(200, 300);
	// are the clouds currently on screen?
	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;
	// how fast is each cloud?
	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	// Variable to control time itself
	Clock clock;

	/**
	Time bar properties
	*/
	// Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setPosition(1920 / 2 - timeBarStartWidth / 2, 980);
	
	float gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;
	timeBar.setFillColor(Color::Red);
	// Trach whether the game is pause
	bool paused = true;

	/**
	Text and Font properties
	*/
	// Draw some text
	int score = 0;
	sf::Text messageText;
	sf::Text scoreText;
	// Font
	Font font;
	font.loadFromFile("fonts/ka1.ttf");
	// Set the font to our message
	messageText.setFont(font);
	scoreText.setFont(font);
	// Assign the actual message
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");
	// Make the messages font size bigger
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);
	// Choose a color for the Text
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);
	// Set position
	messageText.setPosition(1920 / 2, 540);
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(
		textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);

	/**
	Tree Branches properties
	*/
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");
	// set the texture for each sprite
	for (int i = 0; i < NUM_BRANCHES; i++) {
		spriteBranch[i].setTexture(textureBranch);
		spriteBranch[i].setPosition(-2000, -2000);
		// set sprite origin to dead center
		// we can then spin it around without changing its position
		spriteBranch[i].setOrigin(220, 20);
	}

	/**
	Player properties
	*/
	// Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);
	// the player stars on the left side
	side playerSide = side::LEFT;
	
	/**
	Gravestone properties
	*/
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(675, 2000);

	/**
	Axe properties
	*/
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);
	// line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	/**
	Flying log properties
	*/
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);
	// some other useful log related to variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	// conrol the player input
	bool acceptInput = false;

	/**
	Sound properties
	*/
	SoundBuffer soundbufferChop;
	soundbufferChop.loadFromFile("sounds/chop.wav");
	Sound soundChop;
	soundChop.setBuffer(soundbufferChop);

	SoundBuffer soundbufferDeath;
	soundbufferDeath.loadFromFile("sounds/death.wav");
	Sound soundDeath;
	soundDeath.setBuffer(soundbufferDeath);

	SoundBuffer soundbufferOOT;
	soundbufferOOT.loadFromFile("sounds/out_of_time.wave");
	Sound soundOutOfTime;
	soundOutOfTime.setBuffer(soundbufferOOT);

	/**
	Additional Tree properties
	*/
	Texture textureTree2;
	textureTree2.loadFromFile("graphics/tree2.png");
	Sprite spriteTree2;
	Sprite spriteTree3;
	Sprite spriteTree4;
	Sprite spriteTree5;
	Sprite spriteTree6;
	Sprite spriteTree7;
	spriteTree2.setTexture(textureTree2);
	spriteTree3.setTexture(textureTree2);
	spriteTree4.setTexture(textureTree2);
	spriteTree5.setTexture(textureTree2);
	spriteTree6.setTexture(textureTree2);
	spriteTree7.setTexture(textureTree2);
	
	spriteTree2.setPosition(100, -50);
	spriteTree3.setPosition(1200, -100);
	spriteTree4.setPosition(1500, -20);
	spriteTree5.setPosition(1800, -200);
	spriteTree6.setPosition(600, -100);
	spriteTree7.setPosition(0, -200);

	spriteTree2.setScale(Vector2f(.8, 1));
	spriteTree3.setScale(Vector2f(.6, 1));
	spriteTree4.setScale(Vector2f(.7, 1));
	spriteTree5.setScale(Vector2f(.3, 1));
	spriteTree6.setScale(Vector2f(.6, 1));
	spriteTree7.setScale(Vector2f(.5, 1));
	/**
	HUD background
	*/
	RectangleShape timeBarBackground;
	timeBarBackground.setFillColor(Color::White);
	timeBarBackground.setSize(Vector2f(420, 100));
	timeBarBackground.setPosition((1920 / 2) - 210, 970);
	while(window.isOpen()){
		Event evnt;
		/*
		*******************
		Handle events
		*******************
		*/
		while (window.pollEvent(evnt)) {
			switch (evnt.type) {
			case Event::Closed:
				window.close();
				break;
			case Event::KeyReleased:
				if (!paused) {
					acceptInput = true;
					// hide the axe
					spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
				}
				break;
			}
		}

		/*
		*******************
		Handle players input
		*******************
		*/
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			// Reset the time and the score
			score = 0;
			timeRemaining = 5;

			// Make all the branches disapeear
			for (int i = 1; i < NUM_BRANCHES; i++) {
				branchPositions[i] = side::NONE;
			}

			// Make sure the gravestone is hidden
			spriteRIP.setPosition(675, 2000);
			// Move player into position
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}

		// Wrap the player controls to make sure we are accepting input
		if (acceptInput) {
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				playerSide = side::RIGHT;
				score++;

				timeRemaining += (2 / score) + .15 ;


				if (timeRemaining > 5) {
					timeRemaining = 5;
				}

				spriteAxe.setPosition(
					AXE_POSITION_RIGHT,
					spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 720);

				// update the branches
				updateBranches(score);

				// set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				soundChop.play();
			}
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				playerSide = side::LEFT;
				score++;
				timeRemaining += (2 / score) + .15;

				if (timeRemaining > 5) {
					timeRemaining = 5;
				}

				spriteAxe.setPosition(
					AXE_POSITION_LEFT,
					spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);
				
				updateBranches(score);

				spriteLog.setPosition(810, 720);

				logSpeedX = 5000;
				logActive = true;
				acceptInput = false;

				soundChop.play();
			}
		}

		/*
		*******************
		Updating the scene
		*******************
		*/
		if (!paused) {
			Time deltaTime = clock.restart();

			// Subtract from the amount of time remaining
			timeRemaining -= deltaTime.asSeconds();
			// size up the time bar
			timeBar.setSize(Vector2f(
				timeBarWidthPerSecond * timeRemaining,
				timeBarHeight));

			/** 
			When time reaches 0
			*/
			if (timeRemaining <= 0.0f) {
				// pause the game
				paused = true;

				// change the message shown to the player
				messageText.setString("Game Over!");

				// Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(
					textRect.left + textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				soundOutOfTime.play();
			}

			// Setup bee
			if (!beeActive) {
				// how fast is the bee
				srand((int)time(0) * 10);
				beeSpeed = (rand() % 200) + 200;

				// how high is the bee
				srand((int)time(0) * 10);
				float height = ((int)rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;
			}
			else {
				// move the bee
				spriteBee.setPosition(spriteBee.getPosition().x -
					(beeSpeed * deltaTime.asSeconds()),
					spriteBee.getPosition().y);

				if (spriteBee.getPosition().x < -100) {
					// set it up ready to be a whole cloud next frame
					beeActive = false;
				}
			}

			// Setup cloud 1
			if (!cloud1Active) {
				// how fast is the cloud
				srand((int)time(0) * 20);
				cloud1Speed = (rand() % 200);

				// how high is the cloud
				srand((int)time(0) * 20);
				float height = (rand() % 150);
				spriteCloud1.setPosition(-200, height);
				cloud1Active = true;
			}
			else {
				// move the bee
				spriteCloud1.setPosition(spriteCloud1.getPosition().x +
					(cloud1Speed * deltaTime.asSeconds()),
					spriteCloud1.getPosition().y);

				// has cloud 1 reach the right hand edge of the screen?
				if (spriteCloud1.getPosition().x > 1920) {
					cloud1Active = false;
				}
			}

			// Setup cloud 2
			if (!cloud2Active) {
				// how fast is cloud 2 going
				srand((int)time(0) * 15);
				cloud2Speed = (rand() % 200);
				// how high is the cloud
				srand((int)time(0) * 15);
				float height = (rand() % 300) - 150;
				spriteCloud2.setPosition(-200, height);
				cloud2Active = true;
			}
			else {
				spriteCloud2.setPosition(spriteCloud2.getPosition().x +
					(cloud2Speed * deltaTime.asSeconds()),
					spriteCloud2.getPosition().y);
				if (spriteCloud2.getPosition().x > 1920) {
					cloud2Active = false;
				}
			}

			// Setup cloud 3
			if (!cloud3Active) {
				// how fast is cloud 3 going
				srand((int)time(0) * 10);
				cloud3Speed = (rand() % 200);

				// how high is the cloud
				srand((int)time(0) * 30);
				float height = (rand() % 450) - 150;
				spriteCloud3.setPosition(-200, height);
				cloud3Active = true;
			}
			else {
				spriteCloud3.setPosition(spriteCloud3.getPosition().x +
					(cloud3Speed * deltaTime.asSeconds()),
					spriteCloud3.getPosition().y);

				if (spriteCloud3.getPosition().x > 1920) {
					cloud3Active = false;
				}
			}

			// update the score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			// update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++) {
				float height = i * 150;
				if (branchPositions[i] == side::LEFT) {
					// move the sprite to the left side
					spriteBranch[i].setPosition(610, height);
					// flip the sprite around the other way
					spriteBranch[i].setRotation(180);
				}
				else if(branchPositions[i] == side::RIGHT){
					// move the sprite to the rigth side
					spriteBranch[i].setPosition(1330, height);
					spriteBranch[i].setRotation(0);
				}
				else {
					// hide the branch
					spriteBranch[i].setPosition(3000, height);
				}
			}

			// setup log
			if (logActive) {
				spriteLog.setPosition(
					spriteLog.getPosition().x +
					(logSpeedX * deltaTime.asSeconds()),
					spriteLog.getPosition().y +
					(logSpeedY * deltaTime.asSeconds())
				);

				// has the log reached the right hand edge?
				if (spriteLog.getPosition().x < -100 ||
					spriteLog.getPosition().x > 2000) {
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			// has the player been spuished by a branch
			if (branchPositions[5] == playerSide) {

				// death
				paused = true;
				acceptInput = false;

				// draw the gravestone
				if (playerSide == side::LEFT) {
					spriteRIP.setPosition(525, 760);
				}
				else if (playerSide == side::RIGHT) {
					spriteRIP.setPosition(1200, 760);
				}

				// hide the player
				spritePlayer.setPosition(2000, 660);
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
				spriteLog.setPosition(2000, 660);
				// change the txt of the message
				messageText.setString("You've been squished");

				// center it on the screen
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(
					textRect.left +
					textRect.width / 2.0f,
					textRect.top +
					textRect.height / 2.0f
				);

				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				soundDeath.play();

			}
		} // end of if(!pause)



		/* 
		*******************
		Draw the scene
		*******************
		*/
		// clear the screen of window
		window.clear();

		window.draw(spriteBackground);

		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);


		window.draw(spriteTree7);
		window.draw(spriteTree6);
		window.draw(spriteTree5);
		window.draw(spriteTree2);
		window.draw(spriteTree3);
		window.draw(spriteTree4);

		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(spriteBranch[i]);
		}
		window.draw(spriteTree);
		window.draw(spritePlayer);
		window.draw(spriteAxe);
		window.draw(spriteLog);
		window.draw(spriteRIP);

		window.draw(spriteBee);

		window.draw(scoreText);

		window.draw(timeBarBackground);
		window.draw(timeBar);
		// show everything that we just drew
		if (paused) window.draw(messageText);

		window.display();
		 
	}
	return 0;
}


// updating tree branchs
void updateBranches(int seed) {
	// move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}

	// spawn a new branch at position 0
	// LEFT, RIGHT, or NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;
	}

}